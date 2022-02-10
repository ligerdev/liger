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
#include <visualisation/qvizobjectiveranknode.h>

// Tigon
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/Problems/Problem.h>

#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/ObjectiveReduction/IObjectiveReduction.h>
#include <tigon/Operators/ObjectiveReduction/LPCA.h>
#ifdef CSDP_LIBRARY
  #include <tigon/Operators/ObjectiveReduction/NLMVUPCA.h>
#endif
#include <tigon/ExceptionHandling/TException.h>

// Qt
#include <QCheckBox>
#include <QComboBox>
#include <QAction>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;

namespace Visualisation {

QVizObjectiveRankNode::QVizObjectiveRankNode()
    : m_visualSelectCombo(0)
    , m_algorithmSelectCombo(0)
    , m_standardizeButton(0)
{
    m_selectVisualisationOptions << "Ranking";
    m_selectVisualisationOptions << "Correlations";
    m_selectVisualisationOptions << "Variance PCs";

    m_selectAlgorithmOptions << "L-PCA";
#ifdef CSDP_LIBRARY
    m_selectAlgorithmOptions << "NL-MVU-PCA";
#endif
    setSvg(":/visualisation/images/qvizobjectiveranknode.svg");
}

QVizObjectiveRankNode::~QVizObjectiveRankNode()
{

}

void QVizObjectiveRankNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    ObjectiveRankWidget* widget = new ObjectiveRankWidget;

    m_selectedVisualisation = VisualisationType::Ranking;
    m_selectedAlgorithm = ObjectiveReductionAlgorithm::L_PCA;
#ifdef CSDP_LIBRARY
    m_standardize = true;
#endif
    initialiseVizNode(widget);

    showWidget();
}

void QVizObjectiveRankNode::customiseWidget(VisualisationWidget* widget)
{
    ObjectiveRankWidget* sWidget = static_cast<ObjectiveRankWidget*>(widget);
    connect(sWidget, &ObjectiveRankWidget::runObjectiveReductionAlgorithm,
            this, &QVizObjectiveRankNode::runObjectiveReduction);

    sWidget->addSpacerToToolBar();

    setupVisualisationSelection();
    setupAlgorithmSelection();
//    setupSetSelection();

    sWidget->addSpacerToToolBar();

    QString checkboxStyle = "QCheckBox {color: white}";
#ifdef CSDP_LIBRARY
    setupStandardizeCheckbox(checkboxStyle);
#endif
    setupInSyncCheckbox(checkboxStyle);

    runObjectiveReduction();
}

void QVizObjectiveRankNode::setupVisualisationSelection()
{
    m_visualSelectCombo = new QComboBox(widgetObj()->centralWidget());
    m_visualSelectCombo->setToolTip("Select visualisation");

    QStringList list;
    for(auto option : m_selectVisualisationOptions) {
        list << option;
    }

    m_visualSelectCombo->addItems(list);
    widgetObj()->addItemToToolBar(m_visualSelectCombo);

    connect(m_visualSelectCombo, &QComboBox::currentTextChanged,
            this, &QVizObjectiveRankNode::selectVisualisation);

    QAction* selVizAct = widgetObj()->addActionToMenuEdit("Select Visualisation");
    connect(selVizAct, &QAction::triggered,
            this, &QVizObjectiveRankNode::selectVisualisationComboPopup);

}

void QVizObjectiveRankNode::selectVisualisationComboPopup()
{
    m_visualSelectCombo->raise();
    m_visualSelectCombo->showPopup();
}

void QVizObjectiveRankNode::selectVisualisation(const QString &setName)
{
    if(setName == m_selectVisualisationOptions[0]) {
        m_selectedVisualisation = VisualisationType::Ranking;
    } else if(setName == m_selectVisualisationOptions[1]) {
        m_selectedVisualisation = VisualisationType::VarCorr;
    } else if(setName == m_selectVisualisationOptions[2]) {
        m_selectedVisualisation = VisualisationType::VarPCs;
    }

    static_cast<ObjectiveRankWidget*>(widgetObj())->
            setVisualisationType(m_selectedVisualisation);
}

void QVizObjectiveRankNode::setupAlgorithmSelection()
{
    m_algorithmSelectCombo = new QComboBox(widgetObj()->centralWidget());
    m_algorithmSelectCombo->setToolTip("Select algorithm");

    QStringList list;
    list << m_selectAlgorithmOptions[0];
#ifdef CSDP_LIBRARY
    list << m_selectAlgorithmOptions[1];
#endif
    m_algorithmSelectCombo->addItems(list);
    widgetObj()->addItemToToolBar(m_algorithmSelectCombo);

    connect(m_algorithmSelectCombo, &QComboBox::currentTextChanged,
            this, &QVizObjectiveRankNode::selectAlgorithm);

    QAction* selAlgAct = widgetObj()->addActionToMenuEdit("Select Algorithm");
    connect(selAlgAct, &QAction::triggered,
            this, &QVizObjectiveRankNode::selectAlgorithmComboPopup);

}

void QVizObjectiveRankNode::selectAlgorithmComboPopup()
{
    m_algorithmSelectCombo->raise();
    m_algorithmSelectCombo->showPopup();
}

void QVizObjectiveRankNode::selectAlgorithm(const QString &setName)
{
    if(setName == m_selectAlgorithmOptions[0]) {
        m_selectedAlgorithm = ObjectiveReductionAlgorithm::L_PCA;
#ifdef CSDP_LIBRARY
        m_standardizeButton->setDisabled(true);
#endif
    } else if(setName == m_selectAlgorithmOptions[1]) {
        m_selectedAlgorithm = ObjectiveReductionAlgorithm::NL_MVU_PCA;
        m_standardizeButton->setDisabled(false);
    }
}

#ifdef CSDP_LIBRARY
void QVizObjectiveRankNode::setupStandardizeCheckbox(QString checkboxStyle)
{
    m_standardizeButton = new QCheckBox("Standardize",
                                                 widgetObj()->centralWidget());
    m_standardizeButton->setToolTip("Makes the values of each objective data have "
                                  "unit-variance\n Only for NL-MVU-PCA");
    m_standardizeButton->setChecked(true);
    m_standardizeButton->setStyleSheet(checkboxStyle);
    m_standardizeButton->setDisabled(true);
    widgetObj()->addItemToToolBar(m_standardizeButton);
    connect(m_standardizeButton, &QCheckBox::toggled,
            this, &QVizObjectiveRankNode::standardizeCheckboxUpdate);
}

void QVizObjectiveRankNode::standardizeCheckboxUpdate(bool s)
{
    m_standardize = s;
}
#endif

void QVizObjectiveRankNode::runObjectiveReduction()
{
    ISet* pop;
    if(isInSync()) {
        pop = displayedSet();
    } else {
        pop = IPSetObj()->set(0);
    }

    ISet* evaluatedSolutions = new ISet();
    for(IMappingSPtr sol: pop->all()) {
        if(sol->isEvaluated()) {
            evaluatedSolutions->append(sol);
        }
    }

    if(evaluatedSolutions->size()==0) {

        // If there are no solutions then show all objectives as
        // being equally important
        int nobj = IPSetObj()->objectiveVecSize();
        QVector<bool> S1(nobj, true);
        QVector<qreal> S2(nobj,1.0/(double)nobj);
        QVector<qreal> S3(nobj,1.0/(double)nobj);
        QVector<qreal> S4(nobj,1.0/(double)nobj);
        QMap<int, QVector<int>> S5;
        QMap<int, QVector<double>> S6;
        for(int i=0; i<nobj; i++) {
            S5[i] = QVector<int>{i};
            S6[i] = QVector<double>{1.0};
        }
        TStringList objectiveNames = IPSetObj()->objectiveVecNames();
        QStringList S7;
        for(auto elem : objectiveNames) {
            S7.append(QString::fromStdString(elem));
        }

        static_cast<ObjectiveRankWidget*>(widgetObj())->
                setObjReductionData(S1, S2, S3, S4, S5, S6, S7);
        return;
    }

    // Create a new workflow
    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    UserDefinedInit*       init = new UserDefinedInit(form);
    Evaluator*             eval = new Evaluator(init);
    IObjectiveReduction* objred;
#ifdef CSDP_LIBRARY
    switch(m_selectedAlgorithm) {
    case ObjectiveReductionAlgorithm::NL_MVU_PCA:
    {
        objred = new NLMVUPCA(eval);
        static_cast<NLMVUPCA*>(objred)->TP_defineStandardize(m_standardize);
        break;
    }
    case ObjectiveReductionAlgorithm::L_PCA:
    default:
    {
        objred = new LPCA(eval);
        break;
    }
    }
#else
    objred = new LPCA(eval);
#endif

    form->defineProblem(IPSetObj()->problem());
    init->defineInitialSet(evaluatedSolutions);

    objred->evaluate();

    TVector<bool> essentialObjectiveSet = objred->objectiveReductionData()
                                                ->essentialObjectiveSet();
    TVector<double> error = objred->objectiveReductionData()->error();
    TVector<double> variance = objred->objectiveReductionData()->variance();
    TVector<double> variancePCA =  objred->objectiveReductionData()->variancePerPCA();
    TMap<int, TVector<CorrelatedObjectives>>
            corrObj = objred->objectiveReductionData()->correlatedObjectives();
    TStringList objectiveNames = IPSetObj()->objectiveVecNames();

    QVector<bool> S1;
    for(auto elem : essentialObjectiveSet) {
        S1.append(elem);
    }

    QVector<qreal> S2;
    for(auto elem : error) {
        S2.append(elem);
    }

    QVector<qreal> S3;
    for(auto elem : variance) {
        S3.append(elem);
    }

    QVector<qreal> S4;
    for(auto elem : variancePCA) {
        S4.append(elem);
    }

    QMap<int, QVector<int>> S5;
    QMap<int, QVector<double>> S6;
    auto keys = getMapKeys(corrObj);
    for(auto key : keys) {
        TVector<CorrelatedObjectives> v = corrObj[key];
        QVector<int> v1;
        QVector<double> v2;
        for(auto elem : v) {
            v1.append(elem.objectiveIdx);
            v2.append(elem.correlationStrength);
        }
        S5[key] = v1;
        S6[key] = v2;
    }

    QStringList S7;
    for(auto elem : objectiveNames) {
        S7.append(QString::fromStdString(elem));
    }

    static_cast<ObjectiveRankWidget*>(widgetObj())->
            setObjReductionData(S1, S2, S3, S4, S5, S6, S7);

    delete objred;
    delete eval;
    delete init;
    delete form;
    delete base;
}



} // namespace Visualisation
