#include "GestureNetONNX.hpp"

/*----------------------------------------------------------------
                         Main Functions
----------------------------------------------------------------*/

int GestureNetONNX::getPred(vector<float>& _input)
{
    size_t input_tensor_size = _input.size();
    vector<float>& input_tensor_values = _input;

    // create input tensor object from data values
    packed_input[0] = Ort::Value::CreateTensor<float>(  memory_info,
                                                        input_tensor_values.data(),
                                                        input_tensor_size,
                                                        input_node_dims[0].data(),
                                                        3);
    //assert(packed_input.size() == 5 && packed_input.front().IsTensor());
    // score model & input tensor, get back output tensor
    auto output_tensors = session->Run( Ort::RunOptions{ nullptr },
                                        input_node_names.data(),
                                        packed_input.data(),
                                        5,
                                        output_node_names.data(),
                                        5/*output_node_names.size()*/);
    assert(output_tensors.size() == 5 && output_tensors.front().IsTensor());
    // assign lstm buffers
    for (size_t idx = 1; idx < 5; ++idx)
    {
        auto& temp = output_tensors[idx];
        float* tempPtr = output_tensors[idx].GetTensorMutableData<float>();
        packed_input[idx] = Ort::Value::CreateTensor<float>(memory_info,
                                                            tempPtr,
                                                            temp.GetCount(),
                                                            input_node_dims[idx].data(),
                                                            3);
    }

    
    // Get pointer to output tensor float values
    float* floatarr;
    floatarr = output_tensors.front().GetTensorMutableData<float>();
    vector<float> preds(floatarr, floatarr + 2);
    
    return rankPred(preds);
}


/*----------------------------------------------------------------
                        Helper Functions
----------------------------------------------------------------*/


/*----------------------------------------------------------------
                   Constructors & Destructors
----------------------------------------------------------------*/

GestureNetONNX::GestureNetONNX(string modelName) :
    env{ ORT_LOGGING_LEVEL_WARNING, "test" },
    output_node_names{"softmax"},
    lstm1_buffer(128),
    lstm2_buffer(128),
    memory_info{ Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault) }
{
    // initialize session options if needed
    session_options.SetIntraOpNumThreads(1);

    // If onnxruntime.dll is built with CUDA enabled, we can uncomment out this line to use CUDA for this
    // session (we also need to include cuda_provider_factory.h above which defines it)
    // #include "cuda_provider_factory.h"
    // OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 1);

    // Sets graph optimization level
    // Available levels are
    // ORT_DISABLE_ALL -> To disable all optimizations
    // ORT_ENABLE_BASIC -> To enable basic optimizations (Such as redundant node removals)
    // ORT_ENABLE_EXTENDED -> To enable extended optimizations (Includes level 1 + more complex optimizations like node fusions)
    // ORT_ENABLE_ALL -> To Enable All possible opitmizations
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    std::wstring widestr = std::wstring(modelName.begin(), modelName.end());
    const wchar_t* model_path = widestr.c_str();

    DBOUT << "Using Onnxruntime C++ API" << endl;
    session = std::make_unique<Ort::Session>(env, model_path, session_options);

    //*************************************************************************
    // print model input layer (node names, types, shape etc.)
    Ort::AllocatorWithDefaultOptions allocator;

    // print number of model input nodes
    num_input_nodes = session->GetInputCount();
    input_node_names = vector<const char*>(num_input_nodes);
    input_node_dims = vector<vector<int64_t>>(num_input_nodes);


    DBOUT << "<------------------INPUTS-------------------->" << endl;
    DBOUT << "Number of inputs = " << num_input_nodes << endl;

    // iterate over all input nodes
    for (int i = 0; i < num_input_nodes; i++) {
        // print input node names
        char* input_name = session->GetInputName(i, allocator);
        DBOUT << "Input " << i << " : name=" << input_name << endl;
        input_node_names[i] = input_name;

        // print input node types
        Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        DBOUT << "Input " << i << " : type=" << type << endl;

        // print input shapes/dims
        input_node_dims[i] = tensor_info.GetShape();
        DBOUT << "Input " << i << " : num_dims=" << input_node_dims[i].size() << endl;
        for (int j = 0; j < input_node_dims[i].size(); j++)
            DBOUT << "Input " << i << " : dim " << j << "="<< input_node_dims[i][j] << endl;
    }

    //*************************************************************************
    // print model output layer (node names, types, shape etc.)
    DBOUT << "<------------------OUTPUTS-------------------->" << endl;
    size_t num_output_nodes = session->GetOutputCount();
    auto output_node_names = vector<const char*>(num_output_nodes);
    DBOUT << "Number of inputs = " << num_output_nodes << endl;

    // iterate over all outpus nodes
    for (int i = 0; i < num_output_nodes; i++) {
        // print input node names
        char* output_name = session->GetOutputName(i, allocator);
        DBOUT << "Output " << i << " : name=" << output_name << endl;
        output_node_names[i] = output_name;

        // print input node types
        Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        DBOUT << "Output " << i << " : type=" << type << endl;

        // print input shapes/dims
        auto output_node_dims = tensor_info.GetShape();
        DBOUT << "Output " << i << " : num_dims=" << output_node_dims.size() << endl;
        for (int j = 0; j < output_node_dims.size(); j++)
            DBOUT << "Output " << i << " : dim " << j << "=" << output_node_dims[j] << endl;
    }

    //*************************************************************************
    // print model initializer layer (node names, types, shape etc.)
    DBOUT << "<----------------INITIALIZER------------------>" << endl;
    size_t num_init_nodes = session->GetOverridableInitializerCount();
    auto init_node_names = vector<const char*>(num_init_nodes);
    DBOUT << "Number of OverridableInitializer = " << num_init_nodes << endl;

    // initialize the states
    initializeStates();
}

void GestureNetONNX::initializeStates()
{
    DBOUT << "Initializing LSTM States..." << endl;
    float tempZero[200] = { 0 };
    for (size_t i = 0; i < 5; ++i)
    {
        // first calculating the total length of the tensor
        auto& tempSize = input_node_dims[i];
        int totalSize = 1;
        for (size_t j = 0; j < tempSize.size(); ++j)
        {
            totalSize *= tempSize[j];
        }
        // creating the tensor
        
        packed_input.emplace_back(Ort::Value::CreateTensor<float>(memory_info,
                                    tempZero,
                                    totalSize,
                                    input_node_dims[i].data(),
                                    3));
    }
    DBOUT << "LSTM states initialized." << endl;
}

