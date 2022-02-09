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
#include <visualisation/qvizparallelcoordinatesnode.h>
#include <visualisation/core/parallelcoordinatesplotwidget.h>
#include <visualisation/core/selectplotvarsform.h>

#include <tigon/Tigon.h>
#include <QAction>
#include <QCheckBox>
#include <QDebug>

using namespace Visualisation;
using namespace Tigon;

QvizParallelCoordinatesNode::QvizParallelCoordinatesNode()
    : m_toggleDispPreferencesButton(0)
{
    setSvg(":/visualisation/images/qvizparallelcoordinatesnode.svg");
}

QvizParallelCoordinatesNode::~QvizParallelCoordinatesNode()
{

}

void QvizParallelCoordinatesNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    ParallelCoordinatesPlotWidget* widget = new ParallelCoordinatesPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QvizParallelCoordinatesNode::show()
{
    ParallelCoordinatesPlotWidget* widget = new ParallelCoordinatesPlotWidget;

    initialiseVizNode(widget);

    showWidget();
}

void QvizParallelCoordinatesNode::customiseWidget(VisualisationWidget* widget)
{
    extractGoalsAndThresholds();

    ParallelCoordinatesPlotWidget* sWidget =
            static_cast<ParallelCoordinatesPlotWidget*>(widget);

    connect(sWidget->varsSelectForm(),
            &SelectPlotVarsForm::RobustnessIndicatorsUpdated,
            this, &QvizParallelCoordinatesNode::updateRobustness);

    connect(sWidget, &VisualisationWidget::brushedBoundsUpdated,
            this, &QvizParallelCoordinatesNode::receivedBrushedBounds);

    setupSaveBrushedSolutionsFileOption();
    setupSaveAlllSolutionsFileOption();
    setupSelectVariablesEditOptions();

    setupZoomToBrushedButton();

    connect(sWidget, &VisualisationWidget::brushedIndicesUpdated,
            this, &QvizParallelCoordinatesNode::resetBrushedButton);

    sWidget->addSpacerToToolBar();

    QString checkboxStyle = "QCheckBox {color: white}";
    setupDominatedCheckbox(checkboxStyle);
    setupInfeasibleCheckbox(checkboxStyle);
    setupNonPertientCheckbox(checkboxStyle);
    setupShowPrefsCheckbox(checkboxStyle);
    setupRecordGoalsCheckbox(checkboxStyle);
    setupInSyncCheckbox(checkboxStyle);

    sWidget->addSpacerToToolBar();
    setupSetSelection();
    sWidget->addSpacerToToolBar();
    setupTimedTracking();
    sWidget->addSpacerToToolBar();
    setupIterationTracking();
    sWidget->addSpacerToToolBar();
    setupIterationSelection();

}

ParallelCoordinatesPlotWidget* QvizParallelCoordinatesNode::widgetObj() const
{
    return static_cast<ParallelCoordinatesPlotWidget*>(QVizNode::widgetObj());
}

void QvizParallelCoordinatesNode::setupShowPrefsCheckbox(QString checkboxStyle)
{
    m_toggleDispPreferencesButton =
            new QCheckBox("Show Preferences", widgetObj()->centralWidget());
    m_toggleDispPreferencesButton->setChecked(true);
    m_toggleDispPreferencesButton->setStyleSheet(checkboxStyle);
    widgetObj()->addItemToToolBar(m_toggleDispPreferencesButton);
    connect(m_toggleDispPreferencesButton, &QCheckBox::toggled,
            widgetObj(), &ParallelCoordinatesPlotWidget::setDisplayPreferences);
}

void QvizParallelCoordinatesNode::updateRobustness(const QStringList &indicators,
                                const QVector<qreal> &parameters)
{
    QVariantList sv = widgetObj()->selectedIndices();

    int nRows = m_elemData.size();
    int nCols = widgetObj()->allNames().size();
    TVector<OptimizationType> otypes = IPSetObj()->objectiveVecOptimizationTypes();
    TVector<OptimizationType> ctypes = IPSetObj()->constraintVecOptimizationTypes();
    TVector<OptimizationType> utypes = IPSetObj()->unusedVecOptimizationTypes();

    QVector<QVector<qreal> > outputdata(nRows);
    // Create a boxplot data of m x n x 5 tensor.
    // m is the number of data points,
    // n is the number of selected objectives/constraints
    // and 5 is the number of boxplot bars (2%, 25%, 50%, 75%, 98%).
    QVector<QVector<QVector<qreal> > >
            boxplotdata(nRows, QVector<QVector<qreal> >(nCols, QVector<qreal>(5)));

    bool hidePerferences = false;
    for(int i=0; i<nRows; i++) {
        outputdata[i].resize(nCols);

        for(int j=0; j<nCols; j++) {
            IElementSPtr cElem = m_elemData[i][j];
            qreal outputdata_j = cElem->value<qreal>();
            QString indicatorType;
            if(cElem->dist() != Q_NULLPTR) {
                // Stochastic variable
                int idx = sv.indexOf(QVariant::fromValue(j));
                if(idx>-1) {
                    indicatorType = indicators[idx];
                    if(indicatorType == QString(Constants::INDICATOR_CONFIDENCE)) {
                        ConfidenceIndicator indicator(parameters[idx]);
                        outputdata_j = indicator.evaluate(cElem->dist());
                    } else if(indicatorType == QString(Constants::INDICATOR_THRESHOLD)) {
                        ThresholdIndicator indicator(parameters[idx]);
                        outputdata_j = indicator.evaluate(cElem->dist());
                        hidePerferences = true;
                    } else {
                        // No proper indicator defined
                        outputdata_j = cElem->value<qreal>();
                    }
                }
            }

            // Check types
            OptimizationType type = NonOptimization;
            // default ind is -1
            if(dataMaps()[j].m_objInd > -1) {
                // Objs
                type = otypes[dataMaps()[j].m_objInd];
            } else if(dataMaps()[j].m_constInd > -1) {
                // Cnstr
                type = ctypes[dataMaps()[j].m_constInd];
            } else if(dataMaps()[j].m_unusedInd > -1) {
                // Unused
                type = utypes[dataMaps()[j].m_unusedInd];
            }
            if(type == Maximization) {
                outputdata_j = -outputdata_j;
            }

            outputdata[i][j] = outputdata_j;
            if(indicatorType == QString(Constants::INDICATOR_THRESHOLD)) {
                boxplotdata[i][j].fill(outputdata_j, 5);
            } else {
                boxplotdata[i][j] = getBoxplotEntry(cElem, type);
            }
        }
    }
    // Hide preferences if Threshold Indicator is applied
    if(hidePerferences && m_toggleDispPreferencesButton->isChecked()) {
        m_toggleDispPreferencesButton->setChecked(false);
    }
    widgetObj()->setData(outputdata);
    widgetObj()->setBoxPlotData(boxplotdata);
}

void QvizParallelCoordinatesNode::setData()
{
    QVizNode::setData();
    setBoxPlotData();
}

void QvizParallelCoordinatesNode::setBoxPlotData()
{
    if(!displayedSet()) {
        return;
    }

    int nSolutions   = displayedSet()->size();
//    int nElements    = dataMaps().size();

    m_elemData.resize(nSolutions);
//    m_elemData.fill(QVector<IElementSPtr>(nElements), nSolutions);
    QVector<QVector<QVector<qreal>>> boxplotdata(nSolutions);
    TVector<OptimizationType> otypes = IPSetObj()->objectiveVecOptimizationTypes();
    TVector<OptimizationType> ctypes = IPSetObj()->constraintVecOptimizationTypes();
    TVector<OptimizationType> utypes = IPSetObj()->unusedVecOptimizationTypes();

    // Populate the data
    for(auto dataMap : dataMaps()) {
        if(dataMap.m_objInd >= 0) {
            // Objective
            int ind = dataMap.m_objInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = displayedSet()->at(i);
                IElementSPtr elem = imap->objectiveVar(ind);
                m_elemData[i].push_back(elem);
                QVector<qreal> boxdata = getBoxplotEntry(elem, otypes[ind]);
                boxplotdata[i].push_back(boxdata);
            }

        } else if(dataMap.m_constInd >= 0) {
            // Constraint
            int ind = dataMap.m_constInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = displayedSet()->at(i);
                IElementSPtr elem = imap->constraintVar(ind);
                m_elemData[i].push_back(elem);
                QVector<qreal> boxdata = getBoxplotEntry(elem, ctypes[ind]);
                boxplotdata[i].push_back(boxdata);
            }

        } else if(dataMap.m_unusedInd >= 0) {
            // Unused
            int ind = dataMap.m_unusedInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = displayedSet()->at(i);
                IElementSPtr elem = imap->unusedVar(ind);
                m_elemData[i].push_back(elem);
                QVector<qreal> boxdata = getBoxplotEntry(elem, NonOptimization);
                boxplotdata[i].push_back(boxdata);
            }

        } else if(dataMap.m_varInd >= 0) {
            // Decision variables
            int ind = dataMap.m_varInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = displayedSet()->at(i);
                IElementSPtr elem = imap->decisionVar(ind);
                m_elemData[i].push_back(elem);
                QVector<qreal> boxdata = getBoxplotEntry(elem, NonOptimization);
                boxplotdata[i].push_back(boxdata);
            }

        } else if(dataMap.m_paramInd >= 0) {
            // Parameters
            int ind = dataMap.m_paramInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = displayedSet()->at(i);
                IElementSPtr elem = imap->parameterVar(ind);
                m_elemData[i].push_back(elem);
                QVector<qreal> boxdata = getBoxplotEntry(elem, NonOptimization);
                boxplotdata[i].push_back(boxdata);
            }

        } else {
            qDebug() << "Error: data variable has no scope";
            return;
        }
    }

    widgetObj()->setBoxPlotData(boxplotdata);
}

QVector<qreal> QvizParallelCoordinatesNode::getBoxplotEntry(
        IElementSPtr elm, OptimizationType type)
{
    QVector<qreal> boxdata(5);
    IDistribution* dist = elm->dist();
    if(dist == Q_NULLPTR) {
        // Deterministic
        if(type == Maximization) {
            boxdata.fill(-elm->value<qreal>(), 5);
        } else {
            boxdata.fill(elm->value<qreal>(), 5);
        }

    } else {
        if(type == Maximization) {
            boxdata[4] = -dist->percentile(0.02);
            boxdata[3] = -dist->percentile(0.25);
            boxdata[2] = -dist->median();
            boxdata[1] = -dist->percentile(0.75);
            boxdata[0] = -dist->percentile(0.98);
        } else {
            boxdata[0] = dist->percentile(0.02);
            boxdata[1] = dist->percentile(0.25);
            boxdata[2] = dist->median();
            boxdata[3] = dist->percentile(0.75);
            boxdata[4] = dist->percentile(0.98);
        }
    }
    return boxdata;
}
