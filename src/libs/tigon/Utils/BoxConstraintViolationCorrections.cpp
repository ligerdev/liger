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
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Random/RandomGenerator.h>

using namespace Tigon::Representation;
namespace Tigon {

void reflectIntoFeasibleDomain(IMappingSPtr imap, BoxConstraintsDataSPtr box)
{
    TVector<IElement> ubs = box->upperBounds();
    TVector<IElement> lbs = box->lowerBounds();

    for(size_t i=0; i<ubs.size(); i++) {
        double lb = lbs.at(i).value<double>();
        double ub = ubs.at(i).value<double>();

        IElementSPtr var = imap->decisionVar(i);
		double val = var->value<double>();
        if(var->type() == NominalType) {
            if((val < lb) || (val > ub)) {
                imap->defineDecisionVar(i, TRAND.randInt(ub - lb + 1, lb));
            }
        } else if(val < lb) {
            int m = static_cast<int>((lb - val) / (ub-lb));
            double e = val + m * (ub-lb);
            if(m % 2 == 0) {
                e = 2*lb - e;
            } else {
                e = e + (ub - lb);
            }
            imap->defineDecisionVar(i, e);
        } else if(val > ub) {
            int m = (int)(var->value<double>() - ub) / (ub-lb);
            double e = val - m * (ub-lb);
            if(m % 2 == 0) {
                e = 2*ub - e;
            } else {
                e = e - (ub - lb);
            }
            imap->defineDecisionVar(i, e);
        }
    }
}

void truncateIntoFeasibleDomain(IMappingSPtr imap, BoxConstraintsDataSPtr box)
{
    TVector<double> ub = IElementVecToRealVec(box->upperBounds());
    TVector<double> lb = IElementVecToRealVec(box->lowerBounds());

    for(size_t i=0; i<ub.size(); i++) {
        IElementSPtr var = imap->decisionVar(i);
        if(var->type() == NominalType) {
            if((var->value() < lb[i]) || (var->value() > ub[i])) {
                imap->defineDecisionVar(i, TRAND.randInt(ub[i] - lb[i] + 1, lb[i]));
            }
        } else if(*(var) < lb[i]) {
            imap->defineDecisionVar(i, lb[i]);
        } else if(*(var) > ub[i]) {
            imap->defineDecisionVar(i, ub[i]);
        }
    }
}

void mapIntoToroidalDomain(IMappingSPtr imap, BoxConstraintsDataSPtr box)
{
    TVector<double> ub = IElementVecToRealVec(box->upperBounds());
    TVector<double> lb = IElementVecToRealVec(box->lowerBounds());

    for(int i=0; i<ub.size(); i++) {
        IElementSPtr var = imap->decisionVar(i);
        int m;
        if(var->type() == NominalType) {
            if((var->value() < lb[i]) || (var->value() > ub[i])) {
                imap->defineDecisionVar(i, TRAND.randInt(ub[i] - lb[i] + 1, lb[i]));
            }
        } else if(*(var) < lb[i]) {
            m = (int)(lb[i] - var->value<double>()) / (ub[i]-lb[i]);
            IElement e = *(var) + (m+1) * (ub[i]-lb[i]);
            imap->defineDecisionVar(i, e);
        } else if(*(var) > ub[i]) {
            m = (int)(var->value<double>() - ub[i]) / (ub[i]-lb[i]);
            IElement e = *(var) - (m+1) * (ub[i]-lb[i]);
            imap->defineDecisionVar(i, e);
        }
    }
}

} // namespace Tigon
