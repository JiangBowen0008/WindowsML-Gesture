#ifndef UTILS_HPP
#define UTILS_HPP

#include <math.h>
#include <assert.h>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <chrono>
#include <deque>
#include <functional>
using namespace std::placeholders;

using std::cout;
using std::endl;
using std::ostream;
using std::string;
using std::map;
using std::vector;
using std::tuple;
using std::string;
using std::deque;

using std::unique_ptr;

ostream& operator<<(ostream& os, const vector<float>& v);

#endif