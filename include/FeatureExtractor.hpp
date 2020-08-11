#ifndef FEATURE_EXTRACTOR_HPP
#define FEATURE_EXTRACTOR_HPP

#include "utils.hpp"
#include "unsupported/Eigen/FFT"

#include <math.h>
#include <fstream>

//#define _DEBUG_FEAT

// debug streaming
#ifdef _DEBUG_FEAT
#undef DBOUT
#define DBOUT cout // or any other ostream
#else
#undef DBOUT
#define DBOUT 0 && cout
#endif

typedef vector<float> Floats;
typedef tuple<VectorXf, VectorXf> Fourier;

class FeatureExtractor
{
   public:
    FeatureExtractor(double fs = 48e3,
                    double fc = FC,
                    size_t = 8192,
                    double _dfc = 400,
                    double _dfc2 = 50);
    ~FeatureExtractor() = default;
    Floats extractFeature(const MatrixXf&);

   private:
    double fs, fc, f0;
    size_t length;
    VectorXf window;
    float window_m, window_s;
    int AOIstart_, AOIend_, AOISegLen_;
    std::ofstream logger, logger_fft;

    VectorXf cvt2Vect(Floats&);
    Floats cvt2Floats(VectorXf&);

    Fourier FFT(VectorXf signal, double fs);
    VectorXf hammingWindow();

    VectorXf extractAOI(VectorXf);
    VectorXf excludeAOI(VectorXf);
};

ostream& operator<<(ostream& os, const Floats&);
inline VectorXf log2(VectorXf& m);

#endif