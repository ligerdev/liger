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
#include <iostream>
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

void bmk_MOGA_DTLZ2()
{
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(90);

    PSetBase*              base = new PSetBase();
    IFormulation*          prob = new IFormulation(base);

//    IFunctionSPtr          func  = IFunctionSPtr(new DTLZ2);
//    func->TP_defineNInputs(dVecSize);
//    func->TP_defineNOutputs(oVecSize);
//    prob->appendFunction(func);

    IFunctionSPtr func(new CTP1);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2 << 3;
    ProblemSPtr probb(new Problem);
    probb->appendFunction(func, paramInd, cnstrInd, objInd);
    probb->processProblemDefinition();
    TVector<double> threshVec(2, 0.0);
    TVector<ElementType> types(2, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    probb->defineThresholdVector(thVec);
    probb->processProblemDefinition();

    prob->defineProblem(probb);

    TVector<IElementSPtr> goals
            = { IElementSPtr(new IElement(RealType, 0.3)),
                IElementSPtr(new IElement(RealType, Tigon::Lowest))};
    prob->problem()->defineGoalVector(goals);

//    TVector<IElementSPtr> goals
//            = {IElementSPtr(new IElement(RealType, 0.95))
//               , IElementSPtr(new IElement(RealType, 0.4))
//               , IElementSPtr(new IElement(RealType, 0.4))};

//    prob->problem()->defineGoalVector(goals);

    prob->problem()->processProblemDefinition();

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    MOGA* alg   = new MOGA(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running MOGA for " << maxIter << " iterations" << endl;
    cout << " Problem: DTLZ2"  << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();

//        if(iter==200) {
//            TVector<IElementSPtr> goals
//                    = {IElementSPtr(new IElement(RealType, 0.4))
//                       , IElementSPtr(new IElement(RealType, 0.4))
//                       , IElementSPtr(new IElement(RealType, 0.4))};

//            prob->problem()->defineGoalVector(goals);
//            prob->problem()->processProblemDefinition();
//        }

        //Display the results:
        if(iter == maxIter-1) {
            TStringList tags;
            tags << Tigon::TMatingPool << Tigon::TForSelection;
            ISet* ndPop = alg->setWithTags(tags);
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

int main()
{
    using std::chrono::steady_clock;

    cout << "Running Benchmarks..." << endl;
    steady_clock::time_point start = steady_clock::now();

    bmk_MOGA_DTLZ2();


    steady_clock::time_point end   = steady_clock::now();

    std::cout << "Time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" <<std::endl;

    return 0;
}
