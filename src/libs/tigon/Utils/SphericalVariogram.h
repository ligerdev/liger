#ifndef SPHERICALVARIOGRAM_H
#define SPHERICALVARIOGRAM_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/KrigingVariogram.h>
#include <eigen/Dense>
#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

namespace Tigon {

class LIGER_TIGON_EXPORT SphericalVariogram : public KrigingVariogram
{
public:
    SphericalVariogram(TVector<TVector<double> > xx, TVector<double> y, double nug = 0);
    ~SphericalVariogram();

    double value(const double h) const;
    void fitVariogram();

    double range() {return m_r;}

    void Refit();

    //LM Parameter Accessors
    void defineLamda0(double l) {m_lamda0 = l;}
    double getLamda0() {return m_lamda0;}
    void definelamdaUp(double l) {m_lamdaUp = l;}
    double getLamdaUp() {return m_lamdaUp;}
    void defineLamdaDown(double l) {m_lamdaDown = l;}
    double getLamdaDown() {return m_lamdaDown;}
    void defineMaxIterations(int i) {m_maxIter = i;}
    int getMaxIterations() {return m_maxIter;}
    void defineTolerance(double t) {m_tolerance = t;}
    double getTolerance() {return m_tolerance;}

    TVector<double> rHistory;
    TVector<double> cHistory;
    TVector<double> errHistory;
    int iter;

private:
    double m_r; //Range value calculated from a and b. If a or b is negative r is complex.

    int lsqLM(TCVectorReal& y_input, TCVectorReal& x_input, TCVectorReal& W, double &c, double &r);
    double variogramFunction(const double h, const double c, const double r) const;
    double dfunctiondc(const double h, const double r) const;
    double dfunctiondr(const double h, const double c, const double r) const;

    TVector<double> m_lags;
    TVector<double> m_ys;
    TCVectorReal m_weights;

    // Parameters for Levenburg-Marquardt fit
    double m_lamda0;
    double m_lamdaUp;
    double m_lamdaDown;
    int m_maxIter;
    double m_tolerance;
};

} // namespace Tigon

#endif // SPHERICALVARIOGRAM_H
