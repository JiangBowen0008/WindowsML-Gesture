#include "GestureNetTorch.hpp"

/*-------------------------------------------------------
                    Net Definition
-------------------------------------------------------*/

GestureNetTorch::Net::Net()
{
}

torch::Tensor GestureNetTorch::Net::forward(torch::Tensor x)
{
    auto [lstm1_out, hidden1] = lstm1->forward(x, hidden1_);
    auto [lstm2_out, hidden2] = lstm2->forward(lstm1_out, hidden2_);
    hidden1_ = hidden1;
    hidden2_ = hidden2;
    auto fc1_out = F::relu(fc1->forward(lstm2_out));
    auto fc2_out = F::softmax(fc2->forward(fc1_out),
                            torch::nn::functional::SoftmaxFuncOptions(1));
    return fc2_out;
}

/*-------------------------------------------------------
                    GestureNetTorch
                     Main Functions
-------------------------------------------------------*/

int GestureNetTorch::getPred(vector<float>& _input)
{
    auto opts = torch::TensorOptions().dtype(torch::kFloat32);
    torch::Tensor input = torch::from_blob( _input.data(), 
                                            {static_cast<long long>(_input.size())}, 
                                            opts);
    stackedInput[0] = input.resize_({1, 1, 11});
    auto outputs = module.forward(stackedInput).toTuple();
    for (size_t i = 1; i < stackedInput.size(); ++i)
    {
        stackedInput[i] = outputs->elements()[i].toTensor();
    }
    vector<float> pred;
    auto out = outputs->elements()[0].toTensor();
    auto out_acc = out.accessor<float, 1>();
    pred.push_back(out_acc[0]);
    pred.push_back(out_acc[1]);

    DBOUT << "Pred:" << pred << endl;
    return rankPred(pred);
}

/*-------------------------------------------------------
                    GestureNetTorch
                Constructors & Desctructors
-------------------------------------------------------*/

GestureNetTorch::GestureNetTorch(string modelName) :
    net{std::make_shared<Net>()}
{
    try {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        module = jit::load(modelName);
    }
    catch (const c10::Error& e) {
        std::cerr << "error loading the model\n";
        exit(-1);
    }
    DBOUT << "<------------------Model Parameters--------------------->" << endl;
    for (auto& param : module.named_parameters())
    {
        DBOUT << param.name << endl;
    }
    DBOUT << "<------------------------------------------------------->" << endl;

    stackedInput.emplace_back( torch::zeros({ 1, 1, 11 }) );
    stackedInput.emplace_back( torch::zeros({ 1, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 1, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 2, 1, 64 }) );
    stackedInput.emplace_back( torch::zeros({ 2, 1, 64 }) );
   
    DBOUT << "TorchScript Model Loaded." << endl;
}