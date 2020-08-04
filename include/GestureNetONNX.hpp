#ifndef GESTURE_NET_ONNX_HPP
#define GESTURE_NET_ONNX_HPP

#include "utils.hpp"
#include "GestureNet.hpp"
#include <onnxruntime_cxx_api.h>
#include <stdlib.h>
#include <stdio.h>

class GestureNetONNX : public GestureNet
{
public:
    GestureNetONNX() = delete;
    GestureNetONNX(string modelName);
    ~GestureNetONNX() = default;
    int getPred(vector<float>& _input);
    
private:
    Ort::Env env;
    Ort::SessionOptions session_options;
    unique_ptr<Ort::Session> session;

    // inputs
    size_t num_input_nodes;
    vector<const char*> input_node_names;
    vector<int64_t> input_node_dims;  // simplify... this model has only 1 input node {1, 3, 224, 224}.
                                           // Otherwise need vector<vector<>>

    // outputs
    vector<const char*> output_node_names;

    // buffer
    vector<float> lstm1_buffer, lstm2_buffer;

protected:
};

#endif
