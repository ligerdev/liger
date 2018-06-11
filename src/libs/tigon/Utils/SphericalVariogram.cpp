#include <tigon/Utils/SphericalVariogram.h>

namespace Tigon {


SphericalVariogram::SphericalVariogram(TVector<TVector<double> > xx, TVector<double> y, double nug)
    : KrigingVariogram(nug, 0, 1),
      m_lamda0(0.01),
      m_lamdaUp(11),
      m_lamdaDown(0.1111),
      m_maxIter(10000),
      m_tolerance(0.01)
{
    assignNugget(nug);
    assignVariogramData(xx, y);
    calculateEmpiricalVariogram(xx, y);
    fitVariogram();
}

void SphericalVariogram::fitVariogram()
{
    calculateEmpiricalVariogram(inputData(), outputData());

    TCVectorReal eV = empiricalVariograms();
    TCVectorReal eL = empiricalLags();

    int numBins = eL.rows();

    double minLag = eL.minCoeff();
    double maxLag = eL.maxCoeff();
    double rangeLag = maxLag - minLag;

    //Empirical Variogram to vector
    TCVectorReal b(numBins);
    m_weights = TCVectorReal(numBins);
    for (int i=0; i<numBins; i++) {
        b(i) = (double) eV(i) - nugget();
        if(eL(i) == minLag) {
            m_weights(i) = 1;
        } else {
            m_weights(i) = rangeLag / (eL(i) - minLag); // Inverse weighting
        }
    }

    double c;
    lsqLM(b, eL, m_weights, c, m_r);

    assignC(c);
}

void SphericalVariogram::Refit()
{
    fitVariogram();
}

//Spherical variogram function
double SphericalVariogram::variogramFunction(const double h, const double c, const double r) const
{
    if(h <= r) {
        return c*((3*h)/(2*r) - (0.5*pow(h/r, 3)));
    } else {
        return c;
    }
}

// partial derivative of variogram funciton wrt c (dc)
double SphericalVariogram::dfunctiondc(const double h, const double r) const
{
    if(h <= r) {
        return ((3*h)/(2*r)) - (pow(h,3) / (2*pow(r,3)));
    } else {
        return 1;
    }
}

//partial derivative variogram funciton wrt r (dr)
double SphericalVariogram::dfunctiondr(const double h, const double c, const double r) const
{
    if(h <= r) {
    return (3*c*std::pow(h,3))/(2*std::pow(r,4)) - (3*c*h)/(2*std::pow(r,2));
    } else {
        return c;
    }
}

SphericalVariogram::~SphericalVariogram()
{

}

int SphericalVariogram::lsqLM(TCVectorReal& y_input, TCVectorReal& x_input, TCVectorReal& W, double& c, double& r)
{
    rHistory.clear();
    cHistory.clear();
    errHistory.clear();

    double lamda = m_lamda0;
    bool updateJ = true;
    int dataSize = x_input.rows();

    //Parameters
    double cEst=0.0;
    double rEst=0.0;
    double cNew;
    double rNew;

    //Initial estimate of paramters
    // cEst = max(y_input);
    // rEst = 2/3*max(x_input);
    for(int i=0; i<dataSize; ++i) {
        if(cEst < y_input[i]) {
            cEst = y_input[i];
        }
        if(rEst < x_input[i]) {
            rEst = x_input[i];
        }
    }
    rEst *= 0.666;

    rHistory.push_back(rEst);
    cHistory.push_back(cEst);

    TMatrixReal J(dataSize, 2);
    TMatrixReal H(2,2);
    TMatrixReal HNew(2,2);
    TMatrixReal HDiag(2,2);
    TCVectorReal yEst(dataSize);
    TCVectorReal dist(dataSize);
    TCVectorReal wDist(dataSize);
    TCVectorReal pDelta;
    double error;
    double newError;

    iter=0;
    int done=0;
    int attemptsBeforeExit = 3;

    while((iter<m_maxIter) & (done<attemptsBeforeExit)) {
        ++iter;
        if(updateJ) {
            //Jacobian
            for(int i=0; i < dataSize; ++i) {
                J(i,0) = dfunctiondc(x_input[i], rEst);
                J(i,1) = dfunctiondr(x_input[i], cEst, rEst);
                yEst(i) = variogramFunction(x_input[i], cEst, rEst);
            }
            H = J.adjoint()*W.asDiagonal()*J;

            dist = y_input - yEst;
            if(iter == 1) {
                wDist = dist.array() * W.array();
                error = dist.dot(wDist);
            }
        }

        HDiag = H.diagonal().asDiagonal();
        HNew = H + (lamda*HDiag);

        pDelta = HNew.inverse() * (J.adjoint() * W.asDiagonal() * dist); //Parameter change

        cNew = cEst + pDelta(0);
        rNew = rEst + pDelta(1);

        for(int i=0; i < dataSize; ++i) {
            yEst(i) = variogramFunction(x_input[i], cNew, rNew);
        }
        dist = y_input - yEst;
        wDist = dist.array() * W.array();
        newError = dist.dot(wDist);

        if(std::abs(error - newError) < m_tolerance) {
            done++;
        } else { done = 0; }

        //errHistory.push_back(newError);

        if(newError < error) {
            lamda = lamda * m_lamdaDown;
            rEst = rNew;
            cEst = cNew;
            error = newError;
            updateJ = true;

            rHistory.push_back(rEst);
            cHistory.push_back(cEst);
            errHistory.push_back(error);

        } else {
            updateJ = false;
            lamda = lamda * m_lamdaUp;
        }
    }
    c = cEst;
    r = rEst;
    return iter;
}

double SphericalVariogram::value(const double h) const
{
    return nugget() + variogramFunction(h, paramC(), m_r);
}

} //namespace Tigon
