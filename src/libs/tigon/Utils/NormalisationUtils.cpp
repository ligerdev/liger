/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
double magnitudeAndDirectionP(TVector<double>& vec, double p)
{
    double magnitude = 0.0;
    for(auto v : vec) {
        magnitude += std::pow(v, p);
    }

    magnitude = std::pow(magnitude, 1.0 / p);

    if(magnitude == 0.0) {
        return magnitude;
    }

    for(size_t i=0; i < vec.size(); i++) {
        vec[i] /= magnitude;
    }
    return magnitude;
}

double magnitudeAndDirectionP(TVector<IElementSPtr>& vec, double p)
{
    double magnitude = 0.0;
    for(auto v : vec) {
        magnitude += std::pow(v->value<double>(), p);
    }

    magnitude = std::pow(magnitude, 1.0 / p);

    for(size_t i=0; i < vec.size(); i++) {
        *vec[i] = *vec[i] / magnitude;
    }
    return magnitude;
}

void toUnitVec(TVector<double>& vec, double norm)
{
    magnitudeAndDirectionP(vec, norm);
}

void toUnitVec(TVector<IElementSPtr>& vec, double norm)
{
    magnitudeAndDirectionP(vec, norm);
}


void scale(TVector<double>& vec, double factor)
{
    for(size_t i=0; i < vec.size(); i++) {
        vec[i] *= factor;
    }
}

void scale(TVector<IElementSPtr> vec, double factor)
{
    for(size_t i=0; i < vec.size(); i++) {
        *vec[i] = *vec[i] * factor;
    }
}

void normaliseToUnitInterval(double& val,
                             double  lBound,
                             double  uBound)
{
    if(val <= lBound) {
        val = 0.0;
    } else if(val >= uBound) {
        val = 1.0;
    } else {
        val = (val - lBound) / (uBound - lBound);
    }
}

void normaliseToUnitInterval(IElement& val,
                             const IElement& lBound,
                             const IElement& uBound)
{
    val.defineType(RealType);
    if(val <= lBound) {
        val.defineValue(0.0);
    } else if(val >= uBound) {
        val.defineValue(1.0);
    } else {
        val = (val - lBound) / (uBound - lBound);
    }
}

void normaliseToUnitBoxInMemory(TVector<double>& vec, const TVector<double>& lBounds,
                                             const TVector<double>& uBounds)
{
    for(size_t i=0; i < vec.size(); i++) {
        if(vec[i] <= lBounds[i]) {
            vec[i] = 0.0;
        } else if(vec[i] >= uBounds[i]) {
            vec[i] = 1.0;
        } else {
            vec[i] = (vec[i] - lBounds[i]) / (uBounds[i] - lBounds[i]);
        }
    }
}

void normaliseToUnitBoxInMemory(TVector<IElementSPtr> vec,
                                BoxConstraintsDataSPtr box)
{
    TVector<double> lBounds = IElementVecToRealVec(box->lowerBounds());
    TVector<double> uBounds = IElementVecToRealVec(box->upperBounds());

    for(size_t i=0; i < vec.size(); i++) {
        vec[i]->defineType(RealType);
        if(*vec[i] <= lBounds[i]) {
            vec[i]->defineValue(0.0);
        } else if(*vec[i] >= uBounds[i]) {
            vec[i]->defineValue(1.0);
        } else {
            vec[i]->defineValue((*vec[i] - lBounds[i]) / (uBounds[i] - lBounds[i]));
        }
    }
}

TVector<double> normaliseToUnitBox(const TVector<double>& vec,
                                   const TVector<double>& lBounds,
                                   const TVector<double>& uBounds)
{
    TVector<double> ret(vec.size());
    for(size_t i=0; i < vec.size(); i++) {
        if(vec[i] <= lBounds[i]) {
            ret[i] = 0.0;
        } else if(vec[i] >= uBounds[i]) {
            ret[i] = 1.0;
        } else {
            ret[i] = (vec[i] - lBounds[i]) / (uBounds[i] - lBounds[i]);
        }
    }
    return ret;
}

void scaleBackFromUnitInterval(double& normVal,
                               double  lBound,
                               double  uBound)
{
    if(normVal >= 1) {
        normVal = uBound;
    } else if(normVal <= 0.0) {
        normVal = lBound;
    } else {
        normVal = lBound + normVal * (uBound - lBound);
    }
}

void scaleBackFromUnitInterval(IElement& normVal,
                               const IElement& lBound,
                               const IElement& uBound)
{
    if(normVal >= 1.0) {
        normVal.defineValue(uBound);
    } else if(normVal <= 0.0) {
        normVal.defineValue(lBound);
    } else {
        normVal.defineValue(lBound + normVal * (uBound - lBound));
    }
}

void scaleBackFromUnitBox(TVector<double>& vec, const TVector<double>& lBounds,
                                           const TVector<double>& uBounds)
{
    /// \attention The normalisation of elements out-of-bounds is inconsistent.
    /// The CODEM problems seem to lead this situation.
    /// Further checks are needed here.
    for(size_t i=0; i < vec.size(); i++) {
        if(vec[i] > 1.0) {
            //vec[i] = uBounds[i];
            vec[i] = uBounds[i] - (1.0 - vec[i]) * (uBounds[i] - lBounds[i]);
        } else if(vec[i] < 0.0) {
            //vec[i] = lBounds[i];
            vec[i] = lBounds[i] - vec[i] * (uBounds[i] - lBounds[i]);
        } else {
            vec[i] = lBounds[i] + vec[i] * (uBounds[i] - lBounds[i]);
        }
    }
}

void scaleBackFromUnitBox(TVector<IElementSPtr> vec, BoxConstraintsDataSPtr box)
{
    TVector<double> lBounds = IElementVecToRealVec(box->lowerBounds());
    TVector<double> uBounds = IElementVecToRealVec(box->upperBounds());

    for(size_t i=0; i < vec.size(); i++) {
        if(*vec[i] >= 1.0) {
            vec[i]->defineValue(uBounds[i]);
        } else if(*vec[i] <= 0.0) {
            vec[i]->defineValue(lBounds[i]);
        } else {
            vec[i]->defineValue(lBounds[i] + *vec[i] * (uBounds[i] - lBounds[i]));
        }
    }
}

TVector<double> normaliseForSimplexLattice(const TVector<double> &vec,
                                           const TVector<double> &lBounds,
                                           const TVector<double> &uBounds,
                                           double valueNotSet)
{
    TVector<double> normVec = normaliseToUnitBox(vec, lBounds, uBounds);

    // calculate the vector sum
    double vecSum = std::accumulate(normVec.cbegin(), normVec.cend(), 0.0);

    // components not set in the original vector are kept temporarily
    // in the normalised vector
    for(size_t i=0; i<vec.size(); i++) {
        if(areDoublesEqual(vec[i], valueNotSet)) {
            normVec[i] = valueNotSet;
        }
    }

    // format the vector for simplex
    for(auto&& v : normVec) {

        // change the components that have not been set, accordingly to the
        // position of the vector with respect to the simplex
        if(areDoublesEqual(v,valueNotSet)) {
            if(vecSum>1.0) {
                v = 1.0; // vector in the top of the simplex
            }
            else {
                v = 0.0; // vector below the simplex
            }
        }

        // vector has a zero component but lies on the top of the simplex
        if(vecSum>1.0) {
            if(v<Tigon::Epsilon) {
                v = 1.0;
            }
        }
    }

    return normVec;
}

double normalisedDistanceP(TVector<IElementSPtr> m, TVector<IElementSPtr> c,
                          BoxConstraintsDataSPtr box, double p)
{
    double ret = -1.0;
    int nElems = m.size();
    if( (c.size() == m.size()) && (box->size() == nElems) ) {
        TVector<double> diff(nElems, 0.0);
        for(int i = 0; i < nElems; i++) {
            double interval = box->upperBound(i).value() - box->lowerBound(i).value();
            if(m[i]->type() == NominalType) {
                if(*m[i] != *c[i]) {
                    diff[i] = 1.0 / interval;
                }
            } else {
                diff[i] = std::fabs(m[i]->value() - c[i]->value()) / interval;
            }
        }
        ret = magnitudeAndDirectionP(diff, p);
    }
    return ret;
}

double normalisedDistanceVecP(const TVector<double> &m, const TVector<double> &c,
                           BoxConstraintsDataSPtr box, double p)
{
    double ret = -1.0;
    int nElems = m.size();
    if( (c.size() == m.size()) && (box->size() == nElems) ) {
        TVector<double> diff(nElems, 0.0);
        for(int i = 0; i < nElems; i++) {
            double interval = box->upperBound(i).value() - box->lowerBound(i).value();
            diff[i] = std::fabs(m[i] - c[i]) / interval;
        }
        ret = magnitudeAndDirectionP(diff, p);
    }
    return ret;
}


double directedBoxedIntervalLength(const TVector<double> dir, double p)
{
    double maxComponent = 0.0;
    for(auto d : dir) {
        if(d > maxComponent) {
            maxComponent = d;
        }
    }

    double len = 0.0;
    for(auto d : dir) {
        len += std::pow((d / maxComponent), p);
    }

    len = std::pow(len, 1.0 / p);
    return len;
}

} // namespace Tigon
