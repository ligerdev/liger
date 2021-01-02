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
#ifndef UNCERTAINTYMAPPING_H
#define UNCERTAINTYMAPPING_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT UncertaintyMapping
{
public:
    UncertaintyMapping();
    UncertaintyMapping(const UncertaintyMapping& umap);
    ~UncertaintyMapping();

    UncertaintyMapping* clone() const;

    // Uncertainty Mapping definition
    void defineDistributionType(Tigon::DistributionType t);
    void defineConstTerms(TVector<double>        constants);
    void defineLinearTerms(TVector<double>             lin);
    Tigon::DistributionType distributionType()       const;
    TVector<double>          constTerms()             const;
    TVector<double>          linearTerms()            const;
    int                     nParams()                const;

    // IElement evaluation
    void evaluateUncertainty(IElementSPtr elem) const;


private:
    TVector<double> calculateParameters(IElementSPtr elem) const;

    Tigon::DistributionType m_distType;
    int                     m_nParams;
    TVector<double>          m_constTerm;
    TVector<double>          m_linTerm;
//  TVector<double>          m_quadTerm;
};

} // namespace Representation
} // namespace Tigon

#endif // UNCERTAINTYMAPPING_H
