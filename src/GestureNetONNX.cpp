#include "GestureNetONNX.hpp"

/*----------------------------------------------------------------
                         Main Functions
----------------------------------------------------------------*/

int GestureNetONNX::getPred(vector<float>& _input)
{
    size_t input_tensor_size = _input.size();
    vector<float>& input_tensor_values = _input;

    // create input tensor object from data values
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(  memory_info,
                                                                input_tensor_values.data(),
                                                                input_tensor_size,
                                                                input_node_dims.data(),
                                                                5);
    assert(input_tensor.IsTensor());
    DBOUT << "working so far" << endl;
    // score model & input tensor, get back output tensor
    auto output_tensors = session->Run( Ort::RunOptions{ nullptr },
                                        input_node_names.data(),
                                        &input_tensor,
                                        1,
                                        output_node_names.data(),
                                        1/*output_node_names.size()*/);
    //assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
    DBOUT << "working so far" << endl;
    // assign lstm buffers
    float* floatarr;
    //float* floatarr = output_tensors[1].GetTensorMutableData<float>();
    //lstm1_buffer.assign(floatarr, floatarr + 1);
    //DBOUT << floatarr << endl;

    // Get pointer to output tensor float values
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
    lstm2_buffer(128)
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

    cout << "Using Onnxruntime C++ API" << endl;
    session = std::make_unique<Ort::Session>(env, model_path, session_options);

    //*************************************************************************
    // print model input layer (node names, types, shape etc.)
    Ort::AllocatorWithDefaultOptions allocator;

    // print number of model input nodes
    num_input_nodes = session->GetInputCount();
    input_node_names = vector<const char*>(num_input_nodes);

    printf("Number of inputs = %zu\n", num_input_nodes);

    // iterate over all input nodes
    for (int i = 0; i < num_input_nodes; i++) {
        // print input node names
        char* input_name = session->GetInputName(i, allocator);
        printf("Input %d : name=%s\n", i, input_name);
        input_node_names[i] = input_name;

        // print input node types
        Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
        printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        printf("Input %d : num_dims=%zu\n", i, input_node_dims.size());
        for (int j = 0; j < input_node_dims.size(); j++)
            printf("Input %d : dim %d=%jd\n", i, j, input_node_dims[j]);
    }
}

