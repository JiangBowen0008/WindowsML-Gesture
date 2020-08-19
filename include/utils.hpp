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
#include <unordered_map>

#define FC 19e3
#define FS 48e3
#define NUM_GEST 4
#define FEAT_LEN 11
#define RAW_FEAT_LEN 136
#define WINDOW_SIZE 72

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
using std::get;
using std::string;
using std::deque;
using std::ofstream;
using std::unordered_map;

using std::unique_ptr;
using std::shared_ptr;

typedef vector<float> Floats;

ostream& operator<<(ostream& os, const vector<float>& v);

#endif