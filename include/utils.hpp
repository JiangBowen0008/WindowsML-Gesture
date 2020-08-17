#ifndef UTILS_HPP
#define UTILS_HPP

#define _USE_MATH_DEFINES
//#define EIGEN_MALLOC_ALREADY_ALIGNED = 0
#include "Eigen/Core"
#include "Eigen/Dense"
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
#include <fstream>

#define FC 19e3
#define FS 48e3

using Eigen::Matrix;
using Eigen::ArrayXf;
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::MatrixXcf;
using Eigen::VectorXcf;

using std::cout;
using std::endl;
using std::ostream;
using std::string;
using std::map;
using std::vector;
using std::tuple;
using std::string;
using std::deque;
using std::ofstream;

using std::unique_ptr;
using std::shared_ptr;

ostream& operator<<(ostream& os, const vector<float>& v);

#endif