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
#include "ui_qopprobformulationnodeform.h"
#include <qtigon/qtigonconstants.h>
#include <qtigon/dialogs/qopprobformulationnodeform.h>
#include <qtigon/dialogs/functionloaderdialog.h>
#include <qtigon/dialogs/inputoutputcheckingform.h>
#include <qtigon/dialogs/inputpropertiesdialog.h>
#include <qtigon/dialogs/outputpropertiesdialog.h>
#include <qtigon/operators/problemgenerator/inputoutputprivatedata.h>

#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Engine/TigonFunctionFactory.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/JsonUtils.h>

#include <json/json.h>

#include <QDebug>
#include <QCursor>
#include <QFont>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QListWidgetItem>

#include <algorithm>
using namespace std;

using namespace QTigon;
using namespace Tigon;
using Tigon::TigonFunctionFactory;
using namespace Tigon::Representation;

QString toQString(const Tigon::TString& tstr)
{
    return QString::fromStdString(tstr);
}

inline bool operator == (const QString &lhs, const TString &rhs)
{
    return lhs == QString::fromStdString(rhs);
}

inline bool operator == (const TString &lhs, const QString &rhs)
{
    return rhs == QString::fromStdString(lhs);
}

QOpProbFormulationNodeForm::QOpProbFormulationNodeForm(QWidget *parent) :
    QMainWindow(parent),
    m_highlightColor(Qt::lightGray),
    m_defaultColor(Qt::white),
    ui(new Ui::QOpProbFormulationNodeForm),
    m_inputoutputCheckingForm(new InputOutputCheckingForm),
    m_inputPropertiesDialog(new InputPropertiesDialog),
    m_outputPropertiesDialog(new OutputPropertiesDialog),
    m_pg(0),
    m_prob(ProblemSPtr(new Problem)),
    m_problemChanged(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("Problem Formulation"));

    connect(m_inputoutputCheckingForm, &InputOutputCheckingForm::duplicationHandled,
            this, &QOpProbFormulationNodeForm::inputOutputReceived);
    connect(m_inputPropertiesDialog, &InputPropertiesDialog::updateData,
            this, &QOpProbFormulationNodeForm::updateInputData);
    connect(m_outputPropertiesDialog, &OutputPropertiesDialog::updateData,
            this, &QOpProbFormulationNodeForm::updateOutputData);

    createDockWidgets();
}

QOpProbFormulationNodeForm::~QOpProbFormulationNodeForm()
{
    delete ui;
    delete m_inputoutputCheckingForm;
    delete m_inputPropertiesDialog;
    delete m_outputPropertiesDialog;
}

void QOpProbFormulationNodeForm::setup(Tigon::Operators::ProblemGenerator *thisOp)
{
    if(thisOp) {
        m_pg = thisOp;
        m_prob = thisOp->problem();
        loadProblemToViews();
    }
}

void QOpProbFormulationNodeForm::setInputDisabled(bool b)
{
    ui->loadButton->setDisabled(b);
    ui->savButton->setDisabled(b);
    ui->removeButton->setDisabled(b);
    ui->menuFile->setDisabled(b);
    m_inputPropertiesDialog->setReadOnly(b);
    m_outputPropertiesDialog->setReadOnly(b);
}

void QOpProbFormulationNodeForm::closeEvent(QCloseEvent *)
{
    if(m_problemChanged) {
        QMessageBox msgBox;
        msgBox.setText("Problem formulation has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
        case QMessageBox::Save:
            qDebug() << "Save changes";
            m_prob->processProblemDefinition();
            m_pg->defineProblem(m_prob);
            emit opDataChanged();
            break;
        case QMessageBox::Discard:
            qDebug() << "Discard changes";
            m_prob = m_pg->problem(); // reset problem to a clone of the problem
            break;
        default:
            // should never be reached
            break;
        }
    }
    m_problemChanged = false;
}

void QOpProbFormulationNodeForm::loadProblemToViews()
{
    qDebug() << "\n=== Load Problem to View ===\n";
    clearContent();

    TVector<UncertaintyMapping*> dVecUncertainties = cloneDVecUncertaintyMapping();
    TVector<TVector<UncertaintyMapping*> > funcUncertainties = cloneFuncOutUncertainties();
    TVector<bool> isExternals = m_prob->isExternalParameters();

    //! [0] Load decision variables
    TVector<ElementProperties> dprts = m_prob->dPrpts();
    int nVar = dprts.size();
    for(int i=0; i<nVar; i++) {
        QModelIndex index = appendToVarsList(toQString(dprts[i].name()),
                                             toQString(dprts[i].description()));

        InputPrivateData d;
        d.prts = dprts[i];
        d.viewIndex = index;
        d.scope = Constants::VARIABLE;
        d.setUncertainMapping(dVecUncertainties[i]);
        if(m_prob->boxConstraints()) {
            d.varLbound = m_prob->boxConstraints()->lowerBound(i);
            d.varUbound = m_prob->boxConstraints()->upperBound(i);
        } else {
            d.varLbound = Lowest;
            d.varUbound = Highest;
        }
        m_inputData[toQString(dprts[i].ID())] = d;
        m_varIDs.append(toQString(dprts[i].ID()));
    }

    //! [1] Load parameters
    TVector<ElementProperties> pprts = m_prob->pPrpts();
    TVector<IElementSPtr>      paramValue = m_prob->parameterVector();
    int nPar = pprts.size();
    for(int i=0; i<nPar; i++) {
        QModelIndex index = appendToParamsList(toQString(pprts[i].name()),
                                               toQString(pprts[i].description()));
        InputPrivateData d;
        d.prts = pprts[i];
        d.value = *paramValue[i];
        d.viewIndex = index;
        d.scope = Constants::PARAMETER;
        d.setExternal(isExternals[i]);
        m_inputData[toQString(pprts[i].ID())] = d;
        m_paramIDs.append(toQString(pprts[i].ID()));
        if(isExternals[i]) {
            QListWidgetItem* item = ui->paramsListWidget->item(index.row());
            QFont font = item->font();
            font.setItalic(true);
            item->setFont(font);
        }
    }

    //! [2] Load objectives and/or constraints and/or unused
    TVector<ElementProperties> oprts = m_prob->oPrpts();
    TVector<ElementProperties> cprts = m_prob->cPrpts();
    TVector<ElementProperties> uprts = m_prob->uPrpts();
    TVector<bool>           setGoals = m_prob->setGoalVector();
    TVector<IElementSPtr>      goals = m_prob->goalVector();
    TVector<int>          priorities = m_prob->priorityVector();
    TVector<IElementSPtr>      thresholds = m_prob->thresholdVector();
    int nObj = oprts.size();
    for(int i=0; i<nObj; i++) {
        QModelIndex index = appendToObjsList(toQString(oprts[i].name()),
                                             toQString(oprts[i].description()));
        m_objIDs.append(toQString(oprts[i].ID()));

        OutputPrivateData d;
        d.prts = oprts[i];
        d.objViewIndex = index;
        d.scopes.append(Constants::OBJECTIVE);
        d.setGoal = setGoals[i];
        d.goal = *goals[i];
        d.priority = priorities[i];
        m_outputData[d.ID()] = d;
    }

    int nUnu = uprts.size();
    for(int i=0; i<nUnu; i++) {
        QModelIndex index = appendToUnusedList(toQString(uprts[i].name()),
                                             toQString(uprts[i].description()));
        m_unusedID.append(toQString(uprts[i].ID()));

        OutputPrivateData d;
        d.prts = uprts[i];
        d.unusedViewIndex = index;
        d.scopes.append(Constants::UNUSED);
        m_outputData[toQString(uprts[i].ID())] = d;
    }

    int nCnstr = cprts.size();
    for(int i=0; i<nCnstr; i++) {
        QModelIndex index = appendToConstrList(toQString(cprts[i].name()),
                                               toQString(cprts[i].description()));
        m_cnstrIDs.append(toQString(cprts[i].ID()));

        if(m_outputData.keys().contains(toQString(cprts[i].ID()))) {
            // already laoded as objectives
            // Update the existing data
            QString id(toQString(cprts[i].ID()));
            m_outputData[id].cnstrViewIndex = index;
            m_outputData[id].scopes.append(Constants::CONSTRAINT);
            m_outputData[id].threshold = *thresholds[i];
        } else {
            // fresh constraint
            OutputPrivateData d;
            d.prts = cprts[i];
            d.cnstrViewIndex = index;
            d.scopes.append(Constants::CONSTRAINT);
            d.threshold = *thresholds[i];
            m_outputData[d.ID()] = d;
        }
    }

    //! [3] Load functions into model list (also load unused)
    TVector<IFunctionSPtr>     funcs = m_prob->functionVec();
    int nFuncs = funcs.size();
    QStringList keys = m_outputData.keys();
    for(int i=0; i<nFuncs; i++) {
        IFunctionSPtr func = funcs[i];

        QModelIndex index = appendToFuncsList(toQString(func->name()),
                                              toQString(func->description()));

        TVector<ElementProperties> iprts = func->inputPrpts();
        TVector<ElementProperties> oprts = func->outputPrpts();

        int ninput = iprts.size();
        for(int j=0; j<ninput; j++) {
            m_inputData[toQString(iprts[j].ID())].funcIndices.append(index);
        }

        int noutput = oprts.size();
        for(int j=0; j<noutput; j++) {
            if(!keys.contains(toQString(oprts[j].ID()))) {
                // unused
                m_unusedID.append(toQString(oprts[j].ID()));
                OutputPrivateData d;
                d.prts = oprts[j];
                d.unusedViewIndex = appendToUnusedList(toQString(oprts[j].name()),
                                                       toQString(oprts[j].description()));
                d.scopes.append(Constants::UNUSED);
                m_outputData[toQString(oprts[j].ID())] = d;
            }
            int rowCount = ui->functionListWidget->count();
            QModelIndex index = ui->functionListWidget->model()->index(rowCount-1,0);
            QString outputID = toQString(oprts[j].ID());
            m_outputData[outputID].funcIndices.append(index);
            // Load functon output uncertainties
            m_outputData[outputID].setUncertainMapping(funcUncertainties[i][j]);
        }
    }
}

ProblemDefinitionStatus QOpProbFormulationNodeForm::configureProblem(ProblemSPtr problem,
                                                                     TVector<IFunctionSPtr> funcs,
                                                                     TVector<TVector<int> > f2pmap,
                                                                     TVector<TVector<int> > f2omap,
                                                                     TVector<TVector<int> > f2cmap,
                                                                     TVector<TVector<int> > f2umap) const
{
    for(int i=0; i<funcs.size(); i++) {
        IFunctionSPtr func = funcs[i];
        // Extract inParamIdx, outConstrIdx, outObjIdx
        TVector<int>::iterator itr;
        // Param
        TVector<int> inParamIdx = f2pmap[i];
        itr = remove_if(inParamIdx.begin(), inParamIdx.end(), [](int itr){return itr==-1;});
        inParamIdx.erase(itr, inParamIdx.end());

        // Con
        TVector<int> outConstrIdx = f2cmap[i];
        itr = remove_if(outConstrIdx.begin(), outConstrIdx.end(), [](int itr){return itr==-1;});
        outConstrIdx.erase(itr, outConstrIdx.end());

        // Obj
        TVector<int> outObjIdx = f2omap[i];
        itr = remove_if(outObjIdx.begin(), outObjIdx.end(), [](int itr){return itr==-1;});
        outObjIdx.erase(itr, outObjIdx.end());

        TVector<int> outUnuIdx;
        if(!f2umap.empty()){
            outUnuIdx = f2umap[i];
            itr = remove_if(outUnuIdx.begin(), outUnuIdx.end(), [](int itr){return itr==-1;});
            outUnuIdx.erase(itr, outUnuIdx.end());
        } else {
            outUnuIdx = TVector<int>();
        }
        // Append
        problem->appendFunction(func, inParamIdx, outConstrIdx, outObjIdx, outUnuIdx);
    }

    return problem->processProblemDefinition();
}

void QOpProbFormulationNodeForm::createDockWidgets()
{
    // Title
    ui->varsDockWidget->setWindowTitle(tr("Variables"));
    ui->objsDockWidget->setWindowTitle(tr("Objectives"));
    ui->paramsDockWidget->setWindowTitle(tr("Parameters"));
    ui->consDockWidget->setWindowTitle(tr("Constraints"));
    ui->unusedDockWidget->setWindowTitle(tr("Unused"));

    QString stylesheet("QListView::item:hover:!selected {"
                       "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FAFBFE, stop: 1 #DCDEF1);"
                       "    border: 1px solid grey;"
                       "    border-radius: 5px;"
                       "}"
                       "QListView::item {"
                       "  padding: 2px;"
                       "}"
                       );
    ui->functionListWidget->setStyleSheet(stylesheet);
    ui->varsListWidget->setStyleSheet(stylesheet);
    ui->paramsListWidget->setStyleSheet(stylesheet);
    ui->objsListWidget->setStyleSheet(stylesheet);
    ui->consListWidget->setStyleSheet(stylesheet);
    ui->unusedListWidget->setStyleSheet(stylesheet);

    ui->varsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->paramsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->objsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->consListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->unusedListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->varsListWidget, &QListWidget::customContextMenuRequested,
            this, &QOpProbFormulationNodeForm::showVarContextMenu);
    connect(ui->paramsListWidget, &QListWidget::customContextMenuRequested,
            this, &QOpProbFormulationNodeForm::showParamContextMenu);

    ///\todo Yiming Implement the context menu
    //    connect(ui->objsListWidget, SIGNAL(customContextMenuRequested(QPoint)),
    //            this, SLOT(showObjContextMenu(QPoint)));
    //    connect(ui->consListWidget, SIGNAL(customContextMenuRequested(QPoint)),
    //            this, SLOT(showConsContextMenu(QPoint)));
    //    connect(ui->unusedListWidget, SIGNAL(customContextMenuRequested(QPoint)),
    //            this, SLOT(showUnusedContextMenu(QPoint)));

    connect(ui->functionListWidget, &CustomListWidget::emptyClick,
            this, &QOpProbFormulationNodeForm::clearHightlight);
    connect(ui->paramsListWidget, &CustomListWidget::emptyClick,
            ui->paramsListWidget, &CustomListWidget::clearSelection);

    // Save the state
    m_dockProperties = saveState(0);
}

bool QOpProbFormulationNodeForm::checkFunction(const TString &id)
{
    // Check duplication
    bool duplication = false;
    foreach (IFunctionSPtr funcItem, m_prob->functionVec()) {
        if(id == funcItem->ID()) {
            duplication = true;
            break;
        }
    }
    return duplication;
}

void QOpProbFormulationNodeForm::variableToParameter(const InputPrivateData &oldData,
                                                     const InputPrivateData &data)
{
    int nFuncs = ui->functionListWidget->count();
    int varIdx = oldData.viewIndex.row();
    QString id = oldData.ID();

    //! [0] Update Maps
    TVector<TVector<int> > f2dmap = m_prob->f2dMap();
    TVector<TVector<int> > f2pmap = m_prob->f2pMap();
    for(int i=0; i<nFuncs; i++) {
        f2pmap[i].push_back(f2dmap[i][varIdx]);
        removeAt(f2dmap[i], varIdx);
    }

    //! [1] Update prob
    qDebug() << "[] Update prob";
    ProblemSPtr problem = ProblemSPtr(new Problem);
    ProblemDefinitionStatus status = configureProblem(problem,
                                                      m_prob->functionVec(),
                                                      f2pmap,
                                                      m_prob->f2oMap(),
                                                      m_prob->f2cMap(),
                                                      m_prob->f2uMap());
    qDebug() << "process status " << status;
    if(status != FullyDefined) {
        return;
    }

    //! [2] Update Boxconstraints and dVecUncertainty
    int counter = 0;
    TVector<ElementProperties> dprpts = m_prob->dPrpts();
    TVector<IElement> lb = m_prob->boxConstraints()->lowerBounds();
    TVector<IElement> ub = m_prob->boxConstraints()->upperBounds();
    TVector<UncertaintyMapping*> dVecUncertainties = cloneDVecUncertaintyMapping();
    for(int i=0; i<dprpts.size(); i++) {
        // ID != id, this variable is still a variable
        if(id.compare(QString::fromStdString(dprpts[i].ID()), Qt::CaseSensitive)) {
            problem->boxConstraints()->defineLowerBound(counter, lb[i]);
            problem->boxConstraints()->defineUpperBound(counter, ub[i]);
            problem->redefineDVarUncertainty(counter, dVecUncertainties[i]);
            ++counter;
        }
    }
    problem->defineFuncOutUncertainties(cloneFuncOutUncertainties()); // define clones

    //! [3] Update ParamerValue
    TVector<IElementSPtr> parameterVector;
    TVector<bool> isExternal;
    TVector<ElementProperties> pPrpts = problem->pPrpts();
    counter = 0;
    int idx = -1;
    for(int i=0; i<pPrpts.size(); i++) {
        if(pPrpts[i].ID() == id) {
            // Transfered from variable
            parameterVector.push_back(IElementSPtr(new IElement(data.value)));
            isExternal.push_back(data.isExternal());
            idx = parameterVector.size() - 1;
        } else {
            // Parameter still parameter
            parameterVector.push_back(m_prob->parameterVector()[counter]);
            isExternal.push_back(m_prob->isExternalParameters()[counter]);
            ++counter;
        }
    }
    problem->defineParameterVector(parameterVector);
    problem->defineExternalParameters(isExternal);

    //! [4] Update goals, priorities, and thresholds
    problem->defineSetGoalVector(m_prob->setGoalVector());
    problem->defineGoalVector(m_prob->goalVector());
    problem->definePriorityVector(m_prob->priorityVector());
    problem->defineThresholdVector(m_prob->thresholdVector());

    //! [5] Process Problem
    status = problem->processProblemDefinition();

    if(status == FullyDefined) {
        m_prob = problem;
    } else {
        problem.reset();
        return;
    }

    //! [6] Updat GUI
    loadProblemToViews();

    emit opDataChanged();
    emit setProblemChanged();
}

void QOpProbFormulationNodeForm::parameterToVariable(const InputPrivateData& oldData,
                                                     const InputPrivateData& data)
{
    int nFuncs = ui->functionListWidget->count();
    int paramIdx = oldData.viewIndex.row();
    QString id = oldData.ID();

    TVector<TVector<int> > f2dmap = m_prob->f2dMap();
    TVector<TVector<int> > f2pmap = m_prob->f2pMap();
    TVector<IElement> lb = m_prob->boxConstraints()->lowerBounds();
    TVector<IElement> ub = m_prob->boxConstraints()->upperBounds();
    TVector<UncertaintyMapping*> dVecUncertainties = cloneDVecUncertaintyMapping();

    //! [0] Update Maps
    for(int i=0; i<nFuncs; i++) {
        f2dmap[i].push_back(f2pmap[i][paramIdx]);
        removeAt(f2pmap[i], paramIdx);
    }

    //! [1] Update prob
    ProblemSPtr problem = ProblemSPtr(new Problem);
    ProblemDefinitionStatus status = configureProblem(problem,
                                                      m_prob->functionVec(),
                                                      f2pmap,
                                                      m_prob->f2oMap(),
                                                      m_prob->f2cMap(),
                                                      m_prob->f2uMap());
    qDebug() << "process status " << status;
    if(status != FullyDefined) {
        return;
    }

    //! [2] Update Paramer Value
    TVector<IElementSPtr>  parameterVector = m_prob->parameterVector();
    TVector<bool>          isExternal = m_prob->isExternalParameters();
    removeAt(parameterVector, paramIdx);
    removeAt(isExternal, paramIdx);
    problem->defineParameterVector(parameterVector);
    problem->defineExternalParameters(isExternal);

    TVector<ElementProperties> dprpts = problem->dPrpts();
    int counter = 0;
    for(int i=0; i<dprpts.size(); i++) {
        //! [3] Update Boxconstraints and dVecUncertainty
        if(dprpts[i].ID() == id) {
            // Was parameter not variable
            problem->boxConstraints()->defineLowerBound(i, data.varLbound);
            problem->boxConstraints()->defineUpperBound(i, data.varUbound);
            problem->redefineDVarUncertainty(i, data.cloneUncertainMapping());
        } else {
            problem->boxConstraints()->defineLowerBound(i, lb[counter]);
            problem->boxConstraints()->defineUpperBound(i, ub[counter]);
            problem->redefineDVarUncertainty(i, dVecUncertainties[counter]);
            ++counter;
        }
    }

    //! [4] Define function output uncertainties
    problem->defineFuncOutUncertainties(cloneFuncOutUncertainties()); // define clones

    //! [5] Update goals, priorities, and threholds
    problem->defineSetGoalVector(m_prob->setGoalVector());
    problem->defineGoalVector(m_prob->goalVector());
    problem->definePriorityVector(m_prob->priorityVector());
    problem->defineThresholdVector(m_prob->thresholdVector());

    //! [6] Process Problem
    status = problem->processProblemDefinition();

    if(status == FullyDefined) {
        m_prob = problem;
    } else {
        problem.reset();
        return;
    }

    //! [7] update gui
    loadProblemToViews();

    emit opDataChanged();
    emit setProblemChanged();
}

void QOpProbFormulationNodeForm::replaceVariableData(const InputPrivateData &data)
{
    InputPrivateData oldData = m_inputData[data.ID()];
    QModelIndex index = oldData.viewIndex;
    int idx = index.row();
    QString id = m_varIDs.at(idx);

    //! [0] Replace the variable properties in functions
    for(int i=0; i<oldData.funcIndices.size(); i++) {
        IFunctionSPtr func = m_prob->functionVec()[oldData.funcIndices[i].row()];
        TVector<ElementProperties> iprts = func->inputPrpts();
        int idx = -1;
        for(int j=0; j<func->TP_nInputs(); j++) {
            if(id == iprts[j].ID()) {
                idx = j;
                break;
            }
        }
        if(idx != -1) {
            iprts[idx] = data.prts;
        } else {
            return;
        }
        func->defineInputPrpts(iprts);
    }

    //! [1] Replace variable property data in problem
    TVector<ElementProperties> probIprt = m_prob->dPrpts();
    probIprt[idx] = data.prts;
    m_prob->defineDVecPrpts(probIprt);
    if(m_prob->processProblemDefinition() != FullyDefined) {
        m_prob = m_pg->problem();
        qDebug() << "replace variabe prpts failed: cannot process problem";
        return;
    }

    //! [2] Update Boxconstraints
    TVector<IElement> lb = m_prob->boxConstraints()->lowerBounds();
    TVector<IElement> ub = m_prob->boxConstraints()->upperBounds();
    lb[idx] = data.varLbound;
    ub[idx] = data.varUbound;
    m_prob->boxConstraints()->defineLowerBounds(lb);
    m_prob->boxConstraints()->defineUpperBounds(ub);

    //! [3] Update Uncertainty
    qDebug() << "[] Update Uncertainty";
    int size = m_prob->dVecUncertainties().size();
    TVector<UncertaintyMapping*> dVecUncertainties(size);
    for(int i=0; i<size; i++) {
        dVecUncertainties[i] = Q_NULLPTR;
        if(i==idx) {
            dVecUncertainties[i] = data.cloneUncertainMapping(); // clone
        } else {
            if(m_prob->dVecUncertainties()[i] != Q_NULLPTR) {
                dVecUncertainties[i] = m_prob->dVecUncertainties()[i]->clone(); // clone
            }
        }
    }
    m_prob->defineDVecUncertainties(dVecUncertainties);

    //! [4] Process problem
    qDebug() << "[] Process problem";
    if(m_prob->processProblemDefinition() != FullyDefined) {
        m_prob = m_pg->problem();
        qDebug() << "replaceVariableData failed: cannot process problem";
        return;
    }

    //! [5] update internal data
    m_inputData[oldData.ID()] = data;

    emit opDataChanged();
    emit setProblemChanged();
}

void QOpProbFormulationNodeForm::replaceParameterData(const InputPrivateData &data)
{
    InputPrivateData oldData = m_inputData[data.ID()];
    QModelIndex index = oldData.viewIndex;
    int idx = index.row();
    QString id = m_paramIDs.at(idx);

    //! [0] Replace the input properties in functions
    for(int i=0; i<oldData.funcIndices.size(); i++) {
        IFunctionSPtr func = m_prob->functionVec()[oldData.funcIndices[i].row()];
        TVector<ElementProperties> iprts = func->inputPrpts();
        for(int j=0; j<func->TP_nInputs(); j++) {
            if(id == iprts[j].ID()) {
                iprts[j] = data.prts;
                break;
            }
        }
        func->defineInputPrpts(iprts);
    }

    //! [1] Replace parameter properties data in problem
    TVector<ElementProperties> probIprt = m_prob->pPrpts();
    probIprt[idx] = data.prts;
    m_prob->definePVecPrpts(probIprt);

    //! [2] Update ParamerValue or set as external
    TVector<bool> isExternal = m_prob->isExternalParameters();
    if(isExternal.empty()) {
        fill(isExternal, false, m_prob->parameterVector().size());
    }
    isExternal[idx] = data.isExternal();
    m_prob->defineExternalParameters(isExternal);
    //! [] Styling
    QListWidgetItem* item = ui->paramsListWidget->item(idx);
    QFont font = item->font();
    font.setItalic(data.isExternal());
    item->setFont(font);

    TVector<IElementSPtr>  parameterVector = m_prob->parameterVector();
    parameterVector[idx] = IElementSPtr(new IElement(data.value));
    m_prob->defineParameterVector(parameterVector);

    qDebug() << "Before process " << m_prob->parameterVector()[0]->dist();
    //! [3] Process Problem
    if(m_prob->processProblemDefinition() != FullyDefined) {
        m_prob = m_pg->problem();
        qDebug() << "replace parameter prpts failed: cannot process problem";
        return;
    }
    qDebug() << "After process " << m_prob->parameterVector()[0]->dist();

    //! [4] update internal data
    m_inputData[oldData.ID()] = data;

    emit opDataChanged();
    emit setProblemChanged();
}

QModelIndex QOpProbFormulationNodeForm::appendToFuncsList(const QString &name,
                                                          const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->functionListWidget->addItem(item);
    return ui->functionListWidget->model()->index(ui->functionListWidget->count()-1,0);
}

QModelIndex QOpProbFormulationNodeForm::appendToVarsList(const QString &name,
                                                         const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->varsListWidget->addItem(item);
    return ui->varsListWidget->model()->index(ui->varsListWidget->count()-1, 0);
}

QModelIndex QOpProbFormulationNodeForm::appendToParamsList(const QString &name,
                                                           const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->paramsListWidget->addItem(item);
    return ui->paramsListWidget->model()->index(ui->paramsListWidget->count()-1, 0);
}

QModelIndex QOpProbFormulationNodeForm::appendToObjsList(const QString &name,
                                                         const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->objsListWidget->addItem(item);
    return ui->objsListWidget->model()->index(ui->objsListWidget->count()-1, 0);
}

QModelIndex QOpProbFormulationNodeForm::appendToConstrList(const QString &name,
                                                           const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->consListWidget->addItem(item);
    return ui->consListWidget->model()->index(ui->consListWidget->count()-1, 0);
}

QModelIndex QOpProbFormulationNodeForm::appendToUnusedList(const QString &name, const QString &description)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setToolTip(description);
    ui->unusedListWidget->addItem(item);
    return ui->unusedListWidget->model()->index(ui->unusedListWidget->count()-1, 0);
}

bool QOpProbFormulationNodeForm::inputNameExist(const QString &name)
{
    for(int i=0; i<ui->varsListWidget->count(); i++) {
        if(name == ui->varsListWidget->item(i)->data(Qt::DisplayRole).toString()) {
            return true;
        }
    }
    for(int i=0; i<ui->paramsListWidget->count(); i++) {
        if(name == ui->paramsListWidget->item(i)->data(Qt::DisplayRole).toString()) {
            return true;
        }
    }
    return false;
}

int QOpProbFormulationNodeForm::checkInputDuplication(const ElementProperties &prt)
{
    QStringList ids = m_inputData.keys();
    return ids.indexOf(toQString(prt.ID()));
}

int QOpProbFormulationNodeForm::checkOutputDuplication(const ElementProperties &prt)
{
    QStringList ids = m_outputData.keys();
    return ids.indexOf(toQString(prt.ID()));
}

void QOpProbFormulationNodeForm::processSelectedFunction(const QString &className,
                                                         const QString &funcType,
                                                         const int &ninput,
                                                         const int &noutput)
{
    using namespace Tigon::Operators;
    qDebug() << "Selected function: " << className;
    qDebug() << "Selected function funcType: " << funcType;

    IFunctionSPtr func = ProblemGenerator::createFunction(funcType.toStdString(),
                                                          className.toStdString());

    if(func) {
        // Check duplication
        bool duplication = checkFunction(func->ID());
        if(!duplication) {
            if(ninput > -1) {
                func->TP_defineNInputs(ninput);
            }
            if(noutput > -1) {
                func->TP_defineNOutputs(noutput);
            }

            TVector<ElementProperties> iprts = func->inputPrpts();
            TVector<ElementProperties> oprts = func->outputPrpts();
            QVector<int> duplicatedInputs(iprts.size());
            QVector<int> duplicatedOutputs(oprts.size());

            // Check Inputs
            for(int i=0; i<iprts.size(); i++) {
                int idx = checkInputDuplication(iprts[i]);
                duplicatedInputs[i] = idx;
            }

            // Check Outputs
            for(int i=0; i<oprts.size(); i++) {
                int idx = checkOutputDuplication(oprts[i]);
                duplicatedOutputs[i] = idx;
            }

            m_inputoutputCheckingForm->setFunction(func,
                                                   duplicatedInputs,
                                                   duplicatedOutputs);
            m_inputoutputCheckingForm->show();
            m_inputoutputCheckingForm->raise();

        } else {
            QMessageBox::warning(this,
                                 tr("Function Duplication"),
                                 QString("Function "
                                         + toQString(func->name())
                                         + "has already been loaded."),
                                 QMessageBox::Ok);
        }
    } else {
        QMessageBox::warning(this,
                             tr("Error"),
                             QString("Function "
                                     + toQString(func->name())
                                     + "not found."),
                             QMessageBox::Ok);
    }
}

void QOpProbFormulationNodeForm::loadFunction(IFunctionSPtr func)
{
    if(ui->functionListWidget->count() == 0) {
        // no existing function
        m_prob->appendFunction(func);
        if(m_prob->processProblemDefinition() == FullyDefined) {
            emit setProblemChanged();
        } else {
            m_prob.reset();
        }
        return;
    }

    if(ui->functionListWidget->count() > 0) {

        TVector<ElementProperties> iprts = func->inputPrpts();
        TVector<ElementProperties> oprts = func->outputPrpts();
        TVector<IElement> lb = m_prob->boxConstraints()->lowerBounds();
        TVector<IElement> ub = m_prob->boxConstraints()->upperBounds();

        TVector<IElementSPtr> parameterVector;
        parameterVector.reserve(m_prob->parameterVector().size());
        for(IElementSPtr elm : m_prob->parameterVector()) {
            parameterVector.push_back(elm->clone());
        }

        TVector<UncertaintyMapping*> dVecUncertainties = cloneDVecUncertaintyMapping();
        TVector<TVector<UncertaintyMapping*> > funcOutUncertainties = cloneFuncOutUncertainties();
        TVector<bool>      setGoalVector = m_prob->setGoalVector();
        TVector<IElementSPtr> goalVector = m_prob->goalVector();
        TVector<int>      priorityVector = m_prob->priorityVector();
        TVector<IElementSPtr> thresholdVector = m_prob->thresholdVector();

        //! [] obtain maps
        TVector<int> inParam, inCnstr, inObj, inUnu;

        for(int i=0; i<iprts.size(); i++) {
            for(int j=0; j<m_paramIDs.size(); j++) {
                if(iprts[i].ID() == m_paramIDs[j]) {
                    inParam.push_back(i);
                    break;
                }
            }
        }

        for(int i=0; i<oprts.size(); i++) {
            for(int j=0; j<m_objIDs.size(); j++) {
                if(oprts[i].ID() == m_objIDs[j]) {
                    inObj.push_back(i);
                    break;
                }
            }
        }

        for(int i=0; i<oprts.size(); i++) {
            for(int j=0; j<m_cnstrIDs.size(); j++) {
                if(oprts[i].ID() == m_cnstrIDs[j]) {
                    inCnstr.push_back(i);
                    break;
                }
            }
        }

        for(int i=0; i<oprts.size(); i++) {
            for(int j=0; j<m_unusedID.size(); j++) {
                if(oprts[i].ID() == m_unusedID[j]) {
                    inUnu.push_back(i);
                    break;
                }
            }
        }

        m_prob->appendFunction(IFunctionSPtr(func), inParam, inCnstr, inObj, inUnu);
        if(m_prob->processProblemDefinition() != FullyDefined) {
            qDebug() << "not fully defined";
            return;
        }

        //! [] resotre previously defined data
        for(int i=0; i<lb.size(); i++) {
            m_prob->boxConstraints()->replaceLowerBound(i, lb[i]);
            m_prob->boxConstraints()->replaceUpperBound(i, ub[i]);
        }

        // Loading function will not create new parameters
        m_prob->defineParameterVector(parameterVector);

        for(int i=0; i<dVecUncertainties.size(); i++) {
            m_prob->redefineDVarUncertainty(i, dVecUncertainties[i]);
        }

        for(int i=0; i<funcOutUncertainties.size(); i++) {
            for(int j=0; j<funcOutUncertainties[i].size(); j++) {
                m_prob->redefineFuncOutputUncertainty(i, j,
                                                      funcOutUncertainties[i][j]);
            }
        }

        //! [] Update setGoals
        for(int i=0; i<setGoalVector.size(); i++) {
            m_prob->redefineSetGoal(i, setGoalVector[i]);
        }

        //! [] Update goals
        // Only need to copy the first goalVector.size() back
        for(int i=0; i<goalVector.size(); i++) {
            m_prob->redefineGoal(i, goalVector[i]);
        }

        //! [] Update priorities
        for(int i=0; i<priorityVector.size(); i++) {
            m_prob->redefinePriority(i, priorityVector[i]);
        }

        //! [] Update threholds
        // Only need to copy the first thresholdVector.size() back
        for(int i=0; i<thresholdVector.size(); i++) {
            m_prob->redefineThreshold(i, thresholdVector[i]);
        }

        ProblemDefinitionStatus status = m_prob->processProblemDefinition();
        if(status == FullyDefined) {
            emit setProblemChanged();
        } else {
            func.reset();
            return;
        }
    }
}

void QOpProbFormulationNodeForm::inputOutputReceived()
{
    IFunctionSPtr func = m_inputoutputCheckingForm->function();
    QVector<int> inputCheckingVec  = m_inputoutputCheckingForm->inputDuplications();
    //QVector<int> outputCheckingVec = m_inputoutputCheckingForm->outputDuplications();

    // Duplication checks again
    bool hasDuplication = false;
    TVector<ElementProperties> iprts = func->inputPrpts();
    TVector<ElementProperties> oprts = func->outputPrpts();
    QVector<int> duplicatedInputs(iprts.size());
    QVector<int> duplicatedOutputs(oprts.size());

    // Check Inputs
    for(int i=0; i<iprts.size(); i++) {
        int idx = checkInputDuplication(iprts[i]);
        duplicatedInputs[i] = idx;
        qDebug() << "i :" << i << " idx: " << idx << "inputCheckingVec[i] "
                 << inputCheckingVec[i] << "Check : "
                 << (idx != -1 && inputCheckingVec[i] == -1);
        if(idx != -1 && inputCheckingVec[i] == -1) {
            // Choose to rename and append but still has duplication
            hasDuplication = true;
        }
    }

    qDebug() << "Input Duplication? " << hasDuplication;

    // Check Outputs
    for(int i=0; i<oprts.size(); i++) {
        int idx = checkOutputDuplication(oprts[i]);
        duplicatedOutputs[i] = idx;
        //if(idx != -1 && outputCheckingVec[i] == -1) {
        if(idx != -1) {
            hasDuplication = true;
        }
    }

    if(hasDuplication) {
        m_inputoutputCheckingForm->setFunction(func, duplicatedInputs,
                                               duplicatedOutputs);
        m_inputoutputCheckingForm->show();
        m_inputoutputCheckingForm->raise();
    } else {
        loadFunction(func);
        // Rebuild problem for the gui from strach.
        // Trade efficiency for accuracy.
        loadProblemToViews();
    }
}

void QOpProbFormulationNodeForm::setProblemChanged()
{
    m_problemChanged = true;
}

void QOpProbFormulationNodeForm::updateInputData(const InputPrivateData &data)
{
    InputPrivateData oldData = m_inputData[data.ID()];

    if(oldData.scope != data.scope) {
        /// Change of scope
        if(data.scope == Constants::PARAMETER) {
            /// Variable ==> Parameter
            variableToParameter(oldData, data);
        } else if(data.scope == Constants::VARIABLE) {
            /// Variable ==> Parameter
            parameterToVariable(oldData, data);
        }
    } else {
        if(data.scope == Constants::VARIABLE) {
            replaceVariableData(data);
        } else if(data.scope == Constants::PARAMETER) {
            replaceParameterData(data);
        }
    }
}

void QOpProbFormulationNodeForm::updateOutputData(const OutputPrivateData &data)
{
    QString id = data.ID();
    OutputPrivateData oldData = m_outputData[id];
    // Replace old data with the new one
    m_outputData[id] = data;

    bool wasObj   = oldData.scopes.contains(Constants::OBJECTIVE);
    bool wasCnstr = oldData.scopes.contains(Constants::CONSTRAINT);
    bool wasUnusd = !(oldData.scopes.contains(Constants::OBJECTIVE) || oldData.scopes.contains(Constants::CONSTRAINT));
    bool isObj    = data.scopes.contains(Constants::OBJECTIVE);
    bool isCnstr  = data.scopes.contains(Constants::CONSTRAINT);
    bool isUnusd  = !(data.scopes.contains(Constants::OBJECTIVE) || data.scopes.contains(Constants::CONSTRAINT));

    int nFuncs = ui->functionListWidget->count();
    TVector<IFunctionSPtr> funcs  = m_prob->functionVec();
    TVector<ElementProperties> oprpts = m_prob->oPrpts();
    TVector<ElementProperties> cprpts = m_prob->cPrpts();
    TVector<ElementProperties> uprpts = m_prob->uPrpts();
    TVector<bool> setGoals          = m_prob->setGoalVector();
    TVector<IElementSPtr> goals     = m_prob->goalVector();
    TVector<int> priorities         = m_prob->priorityVector();
    TVector<IElementSPtr> threholds = m_prob->thresholdVector();
    TVector<TVector<int> > f2pmap = m_prob->f2pMap();
    TVector<TVector<int> > f2omap = m_prob->f2oMap();
    TVector<TVector<int> > f2cmap = m_prob->f2cMap();
    TVector<TVector<int> > f2umap = m_prob->f2uMap();
    TVector<TVector<UncertaintyMapping*> > funcUncertainties = cloneFuncOutUncertainties();

    if(wasObj && wasCnstr) {
        qDebug() << "Was obj and cnstr";
        int objIndex   = oldData.objViewIndex.row();
        int cnstrIndex = oldData.cnstrViewIndex.row();
        int unusdIndex = oldData.unusedViewIndex.row();

        if(isObj || isCnstr) {
            if(isObj) {
                qDebug() <<"Still obj";
                oprpts[objIndex] = data.prts;
                setGoals[objIndex] = data.setGoal;
                goals[objIndex]->defineValue(data.goal);
                priorities[objIndex] = data.priority;
            } else {
                qDebug() <<"Not obj any more";
                removeAt(oprpts, objIndex);
                removeAt(setGoals, objIndex);
                removeAt(goals, objIndex);
                removeAt(priorities, objIndex);
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2omap[i], objIndex);
                }
            }

            if(isCnstr) {
                qDebug() <<"Still Cnstr";
                cprpts[cnstrIndex] = data.prts;
                threholds[cnstrIndex]->defineValue(data.threshold);

            } else {
                qDebug() <<"Not Cnstr any more";
                removeAt(cprpts, cnstrIndex);
                removeAt(threholds, cnstrIndex);
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2cmap[i], cnstrIndex);
                }
            }

        } else {
            // unused
            qDebug() << "Unused";            
            removeAt(oprpts, objIndex);
            removeAt(setGoals, objIndex);
            removeAt(goals, objIndex);
            removeAt(priorities, objIndex);
            removeAt(cprpts, cnstrIndex);
            removeAt(threholds, cnstrIndex);
            // update f2omap and f2cmap
            for(int i=0; i<nFuncs; i++) {
                removeAt(f2omap[i], objIndex);
                removeAt(f2cmap[i], cnstrIndex);
            }

            uprpts[unusdIndex] = data.prts;
            uprpts.push_back(data.prts);
            //update f2umap
            for(int i=0; i<nFuncs; i++) {
                for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                    if(id == funcs[i]->outputPrpts()[j].ID()) {
                        f2umap[i].push_back(j);
                        break;
                    }
                }
            }
        }
    }

    if(wasObj && !wasCnstr) {
        qDebug() << "obj - yes, cnstr - no";
        int objIndex = oldData.objViewIndex.row();

        if(isObj || isCnstr) {
            if(!isObj) {
                qDebug() << "obj - no";
                removeAt(oprpts, objIndex);
                removeAt(setGoals, objIndex);
                removeAt(goals, objIndex);
                removeAt(priorities, objIndex);
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2omap[i], objIndex);
                }
            } else {
                oprpts[objIndex] = data.prts;
                setGoals[objIndex] = data.setGoal;
                goals[objIndex]->defineValue(data.goal);
                priorities[objIndex] = data.priority;
            }

            if(isCnstr) {
                qDebug() << "cnstr - yes";
                // update f2cmap
                for(int i=0; i<nFuncs; i++) {
                    f2cmap[i].push_back(m_prob->f2oMap()[i][objIndex]);
                }
                cprpts.push_back(data.prts);
                threholds.push_back(IElementSPtr(new IElement(data.threshold)));
            }
        } else {
            // unused
            qDebug() << "Unused";
            removeAt(oprpts, objIndex);
            removeAt(setGoals, objIndex);
            removeAt(goals, objIndex);
            removeAt(priorities, objIndex);
            // update f2omap
            for(int i=0; i<nFuncs; i++) {
                removeAt(f2omap[i], objIndex);
            }

            uprpts.push_back(data.prts);
            //update f2umap
            for(int i=0; i<nFuncs; i++) {
                for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                    if(id == funcs[i]->outputPrpts()[j].ID()) {
                        f2umap[i].push_back(j);
                        break;
                    }
                }
            }
        }
    }

    if(!wasObj && wasCnstr) {
        qDebug() << "Was cnstr but not obj";
        int cnstrIndex = oldData.cnstrViewIndex.row();
        if(isObj || isCnstr) {
            if(isObj) {
                qDebug() <<"Now obj";
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    f2omap[i].push_back(m_prob->f2cMap()[i][cnstrIndex]);
                }
                oprpts.push_back(data.prts);
                setGoals.push_back(data.setGoal);
                goals.push_back(IElementSPtr(new IElement(data.goal)));
                priorities.push_back(data.priority);
            }

            if(!isCnstr) {
                qDebug() <<"Not Cnstr any more";
                removeAt(cprpts, cnstrIndex);
                removeAt(threholds, cnstrIndex);
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2cmap[i], cnstrIndex);
                }

            } else {
                qDebug() <<"Still Cnstr";
                cprpts[cnstrIndex] = data.prts;
                threholds[cnstrIndex]->defineValue(data.threshold);
            }

        } else {
            // unused
            qDebug() << "Now Unused";
            removeAt(cprpts, cnstrIndex);
            removeAt(threholds, cnstrIndex);
            // update f2cmap
            for(int i=0; i<nFuncs; i++) {
                removeAt(f2cmap[i], cnstrIndex);
            }
            uprpts.push_back(data.prts);
            //update f2umap
            for(int i=0; i<nFuncs; i++) {
                for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                    if(id == funcs[i]->outputPrpts()[j].ID()) {
                        f2umap[i].push_back(j);
                        break;
                    }
                }
            }
        }
    }

    if(!wasObj && !wasCnstr) {
        qDebug() << "Was unused";
        int unstrIndex = oldData.unusedViewIndex.row();

        if(isObj || isCnstr) {
            if(isObj) {
                qDebug() <<"Now obj";
                removeAt(uprpts, unstrIndex);
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2umap[i], unstrIndex);
                }

                oprpts.push_back(data.prts);
                setGoals.push_back(data.setGoal);
                goals.push_back(IElementSPtr(new IElement(data.goal)));
                priorities.push_back(data.priority);
                // update f2omap
                for(int i=0; i<nFuncs; i++) {
                    for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                        if(id == funcs[i]->outputPrpts()[j].ID()) {
                            f2omap[i].push_back(j);
                            break;
                        }
                    }
                }
            }

            if(isCnstr) {
                qDebug() <<"Now Cnstr";
                removeAt(uprpts, unstrIndex);
                for(int i=0; i<nFuncs; i++) {
                    removeAt(f2umap[i], unstrIndex);
                }

                cprpts.push_back(data.prts);
                threholds.push_back(IElementSPtr(new IElement(data.threshold)));
                // update f2cmap
                for(int i=0; i<nFuncs; i++) {
                    for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                        if(id == funcs[i]->outputPrpts()[j].ID()) {
                            f2cmap[i].push_back(j);
                            break;
                        }
                    }
                }
            }
        } else {
            // unused
            qDebug() << "Still unused";
        }
    }

    //! [] Replace the output properties in functions
    for(int i=0; i<oldData.funcIndices.size(); i++) {
        IFunctionSPtr func = funcs[oldData.funcIndices[i].row()];
        TVector<ElementProperties> funcOprts = func->outputPrpts();
        int idx = -1;
        for(int j=0; j<func->TP_nOutputs(); j++) {
            if(id == funcOprts[j].ID()) {
                idx = j;
                break;
            }
        }
        if(idx != -1) {
            funcOprts[idx] = data.prts;
        } else {
            return;
        }
        func->defineOutputPrpts(funcOprts);
    }

    //! [] Update problem
    ProblemSPtr problem(new Problem);
    ProblemDefinitionStatus status = configureProblem(problem, funcs,
                                                      f2pmap, f2omap, f2cmap, f2umap);
    if(status == FullyDefined) {
        qDebug() << "Update problem structure: Fully defined.";
        //! [] Replace output property data in problem
        if(!wasObj && isObj) {
            // append obj
            TVector<ElementProperties> newOprpts;
            TVector<bool> newSetGoals;
            TVector<IElementSPtr> newGoals;
            TVector<int> newPriorities;
            int counter = 0;
            for(int i=0; i<problem->oPrpts().size(); i++) {
                if(id == problem->oPrpts()[i].ID()) {
                    newOprpts.push_back(data.prts);
                    newSetGoals.push_back(data.setGoal);
                    newGoals.push_back(IElementSPtr(new IElement(data.goal)));
                    newPriorities.push_back(data.priority);
                } else {
                    newOprpts.push_back(oprpts[counter]);
                    newSetGoals.push_back(setGoals[counter]);
                    newGoals.push_back(goals[counter]);
                    newPriorities.push_back(priorities[counter]);
                    ++counter;
                }
            }
            problem->defineOVecPrpts(newOprpts);
            problem->defineSetGoalVector(newSetGoals);
            problem->defineGoalVector(newGoals);
            problem->definePriorityVector(newPriorities);
        } else {
            problem->defineOVecPrpts(oprpts);
            problem->defineSetGoalVector(setGoals);
            problem->defineGoalVector(goals);
            problem->definePriorityVector(priorities);
        }

        if(!wasCnstr && isCnstr) {
            // append cnstr
            TVector<ElementProperties> newCprpts;
            TVector<IElementSPtr> newThreholds;
            int counter = 0;
            for(int i=0; i<problem->cPrpts().size(); i++) {
                if(id == problem->cPrpts()[i].ID()) {
                    newCprpts.push_back(data.prts);
                    newThreholds.push_back(IElementSPtr(new IElement(data.threshold)));
                } else {
                    newCprpts.push_back(cprpts[counter]);
                    newThreholds.push_back(threholds[counter]);
                    ++counter;
                }
            }
            problem->defineCVecPrpts(newCprpts);
            problem->defineThresholdVector(newThreholds);
        } else {
            problem->defineCVecPrpts(cprpts);
            problem->defineThresholdVector(threholds);
        }

        if(!wasUnusd && isUnusd) {
            //append Unused
            TVector<ElementProperties> newUprpts;
            int counter=0;
            for(int i=0; i<problem->uPrpts().size(); i++) {
                if(id == problem->uPrpts()[i].ID()){
                    newUprpts.push_back(data.prts);
                } else {
                    newUprpts.push_back(uprpts[counter]);
                    ++counter;
                }
            }
            problem->defineUVecPrpts(newUprpts);
        } else {
            problem->defineUVecPrpts(uprpts);
        }

        //! [] Update input properties
        problem->defineDVecPrpts(m_prob->dPrpts());
        problem->definePVecPrpts(m_prob->pPrpts());
        problem->defineBoxConstraints(m_prob->boxConstraints());
        problem->defineParameterVector(m_prob->parameterVector());
        problem->defineExternalParameters(m_prob->isExternalParameters());

        //! [] Update Uncertainties
        problem->defineDVecUncertainties(cloneDVecUncertaintyMapping());
        for(int i=0; i<oldData.funcIndices.size(); i++) {
            int funcIdx = oldData.funcIndices[i].row();
            for(int j=0; j<funcs[i]->TP_nOutputs(); j++) {
                if(data.ID() == funcs[i]->outputPrpts()[j].ID()) {
                    funcUncertainties[funcIdx][j] = data.cloneUncertainMapping();
                    break;
                }
            }
        }
        problem->defineFuncOutUncertainties(funcUncertainties);

        status = problem->processProblemDefinition();
        if(status == FullyDefined) {
            qDebug() << "Problem data: Fully defined.";
            m_prob = problem;
        } else {
            qDebug() << "Problem data: Not fully defined " << status;
        }
        loadProblemToViews();

        emit opDataChanged();
        emit setProblemChanged();
        return;
    }

    // Should not reach here if everything is OK
    qDebug() << "Not fully defined";
    loadProblemToViews();
}

void QOpProbFormulationNodeForm::clearContent()
{
    m_varIDs.clear();
    m_paramIDs.clear();
    m_objIDs.clear();
    m_cnstrIDs.clear();
    m_unusedID.clear();
    ui->functionListWidget->clear();
    ui->varsListWidget->clear();
    ui->paramsListWidget->clear();
    ui->objsListWidget->clear();
    ui->consListWidget->clear();
    ui->unusedListWidget->clear();
    m_inputData.clear();
    m_outputData.clear();
}

void QOpProbFormulationNodeForm::clearHightlight()
{
    ui->functionListWidget->clearSelection();
    for(int i = 0; i < ui->varsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->varsListWidget->item(i);
        item->setBackground(m_defaultColor);
    }

    for(int i = 0; i < ui->paramsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->paramsListWidget->item(i);
        item->setBackground(m_defaultColor);
    }

    for(int i = 0; i < ui->objsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->objsListWidget->item(i);
        item->setBackground(m_defaultColor);
    }

    for(int i = 0; i < ui->consListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->consListWidget->item(i);
        item->setBackground(m_defaultColor);
    }

    for(int i = 0; i < ui->unusedListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->unusedListWidget->item(i);
        item->setBackground(m_defaultColor);
    }
}

void QOpProbFormulationNodeForm::on_actionRestore_triggered()
{
    restoreState(m_dockProperties);
}

void QTigon::QOpProbFormulationNodeForm::on_loadButton_clicked()
{
    // load a new function
    // Add all possible loadable functions from Tigon
    TStringList names = TigonFunctionFactory::instance()->registeredFunctions();
    QStringList probNames;
    for(TString & name : names) {
        replaceAll(name, "Tigon::Representation::", "");
        if (name != "MatlabFunction" && name != "PythonFunction") {
            probNames.append(QString::fromStdString(name));
        }
    }
    probNames.insert(0,"");
    FunctionLoaderDialog* funcLoaderDiag = new FunctionLoaderDialog;
    funcLoaderDiag->setFunctionNames(probNames);

    connect(funcLoaderDiag, &FunctionLoaderDialog::functionSelected,
            this, &QOpProbFormulationNodeForm::processSelectedFunction);

    funcLoaderDiag->setAttribute(Qt::WA_DeleteOnClose);
    funcLoaderDiag->setModal(true); // block inputs to other windows

    funcLoaderDiag->show();
}

void QTigon::QOpProbFormulationNodeForm::on_removeButton_clicked()
{
    //! [0] Get the indice of the selected functions
    QModelIndex index = ui->functionListWidget->currentIndex();
    int row = index.row();
    qDebug() << "selected function index : " << row;
    if(row == -1) {
        return;
    }

    //! [1] Deal with easy cases first
    // If only one function left, just delete the problem completely
    if(row == 0 && ui->functionListWidget->count() == 1) {
        m_prob.reset();
        m_prob = ProblemSPtr(new Problem);
        clearContent();
        return;
    }

    //! [2] Update funcs and maps
    TVector<IFunctionSPtr> funcs  = m_prob->functionVec();
    TVector<TVector<int> > f2dmap = m_prob->f2dMap();
    TVector<TVector<int> > f2pmap = m_prob->f2pMap();
    TVector<TVector<int> > f2omap = m_prob->f2oMap();
    TVector<TVector<int> > f2cmap = m_prob->f2cMap();
    TVector<TVector<int> > f2umap = m_prob->f2uMap();

    removeAt(funcs,  row);
    removeAt(f2dmap, row);
    removeAt(f2pmap, row);
    removeAt(f2omap, row);
    removeAt(f2cmap, row);

    int nd = m_prob->dPrpts().size();
    int np = m_prob->pPrpts().size();

    TVector<TVector<int> > newF2pmap(f2pmap.size());
    for(int col=0; col<np; col++) {
        for(int r=0; r<f2pmap.size(); r++) {
            if(f2pmap[r][col] != -1) {
                for(int k=0; k<f2pmap.size(); k++) {
                    newF2pmap[k].push_back(f2pmap[k][col]);
                }
                break;
            }
        }
    }

    int no = m_prob->oPrpts().size();
    TVector<TVector<int> > newF2omap(f2omap.size());
    for(int col=0; col<no; col++) {
        for(int r=0; r<f2omap.size(); r++) {
            if(f2omap[r][col] != -1) {
                for(int k=0; k<f2omap.size(); k++) {
                    newF2omap[k].push_back(f2omap[k][col]);
                }
                break;
            }
        }
    }

    int nc = m_prob->cPrpts().size();
    TVector<TVector<int> > newF2cmap(f2cmap.size());
    for(int col=0; col<nc; col++) {
        for(int r=0; r<f2cmap.size(); r++) {
            if(f2cmap[r][col] != -1) {
                for(int k=0; k<f2cmap.size(); k++) {
                    newF2cmap[k].push_back(f2cmap[k][col]);
                }
                break;
            }
        }
    }

    int nu = m_prob->uPrpts().size();
    TVector<TVector<int> > newF2umap(f2umap.size());
    for(int col=0; col<nu; col++) {
        for(int r=0; r<f2umap.size(); r++) {
            if(f2umap[r][col] != -1) {
                for(int k=0; k<f2umap.size(); k++) {
                    newF2umap[k].push_back(f2umap[k][col]);
                }
                break;
            }
        }
    }

    //! [3] Update problem
    qDebug() << "[3] Update problem";
    ProblemSPtr problem(new Problem);
    ProblemDefinitionStatus status = configureProblem(problem, funcs, newF2pmap,
                                                      newF2omap, newF2cmap, newF2umap);
    if(status != FullyDefined) {
        return;
    }

    //! [4] Update Boxconstraints and dVecUncertainties
    TVector<IElement> lb = m_prob->boxConstraints()->lowerBounds();
    TVector<IElement> ub = m_prob->boxConstraints()->upperBounds();
    TVector<UncertaintyMapping*> dVecUncertainties = cloneDVecUncertaintyMapping();

    TVector<IElement> newLb, newUb;
    TVector<UncertaintyMapping*> newDVecUncertainties;
    for(int col=0; col<nd; col++) {
        for(int r=0; r<f2dmap.size(); r++) {
            if(f2dmap[r][col] != -1) {
                newLb.push_back(lb[col]);
                newUb.push_back(ub[col]);
                newDVecUncertainties.push_back(dVecUncertainties[col]);
                break;
            }
        }
    }
    problem->boxConstraints()->defineLowerBounds(newLb);
    problem->boxConstraints()->defineUpperBounds(newUb);
    problem->defineDVecUncertainties(newDVecUncertainties);

    //! [5] Update parameter vector
    TVector<IElementSPtr>  parameterVector = m_prob->parameterVector();
    TVector<IElementSPtr> newParameterVector;
    for(int col=0; col<np; col++) {
        for(int r=0; r<f2pmap.size(); r++) {
            if(f2pmap[r][col] != -1) {
                newParameterVector.push_back(parameterVector[col]);
                break;
            }
        }
    }
    problem->defineParameterVector(newParameterVector);

    //! [6] Define funcOutUncertainties
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties = cloneFuncOutUncertainties();
    removeAt(funcOutUncertainties, row);
    problem->defineFuncOutUncertainties(funcOutUncertainties);

    //! [] Update goals and priorities

    TVector<bool> setGoalVector = m_prob->setGoalVector();
    TVector<bool> newSetGoalVector;

    TVector<IElementSPtr> goalVector = m_prob->goalVector();
    TVector<IElementSPtr> newGoalVector;

    TVector<int> priorityVector = m_prob->priorityVector();
    TVector<int> newPriorityVector;

    for(int col=0; col<no; col++) {
        for(int r=0; r<f2omap.size(); r++) {
            if(f2omap[r][col] != -1) {
                setGoalVector.push_back(setGoalVector[col]);
                newGoalVector.push_back(goalVector[col]);
                newPriorityVector.push_back(priorityVector[col]);
                break;
            }
        }
    }
    problem->defineSetGoalVector(newSetGoalVector);
    problem->defineGoalVector(newGoalVector);
    problem->definePriorityVector(newPriorityVector);

    //! [] Update threhold
    TVector<IElementSPtr> thresholdVector = m_prob->thresholdVector();
    TVector<IElementSPtr> newThresholdVector;
    for(int col=0; col<nc; col++) {
        for(int r=0; r<f2cmap.size(); r++) {
            if(f2cmap[r][col] != -1) {
                newThresholdVector.push_back(thresholdVector[col]);
                break;
            }
        }
    }
    problem->defineThresholdVector(newThresholdVector);


    //! [7] Process Problem
    status = problem->processProblemDefinition();
    if(status == FullyDefined) {
        qDebug() << "Fully defined";
        m_prob = problem;
        //! [8] Reload functions/inputs/outputs
        loadProblemToViews();
        emit setProblemChanged();
    } else {
        // Do nothing
        qDebug() << "Not Fully defined : " << status;
        problem.reset();
    }
}

void QOpProbFormulationNodeForm::on_varsListWidget_doubleClicked(const QModelIndex &index)
{
    QString id = m_varIDs.at(index.row());
    InputPrivateData d = m_inputData[id];
    m_inputPropertiesDialog->setData(d);
    m_inputPropertiesDialog->show();
    m_inputPropertiesDialog->raise();
    //InputPropertiesDialog* diag = new InputPropertiesDialog;
    //diag->setData(d);
    //connect(diag, &InputPropertiesDialog::finished,
    //        diag, &InputPropertiesDialog::deleteLater);
    //connect(diag, &InputPropertiesDialog::updateData,
    //        this, &QOpProbFormulationNodeForm::updateInputData);
    //diag->show();
    //diag->raise();
}

void QOpProbFormulationNodeForm::on_paramsListWidget_doubleClicked(const QModelIndex &index)
{

    QString id = m_paramIDs.at(index.row());
    InputPrivateData d = m_inputData[id];
    m_inputPropertiesDialog->setData(d);
    m_inputPropertiesDialog->show();
    m_inputPropertiesDialog->raise();
}

void QOpProbFormulationNodeForm::on_objsListWidget_doubleClicked(const QModelIndex &index)
{
    QString id = m_objIDs.at(index.row());
    OutputPrivateData d = m_outputData[id];
    m_outputPropertiesDialog->setData(d);
    m_outputPropertiesDialog->show();
    m_outputPropertiesDialog->raise();
}

void QOpProbFormulationNodeForm::on_consListWidget_doubleClicked(const QModelIndex &index)
{
    QString id = m_cnstrIDs.at(index.row());
    OutputPrivateData d = m_outputData[id];
    m_outputPropertiesDialog->setData(d);
    m_outputPropertiesDialog->show();
    m_outputPropertiesDialog->raise();
}

void QOpProbFormulationNodeForm::on_unusedListWidget_doubleClicked(const QModelIndex &index)
{
    QString id = m_unusedID.at(index.row());
    OutputPrivateData d = m_outputData[id];
    m_outputPropertiesDialog->setData(d);
    m_outputPropertiesDialog->show();
    m_outputPropertiesDialog->raise();
}

void QTigon::QOpProbFormulationNodeForm::on_savButton_clicked()
{
    qDebug() << "Define the problem";
    if(m_prob->functionVec().size() == 0) {
        m_prob = ProblemSPtr(new Problem);
        m_pg->defineProblem(m_prob);
        return;
    }
    ProblemDefinitionStatus status = m_prob->processProblemDefinition();
    if(status == FullyDefined) {
        m_pg->defineProblem(m_prob);
        m_problemChanged = false;
        emit opDataChanged();
    } else {
        QMessageBox::StandardButton reply
                = QMessageBox::critical(this, "Save Problem Formulation",
                                        tr("Error: Cannot save the current problem."),
                                        QMessageBox::Discard | QMessageBox::Cancel);
        if(reply == QMessageBox::Discard) {
            m_prob = m_pg->problem();
            m_problemChanged = false;
        }
    }
}

void QOpProbFormulationNodeForm::on_functionListWidget_clicked(const QModelIndex &index)
{
    qDebug() << "Highlight for function " << index.row();
    ui->varsListWidget->clearSelection();
    ui->objsListWidget->clearSelection();
    ui->paramsListWidget->clearSelection();
    ui->consListWidget->clearSelection();
    ui->unusedListWidget->clearSelection();

    qDebug() << "Highlight input";
    QMapIterator<QString, InputPrivateData> iter_input(m_inputData);
    while (iter_input.hasNext()) {
        iter_input.next();
        if(iter_input.value().funcIndices.contains(index)) {
            if(iter_input.value().scope == Constants::VARIABLE) {
                ui->varsListWidget->item(iter_input.value().viewIndex.row())
                        ->setBackground(m_highlightColor);
            }
            if(iter_input.value().scope == Constants::PARAMETER) {
                ui->paramsListWidget->item(iter_input.value().viewIndex.row())
                        ->setBackground(m_highlightColor);
            }
        } else {
            if(iter_input.value().scope == Constants::VARIABLE) {
                ui->varsListWidget->item(iter_input.value().viewIndex.row())
                        ->setBackground(m_defaultColor);
            }
            if(iter_input.value().scope == Constants::PARAMETER) {
                ui->paramsListWidget->item(iter_input.value().viewIndex.row())
                        ->setBackground(m_defaultColor);
            }
        }
    }

    qDebug() << "Highlight output";
    QMapIterator<QString, OutputPrivateData> iter_output(m_outputData);
    while (iter_output.hasNext()) {
        iter_output.next();
        if(iter_output.value().funcIndices.contains(index)) {
            if(iter_output.value().scopes.contains(Constants::OBJECTIVE)) {
                ui->objsListWidget->item(iter_output.value().objViewIndex.row())
                        ->setBackground(m_highlightColor);
            }
            if(iter_output.value().scopes.contains(Constants::CONSTRAINT)) {
                ui->consListWidget->item(iter_output.value().cnstrViewIndex.row())
                        ->setBackground(m_highlightColor);
            }
            if(iter_output.value().scopes.contains(Constants::UNUSED)) {
                ui->unusedListWidget->item(iter_output.value().unusedViewIndex.row())
                        ->setBackground(m_highlightColor);
            }
        } else {
            if(iter_output.value().scopes.contains(Constants::OBJECTIVE)) {
                ui->objsListWidget->item(iter_output.value().objViewIndex.row())
                        ->setBackground(m_defaultColor);
            }
            if(iter_output.value().scopes.contains(Constants::CONSTRAINT)) {
                ui->consListWidget->item(iter_output.value().cnstrViewIndex.row())
                        ->setBackground(m_defaultColor);
            }
            if(iter_output.value().scopes.contains(Constants::UNUSED)) {
                ui->unusedListWidget->item(iter_output.value().unusedViewIndex.row())
                        ->setBackground(m_defaultColor);
            }
        }
    }
}

void QOpProbFormulationNodeForm::showVarContextMenu(const QPoint &point)
{
    // Handle global position
    QPoint globalPos = ui->varsListWidget->mapToGlobal(point);
    QModelIndex idx = ui->varsListWidget->indexAt(point);
    if(idx.row() != -1) {
        QMenu myMenu;
        myMenu.addAction(tr("Rename"), this, &QOpProbFormulationNodeForm::renameVar);
        myMenu.addAction(tr("Show Properties"), this, &QOpProbFormulationNodeForm::showVarPrptForm);
        myMenu.exec(globalPos);
    }
}

void QOpProbFormulationNodeForm::showParamContextMenu(const QPoint &point)
{
    // Handle global position
    QPoint globalPos = ui->paramsListWidget->mapToGlobal(point);
    QModelIndex idx  = ui->paramsListWidget->indexAt(point);
    if(idx.row() != -1) {
        QMenu myMenu;
        myMenu.addAction(tr("Rename"), this,
                         &QOpProbFormulationNodeForm::renameParam);
        myMenu.addAction(tr("Show Properties"), this,
                         &QOpProbFormulationNodeForm::showParamPrptForm);
        myMenu.exec(globalPos);
    }
}

void QOpProbFormulationNodeForm::showVarPrptForm()
{
    QModelIndex index = ui->varsListWidget->currentIndex();
    QString id = m_varIDs.at(index.row());
    InputPrivateData d = m_inputData[id];
    m_inputPropertiesDialog->setData(d);
    m_inputPropertiesDialog->show();
    m_inputPropertiesDialog->raise();
}

void QOpProbFormulationNodeForm::showParamPrptForm()
{
    QModelIndex index = ui->paramsListWidget->currentIndex();
    QString id = m_paramIDs.at(index.row());
    InputPrivateData d = m_inputData[id];
    m_inputPropertiesDialog->setData(d);
    m_inputPropertiesDialog->show();
    m_inputPropertiesDialog->raise();
}

void QOpProbFormulationNodeForm::renameVar()
{
    QModelIndex index = ui->varsListWidget->currentIndex();

    //! [0] Get new name
    bool ok;
    QString text = QInputDialog::getText(this,tr("Change Variable Name"),
                                         tr("Name"), QLineEdit::Normal,
                                         index.data().toString(),
                                         &ok);

    if (ok && !text.isEmpty() && text != index.data().toString()) {
        if(inputNameExist(text)) {
            QMessageBox::information(this,"Rename Variable"
                                     , QString("Input name "
                                               + text + " already exists")
                                     , QMessageBox::Ok);
            return;
        }
        int varIndex = index.row();
        QString id = m_varIDs.at(varIndex);
        InputPrivateData d = m_inputData[id];

        ElementProperties oldPrts = d.prts;
        ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();
        ElementProperties newPrt = fct->createElementProperties(oldPrts.idx(),
                                                                text.toStdString(),
                                                                oldPrts.description(),
                                                                oldPrts.type(),
                                                                oldPrts.optimizationType());

        //! [1] Replace the input properties in function
        // Type will not be changed. Only name and id;
        for(int i=0; i<d.funcIndices.size(); i++) {
            IFunctionSPtr func = m_prob->functionVec()[d.funcIndices[i].row()];
            TVector<ElementProperties> iprts = func->inputPrpts();
            int idx = -1;
            for(int j=0; j<func->TP_nInputs(); j++) {
                if(id == iprts[j].ID()) {
                    idx = j;
                    qDebug() << "idx" << idx;
                    break;
                }
            }
            if(idx != -1) {
                iprts[idx] = newPrt;
            } else {
                return;
            }
            func->defineInputPrpts(iprts);
        }

        //! [2] Replace variable property data in problem
        TVector<ElementProperties> probIprt = m_prob->dPrpts();
        probIprt[varIndex] = newPrt;
        m_prob->defineDVecPrpts(probIprt);
        if(m_prob->processProblemDefinition() != FullyDefined) {
            m_prob = m_pg->problem();
            qDebug() << "Rename failed: cannot process problem";
            return;
        }

        //! [3] Update internal data
        d.prts = newPrt;
        m_varIDs.replace(m_varIDs.indexOf(id), toQString(newPrt.ID()));
        m_inputData.remove(id);
        m_inputData[toQString(newPrt.ID())] = d;

        //! [4] Finally update GUI
        ui->varsListWidget->currentItem()->setData(Qt::DisplayRole, text);
        setProblemChanged();
        emit opDataChanged();
        //loadProblemToViews();
    } else {
        return;
    }
}

void QOpProbFormulationNodeForm::renameParam()
{
    QModelIndex index = ui->paramsListWidget->currentIndex();

    //! [0] Get new name
    bool ok;
    QString text = QInputDialog::getText(this,tr("Change Parameter Name"),
                                         tr("Name"),QLineEdit::Normal,
                                         index.data().toString(),
                                         &ok);
    if (ok && !text.isEmpty() && text != index.data().toString()) {
        if(inputNameExist(text)) {
            QMessageBox::information(this,"Rename Variable"
                                     , QString("Input name "
                                               + text + " already exists")
                                     , QMessageBox::Ok);
            return;
        }

        int paramIndex = index.row();
        QString id = m_paramIDs.at(paramIndex);
        InputPrivateData d = m_inputData[id];

        ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();
        ElementProperties oldPrt = d.prts;
        ElementProperties newPrt = fct->createElementProperties(oldPrt.idx(),
                                                                text.toStdString(),
                                                                oldPrt.description(),
                                                                oldPrt.type(),
                                                                oldPrt.optimizationType());
        //! [1] Replace the input properties in function
        // Type will not be changed. Only name and id;
        for(int i=0; i<d.funcIndices.size(); i++) {
            IFunctionSPtr func = m_prob->functionVec()[d.funcIndices[i].row()];
            TVector<ElementProperties> iprts = func->inputPrpts();
            int idx = -1;
            for(int j=0; j<func->TP_nInputs(); j++) {
                if(id == iprts[j].ID()) {
                    idx = j;
                    qDebug() << "idx" << idx;
                    break;
                }
            }
            if(idx != -1) {
                iprts[idx] = newPrt;
            } else {
                return;
            }
            func->defineInputPrpts(iprts);
        }

        //! [2] Replace parameter property data in problem
        TVector<ElementProperties> probIprt = m_prob->pPrpts();
        probIprt[paramIndex] = newPrt;
        m_prob->definePVecPrpts(probIprt);
        if(m_prob->processProblemDefinition() != FullyDefined) {
            m_prob = m_pg->problem();
            qDebug() << "Rename failed: cannot process problem";
            return;
        }

        //! [3] Update internal data
        d.prts = newPrt;
        m_paramIDs.replace(m_paramIDs.indexOf(id), toQString(newPrt.ID()));
        m_inputData.remove(id);
        m_inputData[toQString(newPrt.ID())] = d;

        //! [4] Finally update GUI
        ui->paramsListWidget->currentItem()->setData(Qt::DisplayRole, text);
        //loadProblemToViews();

        setProblemChanged();
        emit opDataChanged();
    } else {
        return;
    }
}

/*! Create a clone of dVecUncertainties to avoid memory conflict
 * when assgin them back using defineDVecUncertainties method, as
 * defineDVecUncertainties will clear the memory of the old defineDVecUncertainties
 * vector first.
 */
TVector<UncertaintyMapping *> QOpProbFormulationNodeForm::cloneDVecUncertaintyMapping() const
{
    int size = m_prob->dVecUncertainties().size();
    TVector<UncertaintyMapping*> dVecUncertainties(size);
    for(int i=0; i< size; i++) {
        if(m_prob->dVecUncertainties()[i] != Q_NULLPTR) {
            dVecUncertainties[i] = m_prob->dVecUncertainties()[i]->clone();
        } else {
            dVecUncertainties[i] = Q_NULLPTR;
        }
    }
    return dVecUncertainties;
}

Tigon::TVector<Tigon::TVector<UncertaintyMapping *> > QOpProbFormulationNodeForm::cloneFuncOutUncertainties() const
{
    TVector<TVector<UncertaintyMapping *> > funcOutUncertainties;
    funcOutUncertainties.resize(m_prob->funcOutUncertainties().size());
    for(int i=0; i<m_prob->funcOutUncertainties().size(); i++) {
        funcOutUncertainties[i].resize(m_prob->funcOutUncertainties()[i].size());
        for(int j=0; j<m_prob->funcOutUncertainties()[i].size(); j++) {
            if(m_prob->funcOutUncertainties()[i][j] != Q_NULLPTR) {
                funcOutUncertainties[i][j] = m_prob->funcOutUncertainties()[i][j]->clone();
            } else {
                funcOutUncertainties[i][j] = Q_NULLPTR;
            }
        }
    }
    return funcOutUncertainties;
}

void QTigon::QOpProbFormulationNodeForm::on_actionSave_triggered()
{
    QString fname = QFileDialog::getSaveFileName(this,"Save...",
                                                 QDir::homePath(),
                                                 "JSON Type (*.json)");
    if(fname.isEmpty()) {
        return;
    }

    Json::JsonObject jprob;
    if(m_pg) {
        jprob = Tigon::problemToJsonObject(m_pg->problem());
    } else {
        return;
    }
    Json::JsonDocument jdoc;
    jdoc.setObject(jprob);
    QString content = QString::fromStdString(jdoc.toJson());

    QFile file(fname);
    if(!file.open(QIODevice::WriteOnly)) {
        return;
    }
    file.write(content.toUtf8());
    file.close();
}

void QTigon::QOpProbFormulationNodeForm::on_actionLoad_triggered()
{
    QString fname = QFileDialog::getOpenFileName(this,"Open...",
                                                 QDir::homePath(),
                                                 "JSON Type (*.json)");
    if(fname.isEmpty()) {
        return;
    }

    QFile file(fname);
    if(!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray content = file.readAll();

    ProblemDefinitionStatus status = m_pg->setProblemDataJson(content.toStdString());
    if(status == FullyDefined) {
        // Update problem
        m_prob = m_pg->problem();
        loadProblemToViews();
        emit opDataChanged();
    } else {
        qDebug() << "Not fully defined";
        QMessageBox::warning(this, tr("Problem Formulation"),
                             QString("Problem is not fully defined."),
                             QMessageBox::Ok);
    }
}
