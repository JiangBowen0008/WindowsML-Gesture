#ifndef GESTURE_NET_HPP
#define GESTURE_NET_HPP

#include "utils.hpp"

#define GESTURE_NET_TYPE 0
/* Gesture Net Types:
    0:  PyTorch - GestureNetTorch
    1:  Tensorflow - GestureNetTF
    2:  ONNX Runtime - GestureNetONNX */

//#define _DEBUG_NET

// debug streaming
#ifdef _DEBUG_NET
#undef DBOUT
#define DBOUT cout // or any other ostream
#else
#undef DBOUT
#define DBOUT 0 && cout
#endif

class GestureNet
{
public:
    GestureNet() = default;
    virtual ~GestureNet() = default;

    [[deprecated("Single input as extracted feature is deprecated. Input raw fft results as well.")]]
    virtual int getPred(Floats& _input) = 0;

    virtual int getPred(Floats& _input, tuple<Floats, Floats>& _rawInput) = 0;
protected:
    virtual int rankPred(vector<float>& predictions);
};

#endif