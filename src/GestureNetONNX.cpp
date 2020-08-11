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
<<<<<<< HEAD
                                                                3);
    assert(input_tensor.IsTensor());
=======
                                                                5);
    assert(input_tensor.IsTensor());
    DBOUT << "working so far" << endl;
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
    // score model & input tensor, get back output tensor
    auto output_tensors = session->Run( Ort::RunOptions{ nullptr },
                                        input_node_names.data(),
                                        &input_tensor,
                                        1,
                                        output_node_names.data(),
                                        1/*output_node_names.size()*/);
    //assert(output_tensors.size() == 1 && output_tensors.front().IsTensor());
<<<<<<< HEAD
=======
    DBOUT << "working so far" << endl;
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
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
<<<<<<< HEAD
    //session_options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
=======
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd

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

<<<<<<< HEAD
    DBOUT << "Using Onnxruntime C++ API" << endl;
=======
    cout << "Using Onnxruntime C++ API" << endl;
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
    session = std::make_unique<Ort::Session>(env, model_path, session_options);

    //*************************************************************************
    // print model input layer (node names, types, shape etc.)
    Ort::AllocatorWithDefaultOptions allocator;

    // print number of model input nodes
    num_input_nodes = session->GetInputCount();
    input_node_names = vector<const char*>(num_input_nodes);

<<<<<<< HEAD

    DBOUT << "<------------------INPUTS-------------------->" << endl;
    DBOUT << "Number of inputs = " << num_input_nodes << endl;
=======
    printf("Number of inputs = %zu\n", num_input_nodes);
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd

    // iterate over all input nodes
    for (int i = 0; i < num_input_nodes; i++) {
        // print input node names
        char* input_name = session->GetInputName(i, allocator);
<<<<<<< HEAD
        DBOUT << "Input " << i << " : name=" << input_name << endl;
=======
        printf("Input %d : name=%s\n", i, input_name);
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        input_node_names[i] = input_name;

        // print input node types
        Ort::TypeInfo type_info = session->GetInputTypeInfo(i);
        auto tensor_info = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensor_info.GetElementType();
<<<<<<< HEAD
        DBOUT << "Input " << i << " : type=" << type << endl;

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        DBOUT << "Input " << i << " : num_dims=" << input_node_dims.size() << endl;
        for (int j = 0; j < input_node_dims.size(); j++)
            DBOUT << "Input " << i << " : dim " << j << "="<< input_node_dims[j] << endl;
    }

    //*************************************************************************
    // print model output layer (node names, types, shape etc.)
    DBOUT << "<------------------OUTPUTS-------------------->" << endl;
    size_t num_output_nodes = session->GetOutputCount();
    auto output_node_names = vector<const char*>(num_input_nodes);
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
    auto init_node_names = vector<const char*>(num_input_nodes);
    DBOUT << "Number of OverridableInitializer = " << num_init_nodes << endl;
=======
        printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        input_node_dims = tensor_info.GetShape();
        printf("Input %d : num_dims=%zu\n", i, input_node_dims.size());
        for (int j = 0; j < input_node_dims.size(); j++)
            printf("Input %d : dim %d=%jd\n", i, j, input_node_dims[j]);
    }
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
}

