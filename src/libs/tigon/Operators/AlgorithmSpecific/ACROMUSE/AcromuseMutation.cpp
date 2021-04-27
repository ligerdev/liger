/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Operators {

AcromuseMutation::AcromuseMutation()
{
    m_name = TString("ACROMUSE Mutation");
    m_description = TString("Mutation rate is deremined by SPD and the "
                            "solution's fitness. Random re-initialisation is "
                            "used for mutated variables.");
}

AcromuseMutation::AcromuseMutation(Tigon::Representation::IPSet* ipset)
    : IPerturbation(ipset)
{
    m_name = TString("ACROMUSE Mutation");
    m_description = TString("Mutation rate is deremined by SPD and the "
                            "solution's fitness. Random re-initialisation is "
                            "used for mutated variables.");
}

AcromuseMutation::~AcromuseMutation()
{

}

void AcromuseMutation::evaluateNode()
{
    if(!multiPackExists(Tigon::AcSpd)) {
        return;
    }

    ISet* optSet = outputSet(0);
    ISet* genSet = outputSet(1);

    double SPD = multiPack(Tigon::AcSpd)[0];
    double maxF = multiPack(Tigon::AcMaxF)[0];
    double minF = multiPack(Tigon::AcMinF)[0];

    double pDiv = Tigon::AcK * (Tigon::AcMaxSpd - SPD) / Tigon::AcMaxSpd;

    TVector<double> lb = IElementVecToRealVec(boxConstraints()->lowerBounds());
    TVector<double> ub = IElementVecToRealVec(boxConstraints()->upperBounds());

    TVector<ElementType> dVarTypes = decisionVecTypes();

    for(int i=0; i<optSet->size(); i++) {
        IMappingSPtr optMap = optSet->at(i);
        IMappingSPtr genMap = genSet->at(i);
        double f = 1.0 - genMap->doubleObjectiveVar(0);
        double pFit = Tigon::AcK * (maxF - f) / (maxF - minF);
        m_mutationProbability = (pDiv + pFit) / 2.0;
        for(int j=0; j<decisionVecSize(); j++) {
            if( doOperate() ) {
                double newVal;
                switch(dVarTypes[j]) {
                case IntegerType:
                case OrdinalType:
                case NominalType:
                {
                    newVal = TRAND.randInt(ub[j] - lb[j] + 1, lb[j]);
                    break;
                }
                case RealType:
                {
                    double r = TRAND.randUni();
                    newVal = lb[j] + r*(ub[j]-lb[j]);
                    break;
                }
                default:
                {
                    throw TException(this->className(),
                                     UnrecognisedTypeException);
                }
                }
                optMap->defineDecisionVar(j, newVal);
            }
        }
    }
}

TString AcromuseMutation::name()
{
    return m_name;
}

TString AcromuseMutation::description()
{
    return m_description;
}

bool AcromuseMutation::doOperate()
{
    if(m_mutationProbability >= 1.0) {
        return true;
    } else if(m_mutationProbability <= 0.0) {
        return false;
    } else {
        return (m_mutationProbability > TRAND.randUni());
    }
}

} // namespace Operators
} // namespace Tigon
