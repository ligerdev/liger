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
#include <tigon/Operators/Evaluators/Validation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/tigonengineregistry.h>

#include <future>
using namespace std;

namespace Tigon {
namespace Operators {

Validation::Validation()
{    
    initialise();
}

Validation::Validation(Tigon::Representation::IPSet *ipset)
    : IEvaluator(ipset)
{
    initialise();
}

Validation::~Validation() {

}

void Validation::TP_defineNEvaluations(int n)
{
    if(n >= 0) {
        m_nEvaluations = n;
    }
}

int Validation::TP_nEvaluations() const
{
    return m_nEvaluations;
}

void Validation::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        int evalCost = 0;

        ///\todo resume parallel support
        if(TP_isParallel()) {
//            TVector<future<int>> results;
//            TVector<int> resIdx;

//            for(int idx=0; idx<oSet->size(); idx++) {
//                IMappingSPtr imap = oSet->at(idx);
//                // validate only non-validated imappings
//                if(imap->isValidated() != SampledValidation) {
//                    resIdx.push_back(idx);
//                    results.push_back(std::async(launch::async,
//                                                 &IMapping::multipleEvaluate,
//                                                 imap.get(), m_nEvaluations));
//                }
//            }

//            for (int i=0; i < results.size(); i++) {
//                // Blocks until all evaluations have returned a value
//                int res = results[i].get();
//                if(res < 0) {
//                    // Error!!
//                } else if(res > 0){
//                    if(TP_isCountEvaluations()) {
//                        if(TP_isCountPerFunction()) {
//                            evalCost += res;
//                        } else {
//                            evalCost++;
//                        }
//                    }

//                    oSet->at(resIdx[i])->defineValidated(SampledValidation);
//                    // TODO: if(!TP_isOffLine) {
//                    updateIdealNadirVec(oSet->at(resIdx[i]));
//                    // }
//                }
//            }
        } else {
            for(int idx=0; idx<usedBudget(); idx++) {
                IMappingSPtr imap = oSet->at(idx);
                if(imap->isValidated() == SampledValidation) {
                    continue;
                } //otherwise validate it

                int evaluationCost=0;

                int nObj = imap->objectiveVec().size();
                TVector<TVector<double> > objSamples(nObj,
                                                    TVector<double>(m_nEvaluations));
                int nCstr = imap->constraintVec().size();
                TVector<TVector<double> > cstrSamples(nCstr,
                                                     TVector<double>(m_nEvaluations));
                int nUnused = imap->unusedVec().size();
                TVector<TVector<double> > unusedSamples(nUnused,
                                                       TVector<double>(m_nEvaluations));

                for(int s=0; s<m_nEvaluations; s++) {
                    imap->defineEvaluated(false);
                    evaluationCost = imap->evaluate();
                    // Record the sample
                    for(int k=0; k<nObj; k++) {
                        objSamples[k][s] = imap->objectiveVar(k)->value<double>();
                    }
                    // Record constraint and unused only if they exist
                    if(nCstr > 0) {
                        for(int k=0; k<nCstr; k++) {
                            cstrSamples[k][s] = imap->constraintVar(k)->value<double>();
                        }
                    }
                    if(nUnused > 0) {
                        for(int k=0; k<nUnused; k++) {
                            unusedSamples[k][s] = imap->unusedVar(k)->value<double>();
                        }
                    }
                }
                imap->defineEvaluated(true);

                for(int j = 0; j < nObj; j++) {
                    SampledDistribution* dist = new SampledDistribution(objSamples[j]);
                    imap->objectiveVec()[j]->defineDist(dist);
                }

                // Consider constraint and unused only if they exist
                if(nCstr > 0) {
                    for(int j=0; j<nCstr; j++) {
                        SampledDistribution* dist = new SampledDistribution(cstrSamples[j]);
                        imap->constraintVec()[j]->defineDist(dist);
                    }
                }

                if(nUnused > 0) {
                    for(int j=0; j<nUnused; j++) {
                        SampledDistribution* dist = new SampledDistribution(unusedSamples[j]);
                        imap->unusedVec()[j]->defineDist(dist);
                    }
                }

                if(evaluationCost < 0) {
                    // Error!!
                } else if(evaluationCost > 0) {
                    if(TP_isCountEvaluations()) {
                        if(TP_isCountPerFunction()) {
                            evalCost += evaluationCost;
                        } else {
                            evalCost++;
                        }
                    }
                    imap->defineValidated(SampledValidation);
                    // TODO: if(!TP_isOffLine) {
                    updateIdealNadirVec(imap);
                }
            }
        }

        decrementBudget(evalCost);
    }
}

void Validation::TP_defineOperateOnFinal(bool o) {
    m_operateOnFinal = o;
}

bool Validation::TP_operateOnFinal() const {
    return m_operateOnFinal;
}

TString Validation::name()
{
    return TString("Validation");
}

TString Validation::description()
{
    return TString("Evaluates every IMapping flagged for validation N times.\n"
                   "The multiple evaluations (of each objective and "
                   "constraint) construct a sampled distribution.");
}

void Validation::initialise()
{   
    addProperty("NEvaluations"
                , TString("Number of repeated evaluations of every "
                          "IMapping.\nDefault is ")
                , typeid(int).hash_code());
    addProperty("OperateOnFinal"
                , TString("Disable the repeated evaluations during "
                          "the optimization process, and only validate "
                          "the final solutions.")
                , typeid(bool).hash_code());

    addAdditionalOutputTag(Tigon::TValidation);

    TP_defineNEvaluations(Tigon::DefaultMonteCarloSample);
    TP_defineCountEvaluations(false);
    TP_defineOperateOnFinal(true);
}

} // namespace Operators
} // namespace Tigon

REGISTER_IPSET_FACTORY(Operators, Validation)
