#include "GestureNetTorch.hpp"

/*-------------------------------------------------------
                    Net Definition
-------------------------------------------------------*/

//GestureNetTorch::Net::Net()
//{
//}

// Depracated
//torch::Tensor GestureNetTorch::Net::forward(torch::Tensor x)
//{
//    auto [lstm1_out, hidden1] = lstm1->forward(x, hidden1_);
//    auto [lstm2_out, hidden2] = lstm2->forward(lstm1_out, hidden2_);
//    hidden1_ = hidden1;
//    hidden2_ = hidden2;
//    auto fc1_out = F::relu(fc1->forward(lstm2_out));
//    auto fc2_out = F::softmax(fc2->forward(fc1_out),
//                            torch::nn::functional::SoftmaxFuncOptions(1));
//    return fc2_out;
//}

/*-------------------------------------------------------
                    GestureNetTorch
                     Main Functions
-------------------------------------------------------*/

int GestureNetTorch::getPred(vector<float>& _input)
{
    /***************************************************
    *   Returns:    0:  No new gesture (not gesture)
                    1:  Left Swing
                    2:  Right Swing
                    3:  Double Click
    ****************************************************/

    // first converting input into pyTorch Tensor
    auto opts = torch::TensorOptions().dtype(torch::kFloat32);
    torch::Tensor input = torch::from_blob( _input.data(),
                                            { static_cast<long long>(_input.size()) },
                                            opts);
    input = input.resize_({ 1, FEAT_LEN });
    // Determines if it is noise
    int vadResult = getVADPred(input);
    if (vadResult == 1)
    {
        addToBuffer(input);
    } else {
        // clear the feature buffer
        featBuffer = torch::zeros( { 0 } );
        currentGest = 0;
    }

    // Predict the actual gesture if the buffer is ready
    if (gestPredRdy())
    {
        int gestPred = getGestPred();
        if (gestPred == currentGest)
        {
            return 0;           // returns 0 if the gesture
                                // has not changed
        } else {
            return gestPred;
        }
    }
    
    return 0;
}
/*-------------------------------------------------------
                    GestureNetTorch
                        Helpers
-------------------------------------------------------*/
int GestureNetTorch::getVADPred(torch::Tensor& input)
{
    stackedInput[0] = input.resize_({ 1, 1, 11 });
    auto outputs = vadModule.forward(stackedInput).toTuple();

    // update lstm states
    for (size_t i = 1; i < stackedInput.size(); ++i)
    {
        stackedInput[i] = outputs->elements()[i].toTensor();
    }

    // extracting the first tensor as output prediction
    vector<float> pred;
    auto out = outputs->elements()[0].toTensor();
    auto out_acc = out.accessor<float, 1>();
    pred.push_back(out_acc[0]);
    pred.push_back(out_acc[1]);

    DBOUT << "Pred:" << pred << endl;
    return rankPred(pred);
}

int GestureNetTorch::getGestPred()
{
    auto input_ = featBuffer.reshape({60, 1, 11});
    vector<jit::IValue> inputs;
    inputs.emplace_back(input_);
    //auto output = gestModule.forward(inputs).toTensor();
    auto output = torch::rand({ 4 });

    // converting output to std vector
    auto output_acc = output.accessor<float, 1>();
    vector<float> output_(NUM_GEST);
    for (size_t i = 0; i < NUM_GEST; ++i)
    {
        output_[i] = output_acc[i];
    }
    return rankPred(output_);
}

bool GestureNetTorch::gestPredRdy()
{
    bool result;
    result = (featBuffer.size(0) >= 60 );
    return result;
}

void GestureNetTorch::addToBuffer(torch::Tensor& input)
{
    // adding to the buffer
    if (featBuffer.size(0) == 0) {
        featBuffer = input;
    }
    else {
        auto newfeatBuffer  = torch::cat({ featBuffer, input }, 0);
        featBuffer = newfeatBuffer;
    }
    
    // remove the extra records
    if (featBuffer.size(0) > 60) {
        featBuffer = featBuffer.index({ indexing::Slice(1,
                                                    indexing::None,
                                                    indexing::None)});
    }
}

/*-------------------------------------------------------
                    GestureNetTorch
                Constructors & Desctructors
-------------------------------------------------------*/

GestureNetTorch::GestureNetTorch(string vadName, string gestName) :
    featBuffer{ torch::zeros({ 0 }) }
    //net{std::make_shared<Net>()}
{
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        vadModule = jit::load(vadName);
        gestModule = jit::load(gestName);
    }
    catch (const c10::Error& e) {
        std::cerr << "error loading models\n";
        exit(-1);
    }
    DBOUT << "<-----------------VAD Model Parameters------------------>" << endl;
    for (auto& param : vadModule.named_parameters())
    {
        DBOUT << param.name << endl;
    }
    DBOUT << "<-----------------Gest Model Parameters----------------->" << endl;
    for (auto& param : gestModule.named_parameters())
    {
        DBOUT << param.name << endl;
    }
    DBOUT << "<------------------------------------------------------->" << endl;
    DBOUT << "TorchScript Model Loaded." << endl;

    stackedInput.emplace_back( torch::zeros({ 1, 1, 11 }) );
    stackedInput.emplace_back( torch::zeros({ 1, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 1, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 2, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 2, 1, 64 }) );
}