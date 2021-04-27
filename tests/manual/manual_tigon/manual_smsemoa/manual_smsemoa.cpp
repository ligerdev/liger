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
#include <iostream>
#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

void SMSEMOA_DTLZ2()
{
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter  = 30000;

    cout.precision(5);
    //srand(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize+1);

    Evaluator* evaluator = new Evaluator(init);

    SMSEMOA* alg = new SMSEMOA(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running SMS-EMOA for " << maxIter << " iterations" << endl;
    cout << " Problem: DTLZ2"  << endl;
    cout << "################################################" << endl;
    cout << endl;

    // Display results
    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        //Display the results:
        if(iter == maxIter-1) {
            cout << "data = " << alg->set(0)
                 << "figure; plot3(data(:,1),data(:,2),data(:,3),'or');"
                 << endl;
        } else if(!(iter%10)){
            cout << iter << "/" << maxIter << endl;
        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void SMSEMOA_DTLZ1()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter  = 30000;

    cout.precision(5);
    //srand(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize+1);

    Evaluator* evaluator = new Evaluator(init);

    SMSEMOA* alg = new SMSEMOA(evaluator);

    cout << endl;
    cout << "################################################" << endl;
    cout << " Running SMS-EMOA for " << maxIter << " iterations" << endl;
    cout << " Problem: DTLZ1"  << endl;
    cout << "################################################" << endl;
    cout << endl;

    // Display results
    for(int iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        //Display the results:
        if(iter == maxIter-1) {
            cout << "data = " << alg->set(0)
                 << "figure; plot3(data(:,1),data(:,2),data(:,3),'or');"
                 << endl;
        } else if(!(iter%10)){
            cout << iter << "/" << maxIter << endl;
        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}


void operators()
{
    int dVecSize = 3;
    int oVecSize = 2;
    int maxIter  = 4;

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);

    // Before alg
    PSetBase*                 base    = new PSetBase();
    IFormulation*             form    = new IFormulation(base);
    UserDefinedInit*           init    = new UserDefinedInit(form);
    Evaluator*             evaluator      = new Evaluator(init);
    form->defineProblem(prob);

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


    // Inside the alg
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
    crossOver->TP_defineSolutionCrossoverProbability(1);
    TruncateSets*            resize    = new TruncateSets(crossOver);
    resize->TP_defineSetSize(1);
    PolynomialMutation*      pm        = new PolynomialMutation(resize);
    pm->TP_defineSolutionMutationProbability(1/dVecSize);

    MergeForNextIteration*   newpop    = new MergeForNextIteration(pm);

    // Run
    for (int i=0; i<maxIter; i++){
        cout << "\n\n############### Iter "<< i << " ###################" << endl;
        newpop->evaluate();

        cout <<"itdata="<< ranking->outputSet(0)
            << "close all; plot(idata(:,1),idata(:,2),'r*'); hold on;"<< endl;
        cout << "################################################" << endl;
        cout<<"otdata="<< reduce->outputSet(0)
           << "plot(odata(:,1),odata(:,2),'bo'); title('reduce I/O') " << endl;
        cout << "################################################" << endl;
        cout <<"idata=" << children->inputSet(0)
            << "figure; plot(idata(:,1),idata(:,2),'r*');hold on;"<< endl;
        cout << "################################################" << endl;
        cout << "odata="<< crossOver->outputSet(0)
             <<"plot(odata(:,1),odata(:,2),'bo');title('tournament I/O');"<<endl;
        cout << "################################################" << endl;
        cout << "odata="<< pm->outputSet(0)
             <<"plot(odata(:,1),odata(:,2),'bo');title('tournament I/O');"<<endl;
        cout << "################################################" << endl;
        cout <<"idata=" << newpop->inputSet(0)
            << "figure; plot(idata(:,1),idata(:,2),'r*');hold on;"<< endl;
        cout << "################################################" << endl;
        cout <<"idata=" << newpop->inputSet(1)
            << "plot(idata(:,1),idata(:,2),'b*');hold on;"<< endl;
        cout << "################################################" << endl;
        cout << "odata="<< newpop->outputSet(0)
             <<"plot(odata(:,1),odata(:,2),'ko');title('merge I/O');"<<endl;

        newpop->incrementIteration();
    }
    delete base;
    delete form;
    delete init;
    delete evaluator;
    delete ranking;
    delete reduce;
    delete children;
    delete crossOver;
    delete resize;
    delete pm;
    delete newpop;
}

void test_SMSEMOA_DTLZ2()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter  = 30000;

    cout.precision(5);
    //srand(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize+1);

    Evaluator* evaluator = new Evaluator(init);

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
    crossOver->TP_defineSolutionCrossoverProbability(1);
    crossOver->TP_defineVariableCrossoverProbability(1);
    TruncateSets*            resize    = new TruncateSets(crossOver);
    resize->TP_defineSetSize(1);
    PolynomialMutation*      pm        = new PolynomialMutation(resize);
    pm->TP_defineVariableMutationProbability(1/dVecSize);
    pm->TP_defineSolutionMutationProbability(0.5);
    MergeForNextIteration*   newpop    = new MergeForNextIteration(pm);


    cout << endl;
    cout << "################################################" << endl;
    cout << " Running SMS-EMOA for " << maxIter << " iterations" << endl;
    cout << " Problem: DTLZ1"  << endl;
    cout << "################################################" << endl;
    cout << endl;

    // Display results
    for(int iter=0;iter<maxIter;iter++) {
        newpop->evaluate();
        newpop->incrementIteration();

        //Display the results:
        if(iter == maxIter-1) {
            cout << "data = " << newpop->set(0)
                 << "figure; plot3(data(:,1),data(:,2),data(:,3),'or');" << endl;
            cout<<ranking->outputSets().size() <<endl;
            cout<<endl;
        } else if(!(iter%10)){
            cout << iter << "/" << maxIter << endl;
        }
    }

    delete base;
    delete prob;
    delete init;
    delete evaluator;
    delete ranking;
    delete children;
    delete crossOver;
    delete resize;
    delete pm;
    delete newpop;
}

int main(int argc, char* argv[])
{
    cout << "Running Benchmarks..." << endl;
    ///Select which test you rant to run and comment the others
    SMSEMOA_DTLZ2();
    //SMSEMOA_DTLZ1();
    //operators();
    //test_SMSEMOA_DTLZ2();
    return 0;
}
