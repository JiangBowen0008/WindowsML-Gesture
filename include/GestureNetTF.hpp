#ifndef GESTURE_NET_TF_HPP
#define GESTURE_NET_TF_HPP

#include "utils.hpp"
#include "GestureNet.hpp"
#include "Model.h"
#include "Tensor.h"

#include <iomanip>

class GestureNetTF : public GestureNet
{
public:
    GestureNetTF() = delete;
    GestureNetTF(string modelName, string inputName, string outputName);
    virtual ~GestureNetTF() = default;
    virtual int getPred(vector<float>& _input);

private:
    Model model_;
    Tensor input_, output_;
    Tensor lstm1_, lstm2_;
    // Tensor LSTM1_buffer_, LSTM2_buffer_;
};

#endif
