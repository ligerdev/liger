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
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>

namespace Tigon {
namespace Operators {

IntegerMutation::IntegerMutation()
{
    initialise();
}

IntegerMutation::IntegerMutation(Tigon::Representation::IPSet* ipset)
    : IPerturbation(ipset)
{
    initialise();
}

IntegerMutation::~IntegerMutation()
{

}

void IntegerMutation::evaluateNode()
{
    TVector<IElement> lb = boxConstraints()->lowerBounds();
    TVector<IElement> ub = boxConstraints()->upperBounds();

    int nvars=0; // number of supported variables
    for(int i=0; i<decisionVecSize(); i++) {
        if( isSupportedElementType(decisionVecTypes().at(i)) ) {
            nvars++;
        }
    }

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        for(IMappingSPtr imap : oSet->all()) {

            // probability of mutating a solution
            if( doSolutionMutation() ) {

                // for each decision variable
                for(int i=0; i<decisionVecSize(); i++) {

                    // probability of mutating a variable
                    if( doVariableMutation() ) {

                        if( isSupportedElementType(decisionVecTypes().at(i)) ) {

                            double stepSize = TP_stepSize();
                            if(stepSize<0) {
                                double width = ub[i].value() - lb[i].value();
                                stepSize = TP_widthPercent() * width;
                            }

                            double s = std::max(1.0, stepSize);
                            double tp = s/(double)nvars;

                            double phi = 1.0 - tp/(1.0 + std::sqrt(1.0 + tp*tp));

                            double u1 = TRAND.randUni();
                            double u2 = TRAND.randUni();

                            double log1 = std::log(1.0-u1)/std::log(1.0-phi);
                            double log2 = std::log(1.0-u2)/std::log(1.0-phi);

                            int G1 = floor(log1);
                            int G2 = floor(log2);

                            IElement var = *(imap->decisionVar(i));
                            imap->defineDecisionVar(i, var + G1 - G2);
                        }
                    }
                }
                // reflect the solution variables back to feasible decision
                // space, if necessary
                reflectIntoFeasibleDomain(imap, boxConstraints());
            }
        }
    }
}

double IntegerMutation::TP_stepSize() const
{
    return m_s;
}

void IntegerMutation::TP_defineStepSize(double s)
{
    m_s = s;
}

double IntegerMutation::TP_widthPercent() const
{
    return m_wp;
}

void IntegerMutation::TP_defineWidthPercent(double wp)
{
    if((wp>=0.0) && (wp<=1.0)){
        m_wp = wp;
    }
}

void IntegerMutation::initialise()
{
    addProperty("Step Size"
                , TString("Step size of the mutation operator.\nDefault is -1.")
                , getTType(double));

    addProperty("WidthPercent"
                , TString("Specifies the percentage of the variable width (as "
                          "defined by its bounds) that is to be used as the "
                          "value of the sigma. Default value is 0.1.")
                , getTType(double));

    TP_defineStepSize(Tigon::DefaultMutationStepSize);
    TP_defineWidthPercent(Tigon::DefaultWidthPercent);

    TStringList types;
    types << Tigon::ETInteger << Tigon::ETOrdinal;
    defineSupportedElementTypes(types);
}

TString IntegerMutation::name()
{
    return TString("Integer Mutation");
}

TString IntegerMutation::description()
{
    return TString("Integer mutation operator is taken from the paper, G. "
                   "Rodolph, An Evolutionary Algorithm for Integer Programming, "
                   "In PPSN III, International Conference on Evolutionary "
                   "Computation. The Proceedings of the Third Conference on "
                   "Parallel Problem Solving from Nature, Lecture Notes in "
                   "Computer Science, Vol. 866, (pp. 139-148). Berlin: Springer.");
}

} // namespace Operators
} // namespace Tigon
