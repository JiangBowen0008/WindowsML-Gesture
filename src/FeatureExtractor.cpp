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
    logger.open("log.txt", ios_base::out);
    logger_fft.open("logfft.txt", ios_base::out);

    f0 = fs / (length);
    int center = ceil(fc / f0);
    
    int dn = floor( _dfc / f0);
    
    // set segmentation index & length
    AOIstart_ = center - dn;
    AOIend_ = center + dn - 1;
    AOISegLen_ = (dn * 2 + 1 - (2 * (floor( _dfc2 / f0) - 1) + 1))/2;

    // set window
    window = hammingWindow();
    window_s = 0.5 * window.cwiseAbs2().sum();
    window_m = 0.5 * window.sum();

    DBOUT << "----Configuration for sfft----" << endl;
    DBOUT << "center:\t" << center << endl;
    DBOUT << "dn: \t" << dn << endl;
    DBOUT << "dn2: \t" << AOISegLen_ << endl;
    DBOUT << "Window Type:\tHamming" << endl;
    DBOUT << "Window Energy:\t" << window_s << endl;
    DBOUT << "Window Magnitude:\t" << window_m << endl;
    DBOUT << "------------------------------" << endl;
}



/*----------------------------------------------------------------
                        Main Function
----------------------------------------------------------------*/

tuple<Floats, tuple<Floats, Floats>> FeatureExtractor::extractFeature(const MatrixXf& _signal)
{
    /******************************************************************
     * Main function of feature extractor
     * @param[in] _signal Signal as a std vector of float.
     * @return A tuple that contains:
     *          1.  A std vector that contains the extracted feature of
     *                  the signal.
     *          2.  A std vector that contains the raw fft (AOI)                    
     ******************************************************************/

    // preprocessing (obtaining AOI in Eigen vector form)
    VectorXf signal = _signal.row(0);
    VectorXf signal2 = _signal.row(1);
    //logger << signal << endl;

    //logger << signal << endl;
    auto [freq_, amp_] = FFT(signal, fs);
    auto [freq2_, amp2_] = FFT(signal2, fs);
    //logger_fft << amp_ << endl;
    
    VectorXf f = extractAOI(freq_);
    VectorXf f2 = extractAOI(freq2_);
    
    VectorXf s = extractAOI(amp_).cwiseAbs2() / window_s;
    VectorXf m = extractAOI(amp_).cwiseAbs() / window_m;
    
    VectorXf f_ = excludeAOI(f);
    VectorXf s_ = excludeAOI(s);
    VectorXf m_ = excludeAOI(m);

#ifdef _DEBUG_FEAT
    // logging to external txt
    logger_fft << m << endl;
    logger << f << endl;
#endif

    DBOUT << "Freq Mean: \t" << f.mean() << endl;

    int spectLen = f.size();
    DBOUT << "SpectLen\t" << spectLen << endl;
    DBOUT << "SpectLen2\t" << f_.size() << endl;
    // DBOUT << "frequency: " << f << endl;
    // DBOUT << "s: " << s << endl;

    //  calculating means and var with the middle spectrum excluded
    float energy = s_.sum();
    float avg = m_.mean();
    //float var = (m_.array() - avg).abs2().mean();

    // aux vars
    float freqBand = /*f[f.size() - 1] - f[0]*/f.size();
    float ssum = s.sum();
    float msum = m.sum();
    float eps = std::nextafter(0.0f, 1.0f);
    VectorXf prob = s / ssum;
    DBOUT << "freqBand\t" << freqBand << endl;

    // feature calculation using the full spectrum
    auto centroid = s.dot(f) / ssum;
    auto spread = sqrt ((f.array() - centroid).square().matrix().dot(s) / ssum);
    auto skewness = (f.array() - centroid).cube().matrix().dot(s) / (ssum * pow(spread, 3.0f));
    auto kurtosis = (f.array() - centroid).pow(4).matrix().dot(s) / (ssum * pow(spread, 4.0f));
    auto entropy = -prob.dot(log2(prob)) / (log2(freqBand));
    //auto flatness = pow(s.prod(), 1/freqBand) / (ssum / freqBand);
    auto flatness = exp(s.array().log().mean()) / s.mean();
    auto crest = s.maxCoeff() / (ssum / freqBand);
    auto diff = s.tail(spectLen - 1) - s.head(spectLen - 1);
    auto flux = pow(diff.cwiseAbs().sum(), 1);
    auto fdiff = (f.array() - f.mean()).matrix();
    auto mdiff = (m.array() - m.mean()).matrix();
    auto spectralSlope = fdiff.dot(mdiff) / fdiff.dot(fdiff);
    VectorXf inverse(spectLen - 1);
    inverse.setLinSpaced(1, spectLen);
    VectorXf tempm(m.tail(spectLen - 1));
    auto decrease = ((tempm.array() - m[0]) / inverse.array()).sum() / tempm.sum();

    // adding to the result
    VectorXf feat(11);
    feat << energy,
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

    // concatenating raw fft results from the two channels
    Floats tempF1 = cvt2Floats(f);
    Floats tempF2 = cvt2Floats(f2);
    assert(tempF1.size() == RAW_FEAT_LEN);
    assert(tempF2.size() == RAW_FEAT_LEN);

    return make_tuple(cvt2Floats(feat), make_tuple(tempF1, tempF2));
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

Floats FeatureExtractor::cvt2Floats(VectorXf& input)
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

    //signal = signal.array();
    
    signal = (signal.array() * window.array());
    //logger << signal << endl;
    //DBOUT << "Signal" << endl;
    //DBOUT << signal << endl;
    VectorXcf tmpOut(signal.size());
    fft.fwd(tmpOut, signal);
    VectorXf out = tmpOut.cwiseAbs().real();

    double T = 1 / fs;
    double f0 = 1 / (T * signal.size());
    VectorXf freq(signal.size());
    freq.setLinSpaced(0, (signal.size() - 1) * f0);
    
    return make_tuple(freq, out);
}

inline VectorXf log2(VectorXf& m)
{
    const float coe = std::log2(std::exp(1));
    return (m.array().log() * coe).matrix();
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

VectorXf FeatureExtractor::hammingWindow()
{
    ArrayXf n(length);
    n.setLinSpaced(1, length);
    ArrayXf window_ = 0.54 - 0.46 * (2 * M_PI * n / length).cos();
    return window_.matrix();
}