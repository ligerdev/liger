/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Operators/Transformations/Normalisation.h>

namespace Tigon {
namespace Operators {

AcromuseDiversity::AcromuseDiversity()
{
    AcromuseDiversity::initialise();
}

AcromuseDiversity::AcromuseDiversity(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    AcromuseDiversity::initialise();
}

AcromuseDiversity::~AcromuseDiversity()
{

}

void AcromuseDiversity::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();

    addOutputTag(Tigon::TFitness);
    addOutputTag(Tigon::TForSelection);

    addInputTag(Tigon::TForEvaluation);
}

void AcromuseDiversity::evaluateNode()
{
    clearOutputSets();
    ISet* iSet = inputSet(0);
    // 1st set are the original Optimization Mappings
    // 2nd set are the normalised Generic Mappings with mathcing indeces
    ISet* optSet = appendOutputSet();
    optSet->append(iSet);

    Normalisation* norm = new Normalisation(this);
    norm->defineInputTags(inputTags());
    norm->TP_defineInputBoundsType(Tigon::GlobalBounds);
    norm->TP_defineOutputBoundsType(Tigon::CurrentDataBounds);
    norm->TP_defineFitnessBoundsType(Tigon::CurrentDataBounds);

    norm->evaluateOnlyThisNode();
    ISet* genSet = appendOutputSet();
    genSet->append(norm->outputSet(0));

    double totalFitness = 0;
    double maxFitness = Tigon::Lowest;
    double minFitness = Tigon::Highest;

    for(int i=0; i<genSet->size(); i++) {
        // ACROMUSE operators consider fitness maximisation
        double f = 1.0 - genSet->at(i)->doubleCost();
        totalFitness += f;
        minFitness = std::min(minFitness, f);
        maxFitness = std::max(maxFitness, f);
    }

    insertMultiPack(Tigon::AcMinF, TVector<double>({minFitness}));
    insertMultiPack(Tigon::AcMaxF, TVector<double>({maxFitness}));

    TVector<double> weights(genSet->size());
    TVector<double> xBar(decisionVecSize(), 0.0);
    TVector<double> weightedXBar(decisionVecSize(), 0.0);
    TVector<double> sigmaS(decisionVecSize(), 0.0);
    TVector<double> sigmaH(decisionVecSize(), 0.0);
    TVector<double> HPDi(genSet->size(), 0.0);


    // Average input vector and Average health input vector
    for(int i=0; i<genSet->size(); i++) {
        IMappingSPtr imap = genSet->at(i);
        weights[i] = (1.0 - imap->doubleCost()) / totalFitness;
        for(int var=0; var<decisionVecSize(); var++) {
            xBar[var] += imap->doubleDecisionVar(var);
            weightedXBar[var] += weights[i] * imap->doubleDecisionVar(var);
        }
    }
    for(int var=0; var<decisionVecSize(); var++) {
        xBar[var] /= genSet->size();
    }

    //SPD and HPD
    for(int i=0; i<genSet->size(); i++) {
        IMappingSPtr imap = genSet->at(i);
        for(int var=0; var<decisionVecSize(); var++) {
            sigmaS[var] +=  std::pow(imap->doubleDecisionVar(var) - xBar[var], 2.0);
            sigmaH[var] +=  std::pow(imap->doubleDecisionVar(var) - weightedXBar[var], 2.0)
                    * weights[i];
            HPDi[i] += std::pow(imap->doubleDecisionVar(var) - weightedXBar[var], 2.0);
        }
        imap->defineCost(1.0 - weights[i] * std::sqrt(HPDi[i]));
    }

    double SPD = 0.0;
    double HPD = 0.0;
    for(int var=0; var<decisionVecSize(); var++) {
        sigmaS[var] = std::sqrt(sigmaS[var] / genSet->size());
        sigmaH[var] = std::sqrt(sigmaH[var]);

        SPD += sigmaS[var];
        HPD += sigmaH[var];
    }
    SPD /= decisionVecSize();
    HPD /= decisionVecSize();

    insertMultiPack(Tigon::AcSpd, TVector<double>({SPD}));
    insertMultiPack(Tigon::AcHpd, TVector<double>({HPD}));

    delete norm;
}

TString AcromuseDiversity::name()
{
    return TString("ACROMUSE Population Diversity Indicator");
}

TString AcromuseDiversity::description()
{
    return TString("Computes the Standard and Health Population Diversity "
                   "(SPD and HPD, respectively) according to ACROMUS"
                   "Mc Ginley et al. 2011.\nThe measure is modified so lower "
                   "fitness values are preffered.");
}

} // namespace Operators
} // namespace Tigon
