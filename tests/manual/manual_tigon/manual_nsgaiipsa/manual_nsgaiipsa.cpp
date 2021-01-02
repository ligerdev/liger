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
#include <iostream>
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

void bmk_NSGAIIPSA()
{
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 210;
    int maxIter = 100;

    cout.precision(5);
    srand(465654);

    PSetBase*              base = new PSetBase();
    IFormulation*          prob = new IFormulation(base);

    IFunctionSPtr          func  = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAIIPSA* alg   = new NSGAIIPSA(evaluator);

    cout << "------------------------------------------------" <<
            "------------------------------------------------" <<
            "-----------------\nRunning NSGAIIPSA for " << maxIter <<
            " iterations" << endl;
    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags({Tigon::TForNextIteration, Tigon::TForSelection});
        ISet* ndPop = alg->setWithTags(tags);
        if(iter == maxIter-1) {
            cout << "fND{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;
        } else if(!(iter%10)){
            cout << iter << "/" << maxIter << endl;
        }
    }

    delete alg;
    delete init;
    delete prob;
    delete base;
}


int main(int argc, char* argv[])
{
    cout << "Running Benchmarks..." << endl;

    bmk_NSGAIIPSA();
    return 0;
}
