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
#include <QtTest>
#include <QDebug>

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

#define TST_PAREGO_OPERATORS
#define PAREGO_WORK_FLOW
//#define PLOT_IN_MATLAB

//TESTED_COMPONENT=src/libs/tigon/....
class tst_parego : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_PAREGO_OPERATORS
    void test_ExpectedImprovement_Alpine2Func();
    void test_RandExpectedImprovement_Alpine2Func();
    void test_ExpectedImprovement_Poly1Func();
    void test_FeasibleExpectedImprovement();
    void test_FeasibleRandExpectedImprovement();
    void test_ProbabilityFeasibility();
    void test_ProbabilityFeasibilityModified();
    void test_ConstrainedExpectedImprovement();
    void test_SurrogateBasedOptimizer();
    void test_ConstrainedSurrogateBasedOptimizer();
#endif

#ifdef PAREGO_WORK_FLOW
    void test_ParEGO_workflow();
    void test_ParEGO();
#endif
};

inline void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(size_t i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

inline void writeVector(QTextStream &st, TVector<double> vec, QString sep = "\t", QString endLine = "\n")
{
    for(size_t i=0; i<vec.size(); i++) {
        st << vec[i];
        if(i<vec.size()-1) {
            st << sep;
        }
    }
    st << endLine;
}

#ifdef TST_PAREGO_OPERATORS
void tst_parego::test_ExpectedImprovement_Alpine2Func()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new Alpine2);
    func->TP_defineNInputs(1);
    prob->appendFunction(func);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    for(double i=0.0; i<=10.0; i+=2.3) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    for(double i=0.6; i<10.0; i*=2) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Fit a Kriging model to the evaluated pionts
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    // Construct the expected improvement function
    IFunctionSPtr func2 = IFunctionSPtr(new ExpectedImprovement(km));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);

    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<double> xq;
    ISet* queryPoints = new ISet;
    for(double x=0.0; x<=10.0; x += 0.01) {
        xq.push_back(x);
        IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
        imap->defineDecisionVar(0, IElement(x));
    }
    init2->defineInitialSet(queryPoints);

    eval2->TP_defineSingleObjective(true);
    eval2->evaluate();

    TVector<double> valueEI; valueEI.reserve(xq.size());
    TVector<double> errorEI; errorEI.reserve(xq.size());

    ExpectedImprovement* fExpImpr =
            static_cast<ExpectedImprovement*>(func2.get());
    for(auto sol : queryPoints->all()) {
        // to get the appropriate error always call value() first
        valueEI.push_back(fExpImpr->value(sol->decisionVec()));
        errorEI.push_back(fExpImpr->error(sol->decisionVec()));
    }

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_expectedImprovement.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    // query points
    out << "% Model estimation:" << endl << "xq = [";
    for(auto x : xq) {
        out << x << "; ";
    }
    out << "];" << endl;


    out << "yq = [";
    for(auto value : valueEI) {
        out << value << "; ";
    }
    out << "];" << endl;

    out << "err = [";
    for(auto error: errorEI) {
        out << error << "; ";
    }
    out << "];" << endl << endl;

    ISet* expImp = init2->outputSet(0);
    out << "% Expected improvement:" << endl;
    out << "expImp = [";
    for(auto sol : expImp->all()) {
        double ei = sol->doubleCost();
        out << -ei << "; ";
    }
    out << "];" << endl;

    out << "syms x;\n"
           "y = -sqrt(x).*sin(x);" << endl;

    out << "% Plot the results\n"
           "figure; hold on;\n"
           "yyaxis left;\n"
           "realFunc = fplot(y,[0 10]);\n"
           "TigonReal = plot(xx,yy,'ok','MarkerFaceColor','k');\n"
           "Model = plot(xq, yq, '--r', 'lineWidth', 2);\n"
           "ExpectedImprovement = plot(xq, expImp, '-.g');\n\n"
           "ylabel('$y = -\\sqrt{x}\\sin{x}$','Interpreter','latex','FontSize',24);\n"
           "yyaxis right;\n"
           "Error = plot(xq, err, 'r');\n"
           "ylabel('Estimation error ($\\hat{\\sigma}$)','Interpreter','latex','FontSize',24);\n"
           "legend([realFunc, TigonReal, Model, ExpectedImprovement, Error],...\n"
           "'Real function', 'Sample points', ...\n"
           "'Kriging estimation', 'Expected improvement', "
           "'Estimation error',...\n"
           "'location', 'northWest');\n"
           "xlabel('Decision space','FontSize',24);\n"
           "grid on;\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('ExpectedImprovement_KrigingEstimatedError_Alpine2.png','-dpng');" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_RandExpectedImprovement_Alpine2Func()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new Alpine2);
    func->TP_defineNInputs(1);
    prob->appendFunction(func);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    for(double i=0.0; i<=10.0; i+=2.3) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    for(double i=0.6; i<10.0; i*=2) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Fit a Kriging model to the evaluated pionts
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    // Construct the rand expected improvement function
    IFunctionSPtr func2 = IFunctionSPtr(new RandExpectedImprovement(km));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);
    eval2->TP_defineSingleObjective(true);

    // Create an IFunction from the Kriging model
    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<double> xq;
    ISet* queryPoints = new ISet;
    for(double x=0.0; x<=10.0; x += 0.01) {
        xq.push_back(x);
        IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
        imap->defineDecisionVar(0, IElement(x));
    }
    init2->defineInitialSet(queryPoints);

    eval2->TP_defineSingleObjective(true);
    eval2->evaluate();

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_randExpectedImprovement.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    ISet* expImp = init2->outputSet(0);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    out << "% Model estimation:" << endl << "xq = [";
    for(auto x : xq) {
        out << x << "; ";
    }
    out << "];" << endl;

    RandExpectedImprovement* randExp =
            static_cast<RandExpectedImprovement*>(func2.get());
    out << "yq = [";
    for(auto sol : queryPoints->all()) {
        double yq = randExp->value(sol->decisionVec());
        out << yq << "; ";
    }
    out << "];" << endl;

    out << "err = [";
    for(auto sol : queryPoints->all()) {
        double err = randExp->error(sol->decisionVec());
        out << err << "; ";
    }
    out << "];" << endl << endl;

    double h = randExp->bandwidth();
    TVector<TVector<double>> samples = randExp->samples();
    out << "% Density with bandwidth = " << h << endl << "density = [";
    for(auto sol : queryPoints->all()) {
        double density = gaussianKDE(samples, sol->doubleDecisionVec(), h);
        out << density << "; ";
    }
    out << "];" << endl << endl;


    out << "% Expected improvement:" << endl;
    out << "expImp = [";
    for(auto sol : expImp->all()) {
        double ei = sol->doubleCost();
        out << -ei << "; ";
    }
    out << "];" << endl;

    out << "syms x;\n"
           "y = -sqrt(x).*sin(x);" << endl;

    out << "% Plot the results\n"
           "figure; hold on;\n"
           "yyaxis left;\n"
           "realFunc = fplot(y,[0 10]);\n"
           "TigonReal = plot(xx,yy,'ok','MarkerFaceColor','k');\n"
           "Model = plot(xq, yq, '--r', 'lineWidth', 2);\n"
           "ExpectedImprovement = plot(xq, expImp, '-.g');\n\n"
           "ylabel('$y = -\\sqrt{x}\\sin{x}$','Interpreter','latex','FontSize',24);\n"
           "yyaxis right;\n"
           "Density = plot(xq, density, 'k');"
           "UnscaledError = plot(xq, atan(1./density).*(2/pi), 'y');"
           "Error = plot(xq, err, 'r');\n"
           "ylabel('Estimation error ($\\hat{\\sigma}$)','Interpreter','latex','FontSize',24);\n"
           "legend([realFunc, TigonReal, Model, ExpectedImprovement, Density, UnscaledError, Error],...\n"
           "'Real function', 'Sample points', ...\n"
           "'Kriging estimation', 'Expected improvement', 'Density with bandwidth: " << h << " '," << " ...\n"
           "'Unscaled estimation error', 'Estimation error', ...\n"
           "'location', 'northWest');\n"
           "xlabel('Decision space','FontSize',24);\n"
           "grid on;\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('RandExpectedImprovement_KernelDensityBasedError_Alpine2.png','-dpng');" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_ExpectedImprovement_Poly1Func()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new Poly1);
    func->TP_defineNInputs(1);
    prob->appendFunction(func);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // points
    TVector<double> training_points;
    training_points << 0.0 << 0.15 << 0.24 << 0.31 << 0.33 << 0.36 << 0.5 << 1.0;

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point));
    }

    init->defineInitialSet(providedPoints);
    eval->evaluate();

    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Fit a Kriging model to the evaluated pionts
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    // Construct the expected improvement function
    IFunctionSPtr func2 = IFunctionSPtr(new ExpectedImprovement(km));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);

    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<double> xq;
    ISet* queryPoints = new ISet;
    for(double x=0.0; x<=(1.0+Tigon::Epsilon); x += 0.001) {
        xq.push_back(x);
        IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
        imap->defineDecisionVar(0, IElement(x));
    }
    init2->defineInitialSet(queryPoints);

    eval2->TP_defineSingleObjective(true);
    eval2->evaluate();

    TVector<double> valueEI; valueEI.reserve(xq.size());
    TVector<double> errorEI; errorEI.reserve(xq.size());

    ExpectedImprovement* fExpImpr =
            static_cast<ExpectedImprovement*>(func2.get());
    for(auto sol : queryPoints->all()) {
        // to get the appropriate error always call value() first
        valueEI.push_back(fExpImpr->value(sol->decisionVec()));
        errorEI.push_back(fExpImpr->error(sol->decisionVec()));
    }

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_expectedImprovement_Poly1.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    out << "% Model estimation:" << endl << "xq = [";
    for(auto x : xq) {
        out << x << "; ";
    }
    out << "];" << endl;

    out << "yq = [";
    for(auto value : valueEI) {
        out << value << "; ";
    }
    out << "];" << endl;

    out << "err = [";
    for(auto error: errorEI) {
        out << error << "; ";
    }
    out << "];" << endl << endl;

    ISet* expImp = init2->outputSet(0);
    out << "% Expected improvement:" << endl;
    out << "expImp = [";
    for(auto sol : expImp->all()) {
        double ei = sol->doubleCost();
        out << -ei << "; ";
    }
    out << "];" << endl;

    out << "syms x;\n"
           "y = (6*x-2)^2 * sin(12*x-4);" << endl;

    out << "% Plot the results\n"
           "figure; hold on;\n"
           "yyaxis left;\n"
           "realFunc = fplot(y,[0 1]);\n"
           "TigonReal = plot(xx,yy,'ok','MarkerFaceColor','k');\n"
           "Model = plot(xq, yq, '--r', 'lineWidth', 2);\n"
           "ExpectedImprovement = plot(xq, expImp, '-.g');\n\n"
           "ylabel('$y = (6x-2)^2 \\sin(12x-4)$','Interpreter','latex','FontSize',24);\n"
           "yyaxis right;\n"
           "Error = plot(xq, err), 'r');\n"
           "ylabel('Estimation error ($\\hat{\\sigma}$)','Interpreter','latex','FontSize',24);\n"
           "ylim([0 4]);\n"
           "legend([realFunc, TigonReal, Model, ExpectedImprovement, Error],...\n"
           "'Real function', 'Sample points', ...\n"
           "'Kriging estimation', 'Expected improvement', "
           "'Estimation error',...\n"
           "'location', 'northWest');\n"
           "xlabel('Decision space','FontSize',24);\n"
           "grid on;\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('ExpectedImprovement_KrigingEstimatedError_Poly1.png','-dpng');" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_FeasibleExpectedImprovement()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new CBraninM1);
    ProblemSPtr problem(new Problem);
    problem->appendFunction(func, TVector<int>(), TVector<int>({1}));
    problem->processProblemDefinition();

    TVector<double> threshVec({0.0});
    TVector<ElementType> types({RealType});
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    problem->defineThresholdVector(thVec);
    problem->processProblemDefinition();

    prob->defineProblem(problem);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // Generate 6 points
    TVector<TVector<double>> training_points;
    training_points << TVector<double>({0.25, 0.24}); // infeasible
    training_points << TVector<double>({0.08, 0.58}); // infeasible
    training_points << TVector<double>({0.75, 0.06}); // infeasible
    training_points << TVector<double>({0.42, 0.92}); // feasible
    training_points << TVector<double>({0.58, 0.42}); // feasible
    training_points << TVector<double>({0.91, 0.75}); // feasible

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point[0]));
        imap->defineDecisionVar(1, IElement(point[1]));
    }

    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Fit a Kriging model to the evaluated points
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    // Calculate constraint violation of the evaluated points
    TVector<double> cViolation;
    cViolation.reserve(eSet->size());
    for(auto sol : eSet->all()) {
         double violation = solutionConstraintViolation(sol, threshVec);
         cViolation.push_back(violation);
    }

    // Create an IFunction from the Kriging model
    IFunctionSPtr func2 = IFunctionSPtr(new ExpectedImprovement(km));
    IFunctionSPtr func3 = IFunctionSPtr(new FeasibleExpectedImprovement(km, cViolation));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);

    prob2->appendFunction(func2);
    prob2->appendFunction(func3);
    prob2->evaluate();

    // Create query points
    TVector<TVector<double>> xq;
    ISet* queryPoints = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xq.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

    TVector<double> valueFEI; valueFEI.reserve(xq.size());
    TVector<double> errorFEI; errorFEI.reserve(xq.size());

    FeasibleExpectedImprovement* fExpImp =
            static_cast<FeasibleExpectedImprovement*>(func3.get());
    for(auto sol : queryPoints->all()) {
        // to get the appropriate error always call value() first
        valueFEI.push_back(fExpImp->value(sol->decisionVec()));
        errorFEI.push_back(fExpImp->error(sol->decisionVec()));
    }

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_feasibleExpectedImprovement_CBraninM1.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    ISet* expImp = init2->outputSet(0);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    // query points
    out << "% Model estimation:" << endl << "xq = [";
    for(auto xvec : xq) {
        out << "[";
        for(auto x : xvec) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    out << "yq = [";
    for(auto value : valueFEI) {
        out << value << "; ";
    }
    out << "];" << endl;

    out << "err = [";
    for(auto error: errorFEI) {
        out << error << "; ";
    }
    out << "];" << endl << endl;

    out << "% Expected Improvement:" << endl;
    out << "expImp = [";
    for(auto sol : expImp->all()) {
        double ei = sol->doubleObjectiveVar(0);
        out << -ei << "; ";
    }
    out << "];" << endl;

    out << "% Feasible Expected Improvement:" << endl;
    out << "fExpImp = [";
    for(auto sol : expImp->all()) {
        double fei = sol->doubleObjectiveVar(1);
        out << -fei << "; ";
    }
    out << "];" << endl;

    out << "\n% Plot the results\n"
           "figure(1);\n"
           "ModelX = reshape(xq(:,1),[101, 101]);\n"
           "ModelY = reshape(xq(:,2),[101, 101]);\n"
           "ModelBranin = reshape(yq,[101, 101]);\n"
           "BraninFuncContour = contourf(ModelX,ModelY,ModelBranin,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Kriging prediction of the Branin function');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('KrigingModelPrediction_CBraninM1.png','-dpng');"
           "\n"
           "figure(2);\n"
           "ModelError = reshape(err,[101, 101]);\n"
           "BraninFuncContour = contourf(ModelX,ModelY,ModelError,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Kriging prediction error of the Branin function');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('KrigingModelError_CBraninM1.png','-dpng');"
           "\n"
           "figure(3);\n"
           "ExpImp = reshape(expImp,[101, 101]);\n"
           "ExpectedImprovement = contourf(ModelX,ModelY,ExpImp,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Expected Improvement');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('ExpImp_CBraninM1.png','-dpng');"
           "\n"
           "figure(4);\n"
           "FExpImp = reshape(fExpImp,[101, 101]);\n"
           "FExpectedImprovement = contourf(ModelX,ModelY,FExpImp,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Feasible Expected Improvement');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('FExpImp_CBraninM1.png','-dpng');"
           "\n" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_FeasibleRandExpectedImprovement()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new CBraninM1);
    ProblemSPtr problem(new Problem);
    problem->appendFunction(func, TVector<int>(), TVector<int>({1}));
    problem->processProblemDefinition();

    TVector<double> threshVec({0.0});
    TVector<ElementType> types({RealType});
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    problem->defineThresholdVector(thVec);
    problem->processProblemDefinition();

    prob->defineProblem(problem);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // Generate 6 points
    TVector<TVector<double>> training_points;
    training_points << TVector<double>({0.25, 0.24}); // infeasible
    training_points << TVector<double>({0.08, 0.58}); // infeasible
    training_points << TVector<double>({0.75, 0.06}); // infeasible
    training_points << TVector<double>({0.42, 0.92}); // feasible
    training_points << TVector<double>({0.58, 0.42}); // feasible
    training_points << TVector<double>({0.91, 0.75}); // feasible

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point[0]));
        imap->defineDecisionVar(1, IElement(point[1]));
    }

    init->defineInitialSet(providedPoints);

    eval->evaluate();

    // Fit a Kriging model to the evaluated points
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Calculate constraint violation of every evaluated solution
    TVector<double> cViolation;
    cViolation.reserve(eSet->size());
    for(auto sol : eSet->all()) {
         double violation = solutionConstraintViolation(sol, threshVec);
         cViolation.push_back(violation);
    }

    // Expected improvement results
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);
    eval2->TP_defineSingleObjective(true);

    // Create an IFunction from the Kriging model
    IFunctionSPtr func2 =
            IFunctionSPtr(new FeasibleRandExpectedImprovement(km, xx, cViolation));

    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<TVector<double>> xq;
    ISet* queryPoints = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xq.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

    TVector<double> errorFEI; errorFEI.reserve(xq.size());
    FeasibleRandExpectedImprovement* fExpImp =
            static_cast<FeasibleRandExpectedImprovement*>(func2.get());
    for(auto sol : queryPoints->all()) {
        // to get the appropriate error always call value() first
        fExpImp->value(sol->decisionVec());
        errorFEI.push_back(fExpImp->error(sol->decisionVec()));
    }

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_feasibleRandExpectedImprovement.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    ISet* expImp = init2->outputSet(0);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    // query points
    out << "% Model estimation:" << endl << "xq = [";
    for(auto xvec : xq) {
        out << "[";
        for(auto x : xvec) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    out << "err = [";
    for(auto error: errorFEI) {
        out << error << "; ";
    }
    out << "];" << endl << endl;

    out << "% Feasible Rand Expected Improvement:" << endl;
    out << "fExpImp = [";
    for(auto sol : expImp->all()) {
        double fei = sol->doubleObjectiveVar(0);
        out << -fei << "; ";
    }
    out << "];" << endl;

    out << "\n% Plot the results\n"
           "ModelX = reshape(xq(:,1),[101, 101]);\n"
           "ModelY = reshape(xq(:,2),[101, 101]);\n"
           "\n"
           "figure(1);\n"
           "ModelError = reshape(err,[101, 101]);\n"
           "BraninFuncContour = contourf(ModelX,ModelY,ModelError,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Density based prediction error of the Branin function');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('FRandExpImpError_CBraninM1.png','-dpng');"
           "\n"
           "figure(2);\n"
           "FExpImp = reshape(fExpImp,[101, 101]);\n"
           "FExpectedImprovement = contourf(ModelX,ModelY,FExpImp,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Feasible Rand Expected Improvement');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('FRandExpImp_CBraninM1.png','-dpng');"
           "\n" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_ProbabilityFeasibility()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new CBraninM1);
    ProblemSPtr problem(new Problem);
    problem->appendFunction(func, TVector<int>(), TVector<int>({1}));
    problem->processProblemDefinition();

    TVector<double> threshVec({0.0});
    TVector<ElementType> types({RealType});
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    problem->defineThresholdVector(thVec);
    problem->processProblemDefinition();

    prob->defineProblem(problem);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // Generate 6 points
    TVector<TVector<double>> training_points;
    training_points << TVector<double>({0.25, 0.24}); // infeasible
    training_points << TVector<double>({0.08, 0.58}); // infeasible
    training_points << TVector<double>({0.75, 0.06}); // infeasible
    training_points << TVector<double>({0.42, 0.92}); // feasible
    training_points << TVector<double>({0.58, 0.42}); // feasible
    training_points << TVector<double>({0.91, 0.75}); // feasible

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point[0]));
        imap->defineDecisionVar(1, IElement(point[1]));
    }

    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement and probability of feasibility

    // Fit a Kriging model to the constraint violation values of the evaluated solutions
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    xx.reserve(eSet->size());
    TVector<double> cy;
    cy.reserve(eSet->size());
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // constraint violation value
        cy.push_back(sol->doubleConstraintVar(0) - threshVec[0]);
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, cy);
    KrigingSPtr kmC = KrigingSPtr(kinterp);
//    PowerVariogram* powerVC = new PowerVariogram(xx, cy);
//    KrigingSPtr kmC = KrigingSPtr(new OrdinaryKriging(powerVC));
    kmC->defineEstimateErrors(true);

    // Construct the feasible expected improvement and probability of feasibility functions
    IFunctionSPtr func2 = IFunctionSPtr(new ProbabilityFeasibility(kmC));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);
    eval2->TP_defineSingleObjective(true);

    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<TVector<double>> xq;
    ISet* queryPoints = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xq.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_probabilityOfFeasibility.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    // query points
    out << "% Model estimation:" << endl << "xq = [";
    for(auto xvec : xq) {
        out << "[";
        for(auto x : xvec) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    ISet* expImp = init2->outputSet(0);
    out << "% Probability of feasibility:" << endl;
    out << "pf = [";
    for(auto sol : expImp->all()) {
        double fei = sol->doubleObjectiveVar(0);
        out << -fei << "; ";
    }
    out << "];" << endl;

    out << "\n% Plot the results\n"
           "ModelX = reshape(xq(:,1),[101, 101]);\n"
           "ModelY = reshape(xq(:,2),[101, 101]);\n"
           "figure;\n"
           "PF = reshape(pf,[101, 101]);\n"
           "ProbabilityFeasibility = contourf(ModelX,ModelY,PF,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Probability of Feasibility');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('ProbabilityFeasibility_CBraninM1.png','-dpng');"
           "\n" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_ProbabilityFeasibilityModified()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new CBraninM1);
    ProblemSPtr problem(new Problem);
    problem->appendFunction(func, TVector<int>(), TVector<int>({1}));
    problem->processProblemDefinition();

    TVector<double> threshVec({0.0});
    TVector<ElementType> types({RealType});
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    problem->defineThresholdVector(thVec);
    problem->processProblemDefinition();

    prob->defineProblem(problem);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // Generate 6 points
    TVector<TVector<double>> training_points;
    training_points << TVector<double>({0.25, 0.24}); // infeasible
    training_points << TVector<double>({0.08, 0.58}); // infeasible
    training_points << TVector<double>({0.75, 0.7});  // feasible
    training_points << TVector<double>({0.42, 0.92}); // feasible
    training_points << TVector<double>({0.58, 0.42}); // feasible
    training_points << TVector<double>({0.91, 0.75}); // feasible

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point[0]));
        imap->defineDecisionVar(1, IElement(point[1]));
    }

    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement and the probability of feasibility
    /// modified version of Bagheri's et al. (2017)

    // Fit a Kriging model to the constraint violation values of the evaluated solutions
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    xx.reserve(eSet->size());
    TVector<double> cy;
    cy.reserve(eSet->size());
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // constraint violation value
        cy.push_back(sol->doubleConstraintVar(0) - threshVec[0]);
    }

    DACE* kinterpm = new DACE();
    kinterpm->updateData(xx, cy);
    KrigingSPtr kmCm = KrigingSPtr(kinterpm);
//    PowerVariogram* powerVCm = new PowerVariogram(xx, cy);
//    KrigingSPtr kmCm = KrigingSPtr(new Kriging(powerVCm));
    kmCm->defineEstimateErrors(true);

    // Construct the feasible expected improvement and modified probability of feasibility
    IFunctionSPtr func2 = IFunctionSPtr(new ProbabilityFeasibilityBagheri(kmCm));

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);
    eval2->TP_defineSingleObjective(true);

    prob2->appendFunction(func2);
    prob2->evaluate();

    // Create query points
    TVector<TVector<double>> xqm;
    ISet* queryPoints = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xqm.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement and the probability of feasibility
    /// function to compare the results with the modfied version

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, cy);
    KrigingSPtr kmC = KrigingSPtr(kinterp);
//    PowerVariogram* powerVC = new PowerVariogram(xx, cy);
//    KrigingSPtr kmC = KrigingSPtr(new Kriging(powerVC));
    kmC->defineEstimateErrors(true);

    // Construct the feasible expected improvement and probability of feasibility functions
    IFunctionSPtr func3 = IFunctionSPtr(new ProbabilityFeasibility(kmC));

    // Create the new IPSet
    PSetBase*                 base3 = new PSetBase();
    IFormulation*             prob3 = new IFormulation(base3);
    UserDefinedInit*          init3 = new UserDefinedInit(prob3);
    Evaluator*                eval3 = new Evaluator(init3);
    eval3->TP_defineSingleObjective(true);

    prob3->appendFunction(func3);
    prob3->evaluate();

    // Create query points
    TVector<TVector<double>> xq;
    ISet* queryPoints2 = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xq.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob3->createOptimizationMapping(queryPoints2);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init3->defineInitialSet(queryPoints2);

    eval3->evaluate();


    // write the results to an .m file:
    QFile file("tst_probabilityOfFeasibilityModified.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    ISet* pfm = init2->outputSet(0);
    ISet* pf  = init3->outputSet(0);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    // query points
    out << "% Model estimation for modified pf:" << endl << "xq_m = [";
    for(auto xvecm : xqm) {
        out << "[";
        for(auto x : xvecm) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    out << "% Model estimation for pf:" << endl << "xq = [";
    for(auto xvec : xq) {
        out << "[";
        for(auto x : xvec) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    out << "% Probability of feasibility modified:" << endl;
    out << "pf_m = [";
    for(auto sol : pfm->all()) {
        double feim = sol->doubleObjectiveVar(0);
        out << -feim << "; ";
    }
    out << "];" << endl;

    out << "% Probability of feasibility:" << endl;
    out << "pf = [";
    for(auto sol : pf->all()) {
        double fei = sol->doubleObjectiveVar(0);
        out << -fei << "; ";
    }
    out << "];" << endl;

    out << "\n% Plot the results\n"
           "ModelX = reshape(xq(:,1),[101, 101]);\n"
           "ModelY = reshape(xq(:,2),[101, 101]);\n"
           "ModelX_m = reshape(xq_m(:,1),[101, 101]);\n"
           "ModelY_m = reshape(xq_m(:,2),[101, 101]);\n"
           "figure;\n"
           "subplot(1,2,1)\n"
           "PF = reshape(pf,[101, 101]);\n"
           "ProbabilityFeasibility = contourf(ModelX,ModelY,PF,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Probability of Feasibility');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "subplot(1,2,2)\n"
           "PF_m = reshape(pf_m,[101, 101]);\n"
           "ProbabilityFeasibilityModified = contourf(ModelX_m,ModelY_m,PF_m,30);\n"
           "hold on;\n"
           "TrainingPoints_m = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc_m = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc_m.LineWidth=3.0;\n"
           "colorbar;xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Probability of Feasibility Modified');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('ProbabilityFeasibilityModified_CBraninM1.png','-dpng');"
           "\n" << endl;
    file.close();

    delete eval3;
    delete init3;
    delete prob3;
    delete base3;
    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_ConstrainedExpectedImprovement()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new CBraninM1);
    ProblemSPtr problem(new Problem);
    problem->appendFunction(func, TVector<int>(), TVector<int>({1}));
    problem->processProblemDefinition();

    TVector<double> threshVec({0.0});
    TVector<ElementType> types({RealType});
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    problem->defineThresholdVector(thVec);
    problem->processProblemDefinition();

    prob->defineProblem(problem);
    prob->evaluate();

    ISet* providedPoints = new ISet;
    // Generate 6 points
    TVector<TVector<double>> training_points;
    training_points << TVector<double>({0.25, 0.24}); // infeasible
    training_points << TVector<double>({0.08, 0.58}); // infeasible
    training_points << TVector<double>({0.75, 0.06}); // infeasible
    training_points << TVector<double>({0.42, 0.92}); // feasible
    training_points << TVector<double>({0.58, 0.42}); // feasible
    training_points << TVector<double>({0.91, 0.75}); // feasible

    for(auto point : training_points) {
        IMappingSPtr imap = prob->createOptimizationMapping(providedPoints);
        imap->defineDecisionVar(0, IElement(point[0]));
        imap->defineDecisionVar(1, IElement(point[1]));
    }

    init->defineInitialSet(providedPoints);

    eval->evaluate();


    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement and probability of feasibility

    // Fit a Kriging model to the evaluated points
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(auto sol : eSet->all()) {
        // decision vector
        xx.push_back(sol->doubleDecisionVec());
        // function value
        yy.push_back(sol->doubleCost());
    }

    DACE* kinterp = new DACE();
    kinterp->updateData(xx, yy);
    KrigingSPtr km = KrigingSPtr(kinterp);
//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    // Calculate constraint violation of every evaluated solution
    TVector<double> cViolation;
    cViolation.reserve(eSet->size());
    for(auto sol : eSet->all()) {
         double violation = solutionConstraintViolation(sol, threshVec);
         cViolation.push_back(violation);
    }

    // Construct the Kriging model for the constraint
    TVector<double> cy;
    cy.reserve(eSet->size());
    for(auto sol : eSet->all()) {
        double constr = sol->doubleConstraintVar(0) - threshVec[0];
        cy.push_back(constr);
    }

    DACE* kinterp2 = new DACE();
    kinterp2->updateData(xx, cy);
    KrigingSPtr kmC = KrigingSPtr(kinterp2);
//    PowerVariogram* powerVC = new PowerVariogram(xx, cy);
//    KrigingSPtr kmC = KrigingSPtr(new OrdinaryKriging(powerVC));
    kmC->defineEstimateErrors(true);

    // Construct the feasible expected improvement and probability of feasibility functions

    FeasibleExpectedImprovement* fei = new FeasibleExpectedImprovement(km, cViolation);
    FeasibleRandExpectedImprovement* frei = new FeasibleRandExpectedImprovement(km, xx, cViolation);
    TVector<ProbabilityFeasibility*> vfpf({new ProbabilityFeasibility(kmC)});
    IFunctionSPtr func2 =
            IFunctionSPtr(new ConstrainedExpectedImprovement(fei, vfpf));
    IFunctionSPtr func3 =
            IFunctionSPtr(new ConstrainedExpectedImprovement(frei, vfpf));

    // Change the name of the outputs of func3
    ElementPropertiesFactory* elemFact = ElementPropertiesFactory::instance();
    TString name1 = "RandFEIxPF"; TString description1 = "RandFEIxPF";
    TString name2 = "RandFEI"; TString description2 = "RandFEI";
    TString name3 = "PF2"; TString description3 = "PF2";
    ElementProperties elem1 = elemFact->createElementProperties(0, name1, description1, RealType, Maximization);
    ElementProperties elem2 = elemFact->createElementProperties(0, name2, description2, RealType, Maximization);
    ElementProperties elem3 = elemFact->createElementProperties(0, name3, description3, RealType, Maximization);
    TVector<ElementProperties> funcProp = func3->outputPrpts();
    funcProp[0] = elem1;
    funcProp[1] = elem2;
    funcProp[2] = elem3;
    func3->defineOutputPrpts(funcProp);

    // Create the new IPSet
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);

    ProblemSPtr problem2(new Problem);
    problem2->appendFunction(func2, TVector<int>(), TVector<int>(), TVector<int>{0});
    problem2->appendFunction(func3, TVector<int>(), TVector<int>(), TVector<int>{0});
    problem2->processProblemDefinition();

    prob2->defineProblem(problem2);
    prob2->evaluate();

    // Create query points
    TVector<TVector<double>> xq;
    ISet* queryPoints = new ISet;
    for(double x1=0.0; x1<=(1.0+Tigon::Epsilon); x1 += 0.01) {
        for(double x2=0.0; x2<=(1.0+Tigon::Epsilon); x2 += 0.01) {
            xq.push_back(TVector<double>({x1, x2}));
            IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
            imap->defineDecisionVar(0, IElement(x1));
            imap->defineDecisionVar(1, IElement(x2));
            imap->defineEvaluated(false);
        }
    }
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

#ifdef PLOT_IN_MATLAB
    // write the results to an .m file:
    QFile file("tst_constrainedExpectedImprovement.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "% Evaluated points:" << endl;

    out << "xx = [";
    for(auto x : xx) {
        writeVector(out, x," ","; ");
    }
    out << "];" << endl;

    out << "yy = [";
    for(auto y : yy) {
        out << y << "; ";
    }
    out << "];" << endl<< endl;

    // query points
    out << "% Model estimation:" << endl << "xq = [";
    for(auto xvec : xq) {
        out << "[";
        for(auto x : xvec) {
            out << x << " ";
        }
        out << "]; ";
    }
    out << "];" << endl;

    ISet* expImp = init2->outputSet(0);
    out << "% Constrained Expected Improvement:" << endl;
    out << "cExpImp = [";
    for(auto sol : expImp->all()) {
        double val = sol->doubleObjectiveVar(0);
        out << -val << "; ";
    }
    out << "];" << endl;

    out << "% Constrained Rand Expected Improvement:" << endl;
    out << "cRandExpImp = [";
    for(auto sol : expImp->all()) {
        double val = sol->doubleObjectiveVar(1);
        out << -val << "; ";
    }
    out << "];" << endl;

    out << "% Feasible Expected Improvement:" << endl;
    out << "fExpImp = [";
    for(auto sol : expImp->all()) {
        double val = sol->doubleUnusedVar(0);
        out << val << "; ";
    }
    out << "];" << endl;

    out << "% Probability of Improvement:" << endl;
    out << "probImp = [";
    for(auto sol : expImp->all()) {
        double val = sol->doubleUnusedVar(1);
        out << val << "; ";
    }
    out << "];" << endl;

    out << "% Feasible Rand Expected Improvement:" << endl;
    out << "fRandExpImp = [";
    for(auto sol : expImp->all()) {
        double val = sol->doubleUnusedVar(2);
        out << val << "; ";
    }
    out << "];" << endl;

    out << "\n% Plot the results\n"
           "ModelX = reshape(xq(:,1),[101, 101]);\n"
           "ModelY = reshape(xq(:,2),[101, 101]);\n"
           "figure(1);\n"
           "CExpImp = reshape(cExpImp,[101, 101]);\n"
           "CExpectedImprovement = contourf(ModelX,ModelY,CExpImp,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Constrained Expected Improvement');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('CExpImp_CBraninM1.png','-dpng');"
           "\n"
           "figure(2);\n"
           "CRandExpImp = reshape(cRandExpImp,[101, 101]);\n"
           "CRandExpectedImprovement = contourf(ModelX,ModelY,CRandExpImp,30);\n"
           "hold on;\n"
           "TrainingPoints = plot(xx(:,1),xx(:,2),'ok');\n"
           "ConstraintFunc = fimplicit(@(x1,x2) x1*x2 - 0.2, [0 1 0 1],'-k');\n"
           "ConstraintFunc.LineWidth=3.0;\n"
           "colorbar;\n"
           "xlabel('x_1'); ylabel('x_2');\n"
           "title('Contours of the Constrained Rand Expected Improvement');\n"
           "x0=10; y0=10; width=800; height=600;\n"
           "set(gcf,'position',[x0,y0,width,height]);\n"
           "print('CRandExpImp_CBraninM1.png','-dpng');"
           "\n" << endl;
    file.close();
#endif

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_parego::test_SurrogateBasedOptimizer()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 2;
    int   nDirs    = 10;
    int   popSize  = 100;
    int   maxSurrogateSize = 50;

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
    dirs->TP_defineReferenceSetSize(nDirs);
    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    filt->TP_defineMaxSolutions(maxSurrogateSize);
    opt->TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);
    opt->TP_defineOptimizationSearchQuality(0);

    eval->evaluate();
    ISet* oSet = eval->outputSet(0);
    QCOMPARE(oSet->size(), popSize);

    /// checks that SurrogateBasedOptimizer adds a solution to the main
    /// optimization set
    opt->evaluate();
    QCOMPARE(oSet->size(), popSize+1);
}

void tst_parego::test_ConstrainedSurrogateBasedOptimizer()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int popSize = 100;

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

    PSetBase*     base = new PSetBase();
    IFormulation* form = new IFormulation(base);
    form->defineProblem(prob);

    LHSInit*      init = new LHSInit(form);
    init->TP_defineSetSize(popSize);

    Evaluator*    eval = new Evaluator(init);

    /// ParEGO starts
    SimplexLatticeDirectionIterator* dirs =
                                       new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(scal);
    ConstrainedSurrogateBasedOptimizer* opt =
            new ConstrainedSurrogateBasedOptimizer(filt);

    opt->TP_defineOptimizationSearchQuality(0);
    eval->evaluate();
    ISet* oSet = eval->outputSet(0);
    QCOMPARE(oSet->size(), popSize);

    /// checks that ConstrainedSurrogateBasedOptimizer adds a solution to the main
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
    int   nDirs    = 5;
    int   popSize  = 100;
    int   budget   = popSize + nDirs * 1;
    int   maxSurrogateSize = 50;

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
    ConstrainedSurrogateBasedOptimizer*  opt =
            new ConstrainedSurrogateBasedOptimizer(filt);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    eval->defineBudget(budget);
    dirs->TP_defineReferenceSetSize(nDirs);
    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    pnlt->defineOutputTags(TStringList({TString("DO_NO_OPERATE")}));
    filt->TP_defineMaxSolutions(maxSurrogateSize);

    opt->TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);
    opt->TP_defineDisableConstraintHandling(true);
    opt->TP_defineOptimizationSearchQuality(0);

    while(opt->remainingBudget() > 0) {
        opt->evaluate();
        ISet* pop = init->outputSet(0);

        cout << "Iteration: " << opt->currentIteration() <<
                " \tRemaining budget: " << opt->remainingBudget() << endl;
        cout << "\tDirection vector: \t";
        dispVector(opt->dirVec(), "\t", "\n");
        cout << "\tLast solutions added: \n";
        for(auto sol : pop->all()) {
            if(!sol->isEvaluated()) {
                dispVector(sol->doubleDecisionVec(), "\t", "\n");
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
    int   nDirs    = 5;
    int   popSize  = 100;
    int   budget   = popSize + nDirs * 1;
    int   maxSurrogateSize = 50;

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
    alg->defineBudget(budget);
    alg->defineOptimizationSearchQuality(0);
    alg->TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);
    alg->TP_defineConstraintHandlingMethod(Tigon::NoConstraintHandling);

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        cout << "Iteration: " << alg->currentIteration() <<
                " \tRemaining budget: " << alg->remainingBudget() << endl;
        cout << "\tDirection vector: \t";
        dispVector(alg->dirVec(), "\t", "\n");
        cout << "\tLast solutions added: \t";
        TVector<IMappingSPtr> newMaps = alg->newMappings();
        for(auto sol : newMaps) {
            dispVector(sol->doubleDecisionVec(), "\t", "\n\t\t\t\t");
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
