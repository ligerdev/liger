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
#include <tigon/Tigon.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_smsemoa : public QObject
{
    Q_OBJECT

public:

private slots:
    /* Add the necessary operators */
    void test_SMSEMOA_Reduce();
    void test_SMSEMOA_workflow();
    void test_SMSEMOA();

};

void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}


void tst_smsemoa::test_SMSEMOA_Reduce()
{
    /// Define the problem
    int dVecSize = 3;
    int oVecSize = 2;
    int maxIter  = 2;
    int popSize  = 5;

    PSetBase*                 base    = new PSetBase();
    IFormulation*             form    = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    form->defineProblem(prob);

    UserDefinedInit*           init    = new UserDefinedInit(form);
    TVector<TVector<double>> x0={{0.0,((double)rand()/(RAND_MAX)),((double)rand()/(RAND_MAX))},
                                 {0.2,((double)rand()/(RAND_MAX)),((double)rand()/(RAND_MAX))},
                                 {0.4,((double)rand()/(RAND_MAX)),((double)rand()/(RAND_MAX))},
                                 {0.6,((double)rand()/(RAND_MAX)),((double)rand()/(RAND_MAX))},
                                 {0.8,((double)rand()/(RAND_MAX)),((double)rand()/(RAND_MAX))}};

    ISet* solset= new ISet;
    for(int i=0; i<5; i++){

        IMappingSPtr imap(new IMapping(prob));
        imap->defineDecisionVar(0,IElement(x0[i][0]));
        imap->defineDecisionVar(1,IElement(x0[i][1]));
        imap->defineDecisionVar(2,IElement(x0[i][2]));
        solset->append(imap);
    }

    init->appendSolutions(solset);
    Evaluator*       evaluator      = new Evaluator(init);

    NonDominanceRanking*  ranking = new NonDominanceRanking(evaluator);
    ranking->defineOutputTags(TStringList({Tigon::TReducePopulation}));
    SMSEMOAReduce* reduce = new SMSEMOAReduce(ranking);

    // Display results
    for(int i=0; i<maxIter;i++){
        reduce->evaluate();
        reduce->incrementIteration();

        if(i == maxIter-1) {
            TStringList tag;
            tag <<Tigon::TForNextIteration << Tigon::TForSelection;
            ISet* reducepop = reduce->setWithTags(tag);
            QCOMPARE(reducepop->size(), popSize-1);

            cout<<"data="<<reduce->set(0)
                  <<"figure; plot(data(:,1),data(:,2),'k*');hold on"
                  <<endl;
            cout << "odata ="<<reduce->outputSets()
                 <<"plot(odata(:,1),odata(:,2),'or');"<< endl;
        } else if(!(i%10)){
            cout << i << "/" << maxIter << endl;
        }
    }

    delete base;
    delete form;
    delete init;
    delete evaluator;
    delete ranking;
    delete reduce;
}

void tst_smsemoa::test_SMSEMOA_workflow()
{
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 5;

    cout.precision(5);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init     = new RandomInit(prob);
    init->TP_defineSetSize(populationSize+1);

    Evaluator* evaluator = new Evaluator(init);

    /* Algorithm */
    NonDominanceRanking*  ranking = new NonDominanceRanking(evaluator);
    ranking->defineOutputTags(TStringList({Tigon::TReducePopulation}));

    SMSEMOAReduce* reduce = new SMSEMOAReduce(ranking);

    TournamentFiltrationForDirection*
            children    = new TournamentFiltrationForDirection(reduce);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineNumberOfMappings(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addOutputTag(Tigon::TForPerturbation);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForResize);

    SBXCrossOver*            crossOver = new SBXCrossOver(children);
    crossOver->TP_defineSolutionCrossoverProbability(1.0);
    TruncateSets*            resize    = new TruncateSets(crossOver);
    resize->TP_defineSetSize(1);
    PolynomialMutation*      pm        = new PolynomialMutation(resize);
    pm->TP_defineVariableMutationProbability(1/dVecSize);
    MergeForNextIteration*   nxt    = new MergeForNextIteration(pm);

    //Display the results:
    cout << endl;
    cout << "################################################" << endl;
    cout << " Running SMS-EMOA for " << maxIter << " iterations" << endl;
    cout << " Problem: DTLZ2"  << endl;
    cout << "################################################" << endl;
    cout << endl;

    for(int iter=0; iter<maxIter; iter++) {
        nxt->evaluate();
        nxt->incrementIteration();

        if (iter>=0){
            ISet* nxtpop = nxt->set(0);
            QCOMPARE(nxtpop->size(), populationSize+1);
        }

        if(iter == maxIter-1) {

            cout << "data = " << nxt->set(0)
                 << "figure; plot3(data(:,1),data(:,2),data(:,3),'or');" << endl;
            cout<<ranking->outputSets().size() <<endl;
            cout<<endl;
        } else if(!(iter%10)){
            cout << iter << "/" << maxIter << endl;
        }
        cout << endl;
    }

    delete nxt;
    delete pm;
    delete crossOver;
    delete children;
    delete reduce;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_smsemoa::test_SMSEMOA()
{
    int iter;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 5;

    cout.precision(5);
    srand(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize+1);

    Evaluator* evaluator = new Evaluator(init);

    SMSEMOA* alg =new SMSEMOA(evaluator);

    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        // Mixed population
        TStringList tags2;
        tags2 << Tigon::TMainOptimizationSet;
        ISet* mixedPop = alg->setWithTags(tags2);
        QCOMPARE(mixedPop->size(),populationSize+1);

        // Reduce population
        if (iter>=1){
            TStringList tags1;
            tags1  << Tigon::TForNextIteration <<Tigon::TForSelection;
            ISet* ndPop = alg->setWithTags(tags1);
            QCOMPARE(ndPop->size(),populationSize);
        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}


QTEST_GUILESS_MAIN(tst_smsemoa)

#include "tst_smsemoa.moc"
