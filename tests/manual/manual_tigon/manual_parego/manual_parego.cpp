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
#include <iostream>
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

inline void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

void bmk_ParEGO_DTLZ2()
{
    TRAND.reset();
    cout.precision(4);

    int   dVecSize = 3;
    int   oVecSize = 2;
    int   nDirs    = 20;
    int   popSize  = 100;
    int   budget   = popSize+nDirs*10;
    int   maxSurrogateSize = 50;

    PSetBase*     base      = new PSetBase();
    IFormulation* prob      = new IFormulation(base);
    RandomInit*   init      = new RandomInit(prob);
    Evaluator*    evaluator = new Evaluator(init);
    ParEGO*       alg       = new ParEGO(evaluator);

    IFunctionSPtr func      = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);

    alg->defineReferenceSetSize(nDirs);
    alg->defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    alg->TP_defineMaxSolutions(maxSurrogateSize);

    alg->defineBudget(budget);

    TVector<IMappingSPtr> newMappings;
    TVector<double> dirVec;
    while(alg->remainingBudget() > 0) {
        alg->evaluate();

        cout << "Reamining budget: " << alg->remainingBudget() << endl;
//        cout << "Iteration " << alg->currentIteration() << endl;
        if(alg->currentIteration() == 0) {
            TVector<IMappingSPtr> initialSet = alg->evaluatedMappings();
            cout << "f{1} = [";
            for(int i=0; i<initialSet.size(); i++) {
                dispVector(initialSet[i]->doubleObjectiveVec(), " ", "; ");
            }
            cout << "];" << endl;

            cout << "d{1} = [0,0];" << endl;
        } else {
            cout << "f{" << alg->currentIteration()+1 << "} = [";
            for(int i=0; i<newMappings.size(); i++) {
                dispVector(newMappings[i]->doubleObjectiveVec(), " ", "; ");
            }
            cout << "];" << endl;

            cout << "d{" << alg->currentIteration()+1 << "} = [";
            dispVector(dirVec, " ", "];");
            cout << endl;

        }
        cout << "\tNumber of IMappings: " <<
                alg->evaluatedMappings().size() + 1 << endl;

        newMappings = alg->newMappings();
        dirVec = alg->dirVec();

        alg->incrementIteration();
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}


int main(int argc, char* argv[])
{
    cout << "Running Benchmarks..." << endl;

    bmk_ParEGO_DTLZ2();

    return 0;
}
