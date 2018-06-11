/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef NONDOMINANCERANKINGPARAMETRIC_H
#define NONDOMINANCERANKINGPARAMETRIC_H

#include <tigon/Operators/Fitness/NonDominanceRanking.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NonDominanceRankingParametric :
        public NonDominanceRanking
{
    DECLARE_CLASS(Tigon::Operators::NonDominanceRankingParametric)

public:
    NonDominanceRankingParametric();
    NonDominanceRankingParametric(Tigon::Representation::IPSet* ipset);
    ~NonDominanceRankingParametric();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    void TP_defineNumberOfBins(int b);
    int TP_numberOfBins() const;

protected:
    void initialise();

private:
    int m_numberOfBins;
};

} // namespace Operators
} // namespace Tigon

#endif // NONDOMINANCERANKINGPARAMETRIC_H
