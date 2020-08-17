#ifndef GESTURE_NET_HPP
#define GESTURE_NET_HPP

#include "utils.hpp"

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
    GestureNet() = default;
    virtual ~GestureNet() = default;
    virtual int getPred(vector<float>& _input) = 0;
protected:
    virtual int rankPred(vector<float>& predictions);
};

#endif