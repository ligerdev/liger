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
#include <chrono>
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

void bmk_MOEAD_1()
{
    int iter;
    //int dVecSize = 7;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 55;
    int maxIter = 400;

    cout.precision(5);
    TRAND.defineSeed(90);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

//    TVector<IElementSPtr> goals
//            = {IElementSPtr(new IElement(RealType, -8.9884656743115785e+307))
//               , IElementSPtr(new IElement(RealType, -8.9884656743115785e+307))
//               , IElementSPtr(new IElement(RealType, -8.9884656743115785e+307))};

//    TVector<IElementSPtr> goals
//            = {IElementSPtr(new IElement(RealType, 0.6))
//               , IElementSPtr(new IElement(RealType, 0.6))
//               , IElementSPtr(new IElement(RealType, 0.6))};

//    prob->problem()->defineGoalVector(goals);
//    prob->problem()->processProblemDefinition();

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);
    //evaluator->TP_defineParallel(false);

    MOEAD* alg = new MOEAD(evaluator);

    //alg->TP_defineRegularisationMethod(Tigon::InitialOrder);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        if(iter==200) {
            TVector<IElementSPtr> goals
                    = {IElementSPtr(new IElement(RealType, 0.4))
                       , IElementSPtr(new IElement(RealType, 0.4))
                       , IElementSPtr(new IElement(RealType, 0.4))};

            prob->problem()->defineGoalVector(goals);
            prob->problem()->processProblemDefinition();
        }

        ISet* pop = alg->setWithTags(tags);

        cout << endl;
        cout << "********" << endl;
        cout << "Iter N." << iter+1 << endl;
        cout << "********" << endl;

        // objectives
        cout << endl;
        cout << "f=[";
        for(int i=0; i< pop->size(); i++) {
            TVector <double> vec = pop->at(i)->doubleObjectiveVec();
            cout << vec << "; ";
        }
        cout << "];" << endl;

        // weight vectors
        cout << endl;
        cout << "w=[";
        for(int i=0; i< pop->size(); i++) {
            TVector<double> w = pop->at(i)->weightingVec();
            cout << w << "; ";
        }
        cout << "];" << endl;

        // ideal
        TVector<double> ideal = IElementVecToRealVec(prob->problem()->idealVector());
        cout << "ideal=" << ideal << ";" << endl;

        // nadir
        TVector<double> nadir = IElementVecToRealVec(prob->problem()->nadirVector());
        cout << "nadir=" << nadir << ";" << endl;

        // anti ideal
        TVector<double> iideal = IElementVecToRealVec(prob->problem()->antiIdealVector());
        cout << "antiideal=" << iideal << ";" << endl;
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void bmk_MOEAD_2()
{
    int iter;
    //int dVecSize = 7;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 210;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(90);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);
    //evaluator->TP_defineParallel(false);

    MOEAD* alg = new MOEAD(evaluator);

    alg->TP_defineRegularisationMethod(Tigon::CentroidBasedOrder);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        //qDebug() << "Iteration{" << alg->currentIteration() << "}";

        //if(iter==(maxIter-1)) {
        cout << endl;
        cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
        ISet* pop = alg->setWithTags(tags);
        for(int i=0; i< pop->size(); i++) {
            TVector <double> vec = pop->at(i)->doubleObjectiveVec();
            for(int var=0; var<vec.size(); var++) {
                cout << vec[var] << " ";
            }
            cout << "; ";
        }
        cout << "];" << endl;
        //}
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void bmk_MOEAD_goalBelowSimplexLatticeObjectiveReduction() {

    int M = 4;
    int dVecSize = 12;
    int oVecSize = M;
    int populationSize = 210;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(465654);

    //Function Definition
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    // MOEAD setup
    PSetBase*              base = new PSetBase();
    IFormulation* prob = new IFormulation(base);
    prob->appendFunction(func);
    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);
    Evaluator* evaluator = new Evaluator(init);

    MOEAD* alg = new MOEAD(evaluator);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    TVector<double> goals;

    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        cout << "iteration " << iter << endl;

        if(iter==0) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==maxIter/2) {
            cout << endl;
            goals << 0.2 << 0.2 << 0.2 << 0.2;
            cout << "goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << goals[i] << " ";
            }
            cout << endl;
            TVector<ElementType> types(M, RealType);
            prob->defineGoalVec(createIElementSPtrVector(goals, types));

            TVector<bool> essentialObjs;
            essentialObjs << true << true << true << false;
            prob->problem()->defineEssentialObjectives(essentialObjs);

            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==(maxIter-1)) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector <double> normGoalVector(oVecSize);
            for(int i=0;i<oVecSize;i++) {
                normGoalVector[i]=goals[i];
            }
            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());
            normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "norm goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << normGoalVector[i] << " ";
            }
            cout << endl;

        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void bmk_MOEAD_goalBelowSimplexLattice() {

    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 210;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(465654);

    //Function Definition
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    // MOEAD setup
    PSetBase*              base = new PSetBase();
    IFormulation* prob = new IFormulation(base);
    prob->appendFunction(func);
    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);
    Evaluator* evaluator = new Evaluator(init);

    MOEAD* alg = new MOEAD(evaluator);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    TVector<double> goals;

    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        if(iter==0) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==maxIter/2) {
            cout << endl;
            goals << 0.15 << 0.6 << Tigon::Lowest;
            cout << "goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << goals[i] << " ";
            }
            cout << endl;
            TVector<ElementType> types(3, RealType);
            prob->defineGoalVec(createIElementSPtrVector(goals, types));

            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==(maxIter-1)) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector <double> normGoalVector(oVecSize);
            for(int i=0;i<oVecSize;i++) {
                normGoalVector[i]=goals[i];
            }
            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());
            normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "norm goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << normGoalVector[i] << " ";
            }
            cout << endl;

        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void bmk_MOEAD_goalAboveSimplexLattice() {
    // Tests the MOEAD with goals vectors


    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 210;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(465654);

    //Function Definition
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    // MOEAD setup
    PSetBase*              base = new PSetBase();
    IFormulation* prob = new IFormulation(base);
    prob->appendFunction(func);
    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);
    Evaluator* evaluator = new Evaluator(init);

    MOEAD* alg = new MOEAD(evaluator);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    TVector<double> goals;

    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        if(iter==0) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==maxIter/2) {
            cout << endl;
            goals << 2.30 << 0.6 << Tigon::Lowest;
            cout << "goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << goals[i] << " ";
            }
            cout << endl;
            TVector<ElementType> types(3, RealType);
            prob->defineGoalVec(createIElementSPtrVector(goals, types));

            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter==(maxIter-1)) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector <double> normGoalVector(oVecSize);
            for(int i=0;i<oVecSize;i++) {
                normGoalVector[i]=goals[i];
            }
            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());
            normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "norm goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << normGoalVector[i] << " ";
            }
            cout << endl;

        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void test() {

    int M=2;
    int dVecSize = 4;
    int oVecSize = M;
    int populationSize = 30;
    int maxIter = 200;

    cout.precision(5);
    TRAND.defineSeed(465654);

    //Function Definition
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    // MOEAD setup
    PSetBase*              base = new PSetBase();
    IFormulation* prob = new IFormulation(base);
    prob->appendFunction(func);
    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);
    Evaluator* evaluator = new Evaluator(init);

    MOEAD* alg = new MOEAD(evaluator);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    TVector<double> goals;
    goals << 0.2 << 0.2;
    TVector<ElementType> types(M, RealType);
    prob->defineGoalVec(createIElementSPtrVector(goals, types));
    prob->problem()->processProblemDefinition();

    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        cout << "iteration " << iter << endl;

        if(iter==0) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;
        }

        if(iter>0) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            int N = pop->size();
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(int var=0; var<oVecSize; var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            cout << endl;
            cout << "weight vector:" << endl;
            cout << "[ ";
            for(int i=0; i<N; i++) {
                TVector <double> vec = pop->at(i)->weightingVec();
                int vecSize = vec.size();
                for(int j=0; j<vecSize; j++) {
                    cout << vec[j] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;

            TVector <double> normGoalVector(oVecSize);
            for(int i=0;i<oVecSize;i++) {
                normGoalVector[i]=goals[i];
            }
            TVector<double> ideal = IElementVecToRealVec(base->idealVec());
            TVector<double> nadir = IElementVecToRealVec(base->nadirVec());
            normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

            cout << endl;
            cout << "ideal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << ideal[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "nadir vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << nadir[i] << " ";
            }
            cout << endl;

            cout << endl;
            cout << "norm goal vector: ";
            for(int i=0; i<oVecSize; i++) {
                cout << normGoalVector[i] << " ";
            }
            cout << endl;

        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

int main()
{
    using std::chrono::steady_clock;

    cout << "Running Benchmarks..." << endl;
    steady_clock::time_point start = steady_clock::now();
    // test general MOEA/D
    bmk_MOEAD_1();
    //bmk_MOEAD_2();

    // test MOEA/D with goals
//    bmk_MOEAD_goalBelowSimplexLattice();
//    bmk_MOEAD_goalAboveSimplexLattice();
    //bmk_MOEAD_goalBelowSimplexLatticeObjectiveReduction();

    steady_clock::time_point end   = steady_clock::now();

    std::cout << "Time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" <<std::endl;

    return 0;
}

