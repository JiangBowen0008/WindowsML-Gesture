#ifndef FEATURE_EXTRACTOR_HPP
#define FEATURE_EXTRACTOR_HPP

#include "utils.hpp"
#include "Eigen/Core"
#include "Eigen/Dense"
#include "unsupported/Eigen/FFT"

#include <math.h>

// #define _DEBUG_FEAT

// debug streaming
#ifdef _DEBUG_FEAT
#undef DBOUT
#define DBOUT cout // or any other ostream
#else
#undef DBOUT
#define DBOUT 0 && cout
#endif

using Eigen::Matrix;
using Eigen::ArrayXf;
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::MatrixXcf;
using Eigen::VectorXcf;

typedef vector<float> Floats;
typedef tuple<VectorXf, VectorXf> Fourier;

class FeatureExtractor
{
   public:
    FeatureExtractor(double fs = 48e3,
                    double fc = 19e3,
                    size_t = 8192,
                    double _dfc = 450,
                    double _dfc2 = 50);
    ~FeatureExtractor() = default;
    Floats extractFeature(Floats&);

   private:
    double fs, fc, f0;
    size_t length;
    int AOIstart_, AOIend_, AOISegLen_;

    VectorXf cvt2Vect(Floats&);
    Floats cvt2Floats(VectorXf);
    Fourier FFT(VectorXf signal, double fs);

    VectorXf extractAOI(VectorXf);
    VectorXf excludeAOI(VectorXf);
};

ostream& operator<<(ostream& os, const Floats&);

#endif