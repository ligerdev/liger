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
#include <QtTest>
#include <QDebug>

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

#define TST_PAREGO_OPERATORS
#define PAREGO_WORK_FLOW

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_parego : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_PAREGO_OPERATORS
    void test_SurrogateBasedOptimizer();
#endif

#ifdef PAREGO_WORK_FLOW
    void test_ParEGO_workflow();
    void test_ParEGO();
#endif
};

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

inline void writeVector(QTextStream &st, TVector<double> vec, QString sep = "\t", QString endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        st << vec[i];
        if(i<vec.size()-1) {
            st << sep;
        }
    }
    st << endLine;
}


#ifdef TST_PAREGO_OPERATORS
void tst_parego::test_SurrogateBasedOptimizer()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 2;
    int   nDirs    = 10;
    int   popSize  = 100;
    int   budget   = popSize+nDirs*1;
    int   maxSurrogateSize = 50;
    double nRaduis  = 0.2;

    PSetBase*                   base = new PSetBase();
    IFormulation*               prob = new IFormulation(base);
    RandomInit*                 init = new RandomInit(prob);
    Evaluator*                  eval = new Evaluator(init);

    /// ParEGO starts
    SimplexLatticeDirectionIterator* dirs =
                                       new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(scal);
    SurrogateBasedOptimizer*    opt  = new SurrogateBasedOptimizer(filt);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    eval->defineBudget(budget);
    dirs->TP_defineReferenceSetSize(nDirs);
    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    filt->TP_defineMaxSolutions(maxSurrogateSize);
    opt->TP_defineErrorMethod(Tigon::ErrDensityBased);
    opt->TP_defineNeighbourhoodRadius(nRaduis);

    eval->evaluate();
    ISet* oSet = eval->outputSet(0);
    QCOMPARE(oSet->size(), popSize);

    /// checks that SurrogateBasedOptimizer adds a solution to the main
    /// optimization set
    opt->evaluate();
    QCOMPARE(oSet->size(), popSize+1);
}

#endif

#ifdef PAREGO_WORK_FLOW
void tst_parego::test_ParEGO_workflow()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 2;
    int   nDirs    = 10;
    int   popSize  = 100;
    int   budget   = popSize+nDirs*1;
    int   maxSurrogateSize = 50;
    double nRaduis  = 0.2;

    PSetBase*                   base = new PSetBase();
    IFormulation*               prob = new IFormulation(base);
    RandomInit*                 init = new RandomInit(prob);
    Evaluator*                  eval = new Evaluator(init);

    /// ParEGO starts
    SimplexLatticeDirectionIterator* dirs =
                                       new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    ConstraintPenalty*          pnlt = new ConstraintPenalty(scal);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(pnlt);
    SurrogateBasedOptimizer*    opt  = new SurrogateBasedOptimizer(filt);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    eval->defineBudget(budget);
    dirs->TP_defineReferenceSetSize(nDirs);
    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    filt->TP_defineMaxSolutions(maxSurrogateSize);
    opt->TP_defineErrorMethod(Tigon::ErrDensityBased);
    opt->TP_defineNeighbourhoodRadius(nRaduis);

    while(opt->remainingBudget() > 0) {
        opt->evaluate();
        ISet* pop = init->outputSet(0);

        cout << "Iteration: " << opt->currentIteration() <<
                " \tRemaining budget: " << opt->remainingBudget() << endl;
        cout << "\tDirection vector: \t";
        dispVector(opt->dirVec(), "\t", "\n");
        cout << "\tLast solutions added: \n";
        for(int i = 0; i < pop->size(); i++) {
            if(!(pop->at(i)->isEvaluated())) {
                dispVector(pop->at(i)->doubleDecisionVec(), "\t", "\n");
            }
        }

        opt->incrementIteration();
    }

    delete opt;
    delete filt;
    delete pnlt;
    delete scal;
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_ParEGO()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 2;
    int   nDirs    = 10;
    int   popSize  = 100;
    int   budget   = popSize+nDirs*1;
    int   maxSurrogateSize = 50;
    double nRaduis  = 0.2;

    PSetBase*      base = new PSetBase();
    IFormulation*  prob = new IFormulation(base);
    RandomInit*    init = new RandomInit(prob);
    Evaluator*     eval = new Evaluator(init);
    ParEGO*        alg  = new ParEGO(eval);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    alg->defineReferenceSetSize(nDirs);
    alg->TP_defineMaxSolutions(maxSurrogateSize);
    alg->TP_defineNeighbourhoodRadius(nRaduis);
    alg->defineBudget(budget);

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        cout << "Iteration: " << alg->currentIteration() <<
                " \tRemaining budget: " << alg->remainingBudget() << endl;
        cout << "\tDirection vector: \t";
        dispVector(alg->dirVec(), "\t", "\n");
        cout << "\tLast solutions added: \t";
        TVector<IMappingSPtr> newMaps = alg->newMappings();
        for(int i=0; i<newMaps.size(); i++) {
            dispVector(newMaps[i]->doubleDecisionVec(), "\t", "\n\t\t\t\t");
        }
        cout << endl;
        cout << "\tNumber of IMappings: " << alg->evaluatedMappings().size() + 1
             << endl;

        alg->incrementIteration();
    }

    delete alg;
    delete eval;
    delete init;
    delete prob;
    delete base;
}


#endif

QTEST_GUILESS_MAIN(tst_parego)

#include "tst_parego.moc"
