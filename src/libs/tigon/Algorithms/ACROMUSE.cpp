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
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/ACROMUSE.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseDiversity.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.h>
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseMutation.h>
#include <tigon/Operators/Directions/SinglePointCrossOver.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

ACROMUSE::ACROMUSE()
{
    initialise();
}

ACROMUSE::ACROMUSE(Representation::IPSet* ipset)
    : IAlgorithm(ipset)
{
    initialise();
}

ACROMUSE::~ACROMUSE()
{

}

IMappingSPtr ACROMUSE::bestMapping() const
{
    return operators().at(0)->outputSet(0)->at(0);
}

void ACROMUSE::initialise()
{
    AcromuseEliteFiltration*   elite  = new AcromuseEliteFiltration(this);
    AcromuseDiversity*         div    = new AcromuseDiversity(this);
    AcromuseFiltrationForDirection*
                               filtD  = new AcromuseFiltrationForDirection(this);
    AcromuseFiltrationForPerturbation*
                               filtM  = new AcromuseFiltrationForPerturbation(this);
    AcromuseMutation*          mutat  = new AcromuseMutation(this);
    SinglePointCrossOver*      co     = new SinglePointCrossOver(this);
    PolynomialMutation*        pm     = new PolynomialMutation(this);
    IntegerMutation*           im     = new IntegerMutation(this);
    CategoricalPerturpation*   cPert  = new CategoricalPerturpation(this);
    MergeForNextIteration*     merge  = new MergeForNextIteration(this);

    appendOperator(elite);
    appendOperator(div);
    appendOperator(filtD);
    appendOperator(filtM);
    appendOperator(mutat);
    appendOperator(co);
    appendOperator(pm);
    appendOperator(im);
    appendOperator(cPert);
    appendOperator(merge);

    //eval->TP_defineSingleObjective(true);
    pm->defineOutputTags(co->outputTags());
    pm->TP_defineMutationDistributionIndex(50);
    cPert->defineOutputTags(co->outputTags());
}

TString ACROMUSE::name()
{
    return TString("ACROMUSE");
}

TString ACROMUSE::description()
{
    return TString("Single objective evolutionary algorithm with adaptive "
                   "parameter.\nThe implementation is based on "
                   "Mc Ginley et al. \"Maintaining healthy population diversity "
                   "using adaptive crossover, mutation, and selection\" Trans. "
                   "Evol. Comput., vol. 15, no. 5, pp. 692-714.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, ACROMUSE)
