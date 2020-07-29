#ifndef GESTURE_NET_HPP
#define GESTURE_NET_HPP

#include "utils.hpp"
#include "Model.h"
#include "Tensor.h"

#include <iomanip>
#include <chrono>

#define _DEBUG_NET

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
    GestureNet(string modelName, string inputName, string outputName);
    ~GestureNet() = default;
    int getPred(vector<float>& _input);

   private:
    Model model_;
    Tensor input_, output_;
};

#endif