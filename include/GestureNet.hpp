#ifndef GESTURE_NET_HPP
#define GESTURE_NET_HPP

#include "Model.h"
#include "Tensor.h"

#include <numeric>
#include <iomanip>
#include <string>
#include <chrono>
#include <math.h>
#include <algorithm>

using namespace std;

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