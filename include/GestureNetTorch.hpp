#ifndef GESTURE_NET_TORCH_HPP
#define GESTURE_NET_TORCH_HPP

#include "utils.hpp"
#include "GestureNet.hpp"
#include <torch/torch.h>
#include <torch/script.h>

//#define _DEBUG_TORCH

// debug streaming
#ifdef _DEBUG_TORCH
#undef DBOUT
#define DBOUT cout // or any other ostream
#else
#undef DBOUT
#define DBOUT 0 && cout
#endif

namespace F = torch::nn::functional;
namespace jit = torch::jit;

class GestureNetTorch : public GestureNet
{
public:
    GestureNetTorch() = delete;
    GestureNetTorch(string modelName);
    virtual ~GestureNetTorch() = default;
    virtual int getPred(vector<float>& _input);

private:
    struct Net : torch::nn::Module
    {
        Net();
        ~Net() = default;
        torch::Tensor forward(torch::Tensor);
        torch::nn::Linear fc1{nullptr}, fc2{nullptr};
        torch::nn::LSTM lstm1{nullptr}, lstm2{nullptr};
        tuple<torch::Tensor, torch::Tensor> hidden1_, hidden2_;
    };
    shared_ptr<Net> net;

    // solution 2
    jit::script::Module module;
    vector<jit::IValue> stackedInput;

};

#endif
