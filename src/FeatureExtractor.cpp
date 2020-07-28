#include "FeatureExtractor.hpp"

using namespace std;
using Eigen::seq;
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::MatrixXcf;
using Eigen::VectorXcf;

FeatureExtractor::FeatureExtractor( double _fs, 
                                    double _fc,
                                    size_t _length,
                                    double _dfc,
                                    double _dfc2):
                                fc(_fc), fs(_fs), length(_length)

{
    f0 = fs / (length);
    int center = ceil(fc / f0);
    int dn = floor( _dfc / f0);
    AOIstart_ = center - dn;
    AOIend_ = center + dn;
    AOISegLen_ = floor( _dfc2 / f0);
}

/*----------------------------------------------------------------
                        Main Function
----------------------------------------------------------------*/

Floats FeatureExtractor::extractFeature(Floats& _signal)
{
    /**
     * Main function of feature extractor
     * @param[in] _signal Signal as a std vector of float.
     * @return A std vector containing the extracted feature of
     *         the signal.
     */

    // preprocessing (obtaining AOI in Eigen vector form)
    VectorXf signal = cvt2Vect(_signal);
    signal = VectorXf::Random(_signal.size());
    auto [freq_, amp_] = FFT(signal, fs);
    
    VectorXf f = extractAOI(freq_);
    VectorXf s = extractAOI(amp_);
    VectorXf f_ = excludeAOI(f);
    VectorXf s_ = excludeAOI(s);

    int spectLen = f.size();
    // DBOUT << "frequency: " << f << endl;
    // DBOUT << "s: " << s << endl;

    //  calculating means and var with the middle spectrum excluded
    float rms = s_.cwiseAbs2().mean();
    float avg = s_.cwiseAbs().mean();
    float var = (s_.array() - avg).abs2().mean();

    // feature calculation using the full spectrum
    float centroid = s.dot(f) / s.sum();
    float spread = sqrt ((f.array() - centroid).square().matrix().dot(s) / s.sum());
    float skewness = (f.array() - centroid).cube().matrix().dot(s) / (s.sum() * pow(spread, 3));
    float kurtosis = (f.array() - centroid).pow(4).matrix().dot(s) / (s.sum() * pow(spread, 4));
    float entropy = -s.dot(s.array().log().matrix()) / (log(spectLen));
    float flatness = pow(s.prod(), 1/spectLen) / (s.sum() / spectLen);
    float crest = s.maxCoeff() / (s.sum() / spectLen);
    auto diff = s.tail(spectLen - 1) - s.head(spectLen - 1);
    float flux = pow(diff.cwiseAbs().sum(), 1);
    auto fdiff = (f.array() - f.mean()).matrix();
    auto sdiff = (s.array() - s.mean()).matrix();
    float spectralSlope = fdiff.dot(sdiff) / fdiff.dot(fdiff);
    VectorXf inverse(spectLen);
    inverse.setLinSpaced(1, spectLen).cwiseInverse();
    float decrease = (s.array() - s[0]).matrix().dot(inverse) / s.sum();

    // adding to the result
    VectorXf feat(12);
    feat << rms,
            var,
            centroid,
            spread,
            skewness,
            kurtosis,
            entropy,
            flatness,
            crest,
            flux,
            spectralSlope,
            decrease;

    DBOUT << "Extracted: " << endl << feat << endl;
    return cvt2Floats(feat);
}

/*----------------------------------------------------------------
                            Conversions
----------------------------------------------------------------*/

VectorXf FeatureExtractor::cvt2Vect(Floats& input)
{
    /**
     * Convert an Eigen vector into a std vector.
     */

    VectorXf output = Eigen::Map<VectorXf>(input.data(), input.size());
    return output;
}

Floats FeatureExtractor::cvt2Floats(VectorXf input)
{
    /**
     * Convert a std vector into an Eigen vector.
     */

    Floats output(input.size());
    output.assign(input.data(), input.data() + input.size());
    return output;
}

/*----------------------------------------------------------------
                    Calculation Helper Functions
----------------------------------------------------------------*/

Fourier FeatureExtractor::FFT(VectorXf signal, double fs)
{
    Eigen::FFT<float> fft;

    VectorXcf tmpOut(signal.size());
    fft.fwd(tmpOut, signal);
    VectorXf out = tmpOut.cwiseAbs().real();

    double T = 1 / fs;
    double f0 = 1 / (T * signal.size());
    VectorXf freq(signal.size());
    freq.setLinSpaced(1 * f0, signal.size() * f0);
    
    return make_tuple(freq, out);
}

VectorXf FeatureExtractor::extractAOI(VectorXf input)
{
    auto idx = seq(AOIstart_, AOIend_);
    return input(idx);
}

VectorXf FeatureExtractor::excludeAOI(VectorXf input)
{
    VectorXf joined(2 * AOISegLen_);
    joined << input.head(AOISegLen_), input.tail(AOISegLen_);
    return joined;
}


/*----------------------------------------------------------------
                        Utility Functions
----------------------------------------------------------------*/

ostream& operator<<(ostream& os, const Floats& floats)
{
    for (auto& f : floats)
    {
        os << f << "\t";
    }
    os << endl;
    return os;
}