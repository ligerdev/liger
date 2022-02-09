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

void bmk_NSGAII_DTLZ2()
{
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 55;
    int maxIter = 2000;

    cout.precision(5);
    TRAND.defineSeed(90);

    PSetBase*              base = new PSetBase();
    IFormulation*          prob = new IFormulation(base);

    IFunctionSPtr          func  = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

//    TVector<IElementSPtr> goals
//            = {IElementSPtr(new IElement(RealType, 0.95))
//               , IElementSPtr(new IElement(RealType, 0.4))
//               , IElementSPtr(new IElement(RealType, 0.4))};

//    prob->problem()->defineGoalVector(goals);
//    prob->problem()->processProblemDefinition();

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
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
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

void bmk_NSGAII_CTP1()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP1);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2 << 3;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(2, 0.0);
    TVector<ElementType> types(2, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP1" << endl;
    cout << "################################################" << endl;
    cout << endl;

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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP2()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP2);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP2" << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP3()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP3);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP3" << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP4()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP4);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP4" << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP5()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP5);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP5" << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP6()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP6);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP6" << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
        //Display the results:
        TStringList tags;
        tags << Tigon::TForNextIteration << Tigon::TForSelection;
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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

void bmk_NSGAII_CTP7()
{
    int populationSize = 210;
    int maxIter = 500;

    cout.precision(5);
    srand(465654);

    /// Define the problem
    IFunctionSPtr func(new CTP7);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    form->defineProblem(prob);

    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);
    NSGAII* alg   = new NSGAII(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running NSGAII for " << maxIter << " iterations" << endl;
    cout << " Problem: CTP7" << endl;
    cout << "################################################" << endl;
    cout << endl;

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

            cout << "d{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleDecisionVec();
                for(int var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << "c{" << alg->currentIteration() << "} = [";
            for(int i=0; i < ndPop->size(); i++) {
                TVector<double> vec = ndPop->at(i)->doubleConstraintVec();
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
    delete form;
    delete base;
}

int main(int argc, char* argv[])
{
    cout << "Running Benchmarks..." << endl;

    bmk_NSGAII_DTLZ2();
//    bmk_NSGAII_CTP1();
//    bmk_NSGAII_CTP2();
//    bmk_NSGAII_CTP3();
//    bmk_NSGAII_CTP4();
//    bmk_NSGAII_CTP5();
//    bmk_NSGAII_CTP6();
//    bmk_NSGAII_CTP7();

    return 0;
}
