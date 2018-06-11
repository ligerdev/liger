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
#include <visualisation/qviznode.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/visualisationwidget.h>
#include <designer/designeditorw.h>
#include <designer/ldesigner/ldesigner.h>

// Tigon
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Indicators/ConfidenceIndicator.h>
#include <tigon/Representation/Indicators/ThresholdIndicator.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/JsonUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Problems/Problem.h>

// Json
#include <json/json.h>

// Qt
#include <QMenu>
#include <QLabel>
#include <QString>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTimer>
#include <QDebug>

using namespace Designer;
using namespace Tigon;
using namespace Tigon::Representation;

namespace Visualisation {
const QString ButtonShowAll("Show All");
const QString ButtonZoomToBrushed("Zoom to Brushed");

QVizNode::QVizNode()
    : m_widget(0)
    , m_ipset(0)
    , m_selectedSet(0)
    , m_filteredSet(new ISet)
    , m_displayedSet(0)
    , m_selectSetsCombo(0)
    , m_toggleBrushedButton(0)
    , m_toggleDominatedButton(0)
    , m_toggleFeasibleButton(0)
    , m_togglePertinentButton(0)
    , m_toggleDispPreferencesButton(0)
    , m_isInSync(false)
    , m_recordGoals(false)
    , m_timer(new QTimer)
    , m_refreshRequested (false)
{
    m_text         = QString("Viz");
    m_procType     = SINK_NODE;

    setNumberOfOutputs(1);
    setNumberOfInputs(1);

    setSvg(":/visualisation/images/default_qviznode.svg");

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();

    connect(m_timer, &QTimer::timeout, this, &QVizNode::refreshPlotRequested);
    connect(this, &QVizNode::nodeStateChangedSignal, this, &QVizNode::refreshPlotRequested);

    m_selectSetsOptions << "Main Optimization Set"
                        << "Archived Non-Dominated Solutions";
}

QVizNode::~QVizNode()
{
    disconnect(this, 0, 0, 0);
    delete m_timer;
    delete m_widget;
    delete m_filteredSet;
}

void QVizNode::refreshPlotRequested()
{
    m_refreshRequested = true;
}
bool QVizNode::isInSync() const
{
    return m_isInSync;
}

bool QVizNode::isWidgetVisible() const
{
    if(m_widget) {
        return m_widget->isVisible();
    } else {
        return false;
    }
}

void QVizNode::setupInteractions(IProcessNode *node)
{
    QVizNode* receiver = dynamic_cast<QVizNode*>(node);
    if(receiver) {
        connect(this, &QVizNode::requestToUpdateDisplayedSolutions,
                receiver, &QVizNode::autoBrushPlot);
        connect(this, &QVizNode::requestToReset,
                receiver, &QVizNode::refreshPlotRequested);
        qDebug() << this->metaObject()->className()
                 << "linked with "
                 << node->metaObject()->className();
    }
}


void QVizNode::dataUpdatedByEngine()
{
    refreshPlot();
}

void QVizNode::autoBrushPlot(const QMap<QString, QVector<double>> &dataMap)
{
    if(m_widget && m_widget->isVisible() && m_isInSync) {

        // Get all names for its own
        QStringList allNames = m_widget->allNames();

        // Get all names from connected nodes
        QStringList names = dataMap.keys();

        // Take the current filtered set.
        TVector<IMappingSPtr> mappings = m_filteredSet->all();
        //m_filteredSet->define(mappings);
        for(int i=0; i<names.size(); i++) {
            QString name = names[i];
            int idx = allNames.indexOf(name);

            if(idx > -1) {
                DataMap d = m_dataMaps[idx];
                QVector<double> vec = dataMap[name];

                for(const IMappingSPtr &imap : mappings) {
                    if(d.m_objInd > -1) {
                        double val = imap->objectiveVar(d.m_objInd)->value();
                        if(!vec.contains(val)) {
                            qDebug() << "remove obj";
                            m_filteredSet->remove(imap);
                        }

                    } else if(d.m_constInd > -1) {
                        double val = imap->constraintVar(d.m_constInd)->value();

                        if(!vec.contains(val)) {
                            qDebug() << "remove constr";
                            m_filteredSet->remove(imap);
                        }

                    } else if(d.m_unusedInd > -1) {
                        double val = imap->unusedVar(d.m_unusedInd)->value();

                        if(!vec.contains(val)) {
                            qDebug() << "remove m_unused";
                            m_filteredSet->remove(imap);
                        }

                    } else if(d.m_varInd > -1) {
                        double val = imap->decisionVar(d.m_varInd)->value();

                        if(!vec.contains(val)) {
                            qDebug() << "remove var";
                            m_filteredSet->remove(imap);
                        }

                    } else if(d.m_paramInd > -1) {
                        double val = imap->parameterVar(d.m_paramInd)->value();

                        if(!vec.contains(val)) {
                            qDebug() << "remove param  ";
                            m_filteredSet->remove(imap);
                        }
                    }
                }
            }

        }

        // Display the set
        m_displayedSet = m_filteredSet;
        qDebug() << "m_filteredSet->size(): " << m_filteredSet->size();

        ///\todo Need to figure out how to handle the empty display set.
        ///
        setData();
    }
}

void QVizNode::refreshPlot()
{
    if( isRefreshable() )
    {
        m_ipset = static_cast<IPSet*>(data());
        if(m_ipset) {
            m_selectedSet = m_ipset->set(0);
            m_displayedSet = m_selectedSet;

            if(m_displayedSet) {
                if(m_widget->allNames().size() == 0){
                    setDataMaps();
                    setSelectedIndices();
                    setData();
                    extractNamesAndCategories();
                    extractGoalsAndThresholds();
                    setSelectSetsList();
                    customiseWidget(m_widget);
                    m_widget->reloadView();
                } else {
                    applyFilters();
                }
            }
        }
        m_refreshRequested = false;
    }
}

void QVizNode::initialiseVizNode(VisualisationWidget* widget)
{
    setWidget(widget);

    m_ipset = static_cast<IPSet*>(data());
    if(m_ipset) {
        m_selectedSet = m_ipset->set(0);
        m_displayedSet = m_selectedSet;
    }

    if(m_displayedSet) {
        setDataMaps();
        setSelectedIndices();
        setData();
        extractNamesAndCategories();
        extractGoalsAndThresholds();
        setSelectSetsList();
        customiseWidget(widget);
    }
}

void QVizNode::setWidget(VisualisationWidget *widget)
{
    if(m_widget != widget) {
        m_timer->stop();
        if(m_widget) {
            disconnect(m_widget, 0, 0, 0);
            delete m_widget;
        }

        m_widget = widget;
        connect(m_widget, &VisualisationWidget::brushedBoundsUpdated,
                this, &QVizNode::receivedBrushedBounds);
        connect(static_cast<QObject*>(m_widget), &QObject::destroyed,
                m_timer, &QTimer::stop);
        connect(static_cast<QObject*>(m_widget), &QObject::destroyed,
                this, &QVizNode::resetWidget);
        m_widget->setAttribute(Qt::WA_DeleteOnClose);

        // Tools
        QAction* saveBrushedSols =
                m_widget->addActionToMenuFile(tr("Save Brushed Solutions"));
        connect(saveBrushedSols, &QAction::triggered,
                this, &QVizNode::saveBrushedSolutions);

        QAction* saveAllSols =
                m_widget->addActionToMenuFile(tr("Save All Solutions"));
        connect(saveAllSols, &QAction::triggered, this, &QVizNode::saveAllSolutions);

        QAction* selSetAct = m_widget->addActionToMenuEdit("Select Set to Display");
        connect(selSetAct, &QAction::triggered, this, &QVizNode::selectSetsComboPopup);

        m_toggleBrushedButton = new QPushButton(ButtonZoomToBrushed, m_widget->centralWidget());
        m_widget->addItemToToolBar(m_toggleBrushedButton);
        connect(m_toggleBrushedButton, &QPushButton::clicked,
                this, &QVizNode::filterBrushedSolutions);

        connect(m_widget, &VisualisationWidget::brushedIndicesUpdated,
                this, &QVizNode::resetBrushedButton);

        // Checkboxes
        QString checkboxStyle = "QCheckBox {color: white}";

        m_toggleDominatedButton =
                new QCheckBox("Dominated", m_widget->centralWidget());
        m_toggleDominatedButton->setChecked(true);
        m_toggleDominatedButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(m_toggleDominatedButton);
        connect(m_toggleDominatedButton, &QCheckBox::toggled,
                this, &QVizNode::applyFilters);

        m_toggleFeasibleButton =
                new QCheckBox("Infeasible", m_widget->centralWidget());
        m_toggleFeasibleButton->setChecked(true);
        m_toggleFeasibleButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(m_toggleFeasibleButton);
        connect(m_toggleFeasibleButton, &QCheckBox::toggled,
                this, &QVizNode::filterFeasible);

        m_togglePertinentButton =
                new QCheckBox("Non Pertinent", m_widget->centralWidget());
        m_togglePertinentButton->setChecked(true);
        m_togglePertinentButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(m_togglePertinentButton);
        connect(m_togglePertinentButton, &QCheckBox::toggled,
                this, &QVizNode::filterPertinent);

        m_toggleDispPreferencesButton =
                new QCheckBox("Show Preferences", m_widget->centralWidget());
        m_toggleDispPreferencesButton->setChecked(true);
        m_toggleDispPreferencesButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(m_toggleDispPreferencesButton);
        connect(m_toggleDispPreferencesButton, &QCheckBox::toggled,
                m_widget, &VisualisationWidget::setDisplayPreferences);


        QCheckBox* recordGoalsButton = new QCheckBox("Record Goals", m_widget->centralWidget());
        recordGoalsButton->setChecked(false);
        recordGoalsButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(recordGoalsButton);
        connect(recordGoalsButton, &QCheckBox::toggled,
                this, &QVizNode::recordGoals);

        QCheckBox* inSyncButton = new QCheckBox("InSync", m_widget->centralWidget());
        inSyncButton->setChecked(false);
        inSyncButton->setStyleSheet(checkboxStyle);
        m_widget->addItemToToolBar(inSyncButton);
        connect(inSyncButton, &QCheckBox::toggled,
                this, &QVizNode::inSyncCheckboxUpdate);

        m_widget->addSpacerToToolBar();

        m_selectSetsCombo = new QComboBox(m_widget->centralWidget());
        m_widget->addItemToToolBar(m_selectSetsCombo);

        connect(m_selectSetsCombo, &QComboBox::currentTextChanged,
                this, &QVizNode::selectSetsToDisplay);

        // Tracking
        m_widget->addSpacerToToolBar();
        QLabel* trackingLabel = new QLabel("Tracking: ", m_widget->centralWidget());
        m_widget->addItemToToolBar(trackingLabel);
        QComboBox* trackingOptions = new QComboBox(m_widget->centralWidget());
        m_widget->addItemToToolBar(trackingOptions);
        QStringList items({"", "1s", "5s", "10s", "1min", "5min"});
        trackingOptions->insertItems(0, items);
        connect(trackingOptions, &QComboBox::currentTextChanged,
                this, &QVizNode::updateTrackingInterval);
    }
}

void QVizNode::showWidget() const
{
    if(m_widget) {
        m_widget->show();
        m_widget->raise();
    }
}

void QVizNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void QVizNode::updateGoal(int index, qreal lb, qreal ub)
{
    if(index >= m_dataMaps.size()) {
        return;
    }

    int idxIPset = m_dataMaps.at(index).m_objInd;
    if(idxIPset > -1) {
        OptimizationType type = m_ipset->objectiveVecProperties()[idxIPset]
                .optimizationType();
        if(type == Minimization) {
            m_ipset->defineGoal(idxIPset, IElementSPtr(new IElement(ub)));
        } else if(type == Maximization) {
            m_ipset->defineGoal(idxIPset, IElementSPtr(new IElement(-lb)));
        }
        qDebug() << "Goal defined in ipset for index "
                 << idxIPset
                 << ": [" << m_ipset->goalVec()[idxIPset]->value() << "]";
    }
}

void QVizNode::saveBrushedSolutions()
{
    QString file = getSaveFileName("Save Brushed Solutions");
    if(file.isEmpty()) {
        return;
    }

    filterBrushedSolutions();

    saveToFile(file, m_filteredSet);
}

void QVizNode::saveAllSolutions()
{
    QString file = getSaveFileName();
    if(file.isEmpty()) {
        return;
    }

    saveToFile(file, m_selectedSet);
}

void QVizNode::selectSetsToDisplay(const QString &setName)
{
    if(setName == m_selectSetsOptions[0]) {
        setSelectedSet(m_ipset->setWithTag(Tigon::TMainOptimizationSet));
    } else if(setName == m_selectSetsOptions[1]) {
        setSelectedSet(m_ipset->setWithTag(Tigon::TNonDominatedArchive));
    }
}

void QVizNode::selectSetsComboPopup()
{
    m_selectSetsCombo->raise();
    m_selectSetsCombo->showPopup();
}

void QVizNode::updateRobustness(const QStringList &indicators,
                                const QVector<qreal> &parameters)
{
    QVariantList sv = m_widget->selectedIndices();

    int nRows = m_elemData.size();
    int nCols = m_widget->allNames().size();
    TVector<OptimizationType> otypes = m_ipset->objectiveVecOptimizationTypes();
    TVector<OptimizationType> ctypes = m_ipset->constraintVecOptimizationTypes();
    TVector<OptimizationType> utypes = m_ipset->unusedVecOptimizationTypes();

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
            if(m_dataMaps[j].m_objInd > -1) {
                // Objs
                type = otypes[m_dataMaps[j].m_objInd];
            } else if(m_dataMaps[j].m_constInd > -1) {
                // Cnstr
                type = ctypes[m_dataMaps[j].m_constInd];
            } else if(m_dataMaps[j].m_unusedInd > -1) {
                // Unused
                type = utypes[m_dataMaps[j].m_unusedInd];
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
    m_widget->setData(outputdata);
    m_widget->setBoxPlotData(boxplotdata);
}

bool QVizNode::isRefreshable()
{
    if (m_widget && m_widget->isVisible()){
        return m_refreshRequested;
    } else {
        return false;
    }
}

void QVizNode::receivedBrushedBounds(int index, qreal lb, qreal ub)
{
    if(m_recordGoals) {
        updateGoal(index, lb, ub);
    }
}

void QVizNode::filterBrushedSolutions()
{
    if(m_toggleBrushedButton->text() == ButtonZoomToBrushed) {
        updateFilteredSetFromBrushing();
        m_toggleBrushedButton->setText(ButtonShowAll);
        displaySet(m_filteredSet);

        if(m_isInSync) {
            // Sync with linked nodes
            syncNetworkBrushing();
        }
    } else {
        resetButtons();
        if(m_isInSync) {
            // Sync with linked nodes
            emit requestToReset();
        }
    }

    if(m_isInSync) {
        // Sync with other local plots
        syncBrushing();
    }
}

void QVizNode::applyFilters()
{
    displaySet(m_selectedSet);
    if (!m_toggleDominatedButton->isChecked()) filterNonDominated();
    if (m_toggleFeasibleButton->isChecked()) filterFeasible();
    if (!m_togglePertinentButton->isChecked()) filterPertinent();
}
void QVizNode::filterNonDominated()
{
    setFilteredSet(Tigon::nonDominatedSet(m_displayedSet));
}

void QVizNode::filterFeasible()
{
    ISet* onlyFeasible = new ISet;
    for(int i = 0; i < m_displayedSet->size(); i++) {
        if(m_displayedSet->at(i)->isFeasible()) {
            onlyFeasible->append(m_displayedSet->at(i));
        }
    }
    setFilteredSet(onlyFeasible);
}

void QVizNode::filterPertinent()
{
    ISet* onlyPertinent = new ISet;
    for(int i = 0; i < m_displayedSet->size(); i++) {
        if(m_displayedSet->at(i)->isPertinent()) {
            onlyPertinent->append(m_displayedSet->at(i));
        }
    }
    setFilteredSet(onlyPertinent);
}

void QVizNode::resetBrushedButton()
{
    m_toggleBrushedButton->setText(ButtonZoomToBrushed);
}

void QVizNode::updateTrackingInterval(const QString &interval)
{
    if(interval.isEmpty()) {
        if(m_timer->isActive()) {
            m_timer->stop();
        }
        return;
    }

    int timeInterval = 0;
    if(interval == "1s") {
        timeInterval = 1000;
    }

    if(interval == "5s") {
        timeInterval = 5000;
    }

    if(interval == "10s") {
        timeInterval = 10000;
    }

    if(interval == "1min") {
        timeInterval = 60*1000;
    }

    if(interval == "5min") {
        timeInterval = 5*60*1000;
    }

    m_timer->setInterval(timeInterval);
    m_timer->start();
}

void QVizNode::resetWidget()
{
    m_widget = nullptr;
}

void QVizNode::inSyncCheckboxUpdate(bool insync)
{
    m_isInSync = insync;
    qDebug() << "Insync status: " << m_isInSync;
}

void QVizNode::recordGoals(bool toggled)
{
    m_recordGoals = toggled;
}

QString QVizNode::getSaveFileName(const QString& title)
{
    QString file = QFileDialog::getSaveFileName(0, title, QDir::homePath(),
                                                QString("Json (*.json);;"
                                                        "Text (*.txt);;"));
    return file;
}

bool QVizNode::saveToFile(const QString &file,
                          ISet* solset)
{
    if(!m_ipset) {
        return false;
    }
    QFile ofile(file);
    if(!ofile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QFileInfo finfo(file);
    QString suffix = finfo.suffix();
    if(suffix == "txt") {
        saveAsTxtFormat(&ofile, solset);
    } else if(suffix == "json") {
        saveAsJsonFormat(&ofile, solset);
    }

    ofile.close();
    return true;
}

void QVizNode::setDataMaps()
{
    int nObjectives  = m_ipset->objectiveVecSize();
    int nConstraints = m_ipset->constraintVecSize();
    int nUnused = m_ipset->unusedVecSize();
    int nVariables   = m_ipset->decisionVecSize();
    int nParameters = m_ipset->parameterVecSize();

    // find which objectives are also constraints
    // index of the the constraint shared with each objective
    QVector<int>  sharedObj(nObjectives, -1);
    QVector<bool> sharedConst(nConstraints, false);
    Tigon::TStringList objNames = m_ipset->objectiveVecNames();
    Tigon::TStringList constNames = m_ipset->constraintVecNames();
    for(int i=0; i<nObjectives; i++) {
        for(int j=0; j<nConstraints; j++) {
            if(!sharedConst[j] && (objNames[i]==constNames[j])) {
                sharedObj[i] = j;
                sharedConst[j] = true;
                break;
            }
        }
    }

    // create the maps from each element of the data to the corresponding
    // vector(s). -1 is not of this type.
    m_dataMaps.clear();
    for(int i=0; i<nObjectives; i++) {
        if(sharedObj[i] < 0) {
            m_dataMaps.append(DataMap(i, -1, -1, -1, -1));
        }
    }

    for(int i=0; i<nObjectives; i++) {
        if(sharedObj[i] >= 0) {
            m_dataMaps.append(DataMap(i, sharedObj[i], -1, -1, -1));
        }
    }

    for(int i=0; i<nConstraints; i++) {
        if(!sharedConst[i]) {
            m_dataMaps.append(DataMap(-1, i, -1, -1, -1));
        }
    }

    for(int i=0; i<nUnused; i++) {
        m_dataMaps.append(DataMap(-1, -1, i, -1, -1));
    }

    for(int i=0; i<nVariables; i++) {
        m_dataMaps.append(DataMap(-1, -1, -1, i, -1));
    }

    for(int i=0; i<nParameters; i++) {
        if(m_ipset->problem()->isExternalParameters()[i] ) {
            m_dataMaps.append(DataMap(-1, -1, -1, -1, i));
        }
    }
}

void QVizNode::setSelectedIndices()
{
    // Set default selected indices to be objectives and/or constraints
    QVariantList selectedIndices;
    for(int i = 0; i < m_dataMaps.size(); i++) {
        if((m_dataMaps[i].m_objInd >= 0) || (m_dataMaps[i].m_constInd >= 0)) {
            selectedIndices.append(QVariant::fromValue(i));
        }
    }
    m_widget->setSelectedIndices(selectedIndices);
}

void QVizNode::setData()
{
    if(m_displayedSet) {
        int nSolutions   = m_displayedSet->size();
        int nElements    = m_dataMaps.size();

        m_elemData.fill(QVector<IElementSPtr>(nElements), nSolutions);
        QVector<QVector<qreal> > outputData(nSolutions, QVector<qreal>(nElements));
        QVector<QVector<QVector<qreal> > > boxplotdata(
                    nSolutions, QVector<QVector<qreal> >(
                        nElements, QVector<qreal>(5)));

        // Populate the data
        for(int j = 0; j < nElements; j++) {
            if(m_dataMaps[j].m_objInd >= 0) {
                // objectives
                TVector<OptimizationType> otypes = m_ipset->objectiveVecOptimizationTypes();
                int ind = m_dataMaps[j].m_objInd;
                for(int i = 0; i < nSolutions; i++) {
                    IMappingSPtr imap = m_displayedSet->at(i);
                    IElementSPtr elem = imap->objectiveVar(ind);
                    m_elemData[i][j] = elem;
                    qreal value = elem->value<qreal>();
                    // Account for the optimization types
                    if(otypes[ind] == Maximization) {
                        value = -value;
                    }
                    outputData[i][j] = value;
                    boxplotdata[i][j] = getBoxplotEntry(elem, otypes[ind]);
                }

            } else if(m_dataMaps[j].m_constInd >= 0) {
                // Consraints
                TVector<OptimizationType> ctypes = m_ipset->constraintVecOptimizationTypes();
                int ind = m_dataMaps[j].m_constInd;
                for(int i = 0; i < nSolutions; i++) {
                    IMappingSPtr imap = m_displayedSet->at(i);
                    IElementSPtr elem = imap->constraintVar(ind);
                    m_elemData[i][j] = elem;
                    qreal value = elem->value<qreal>();
                    // Account for the optimization types
                    if(ctypes[ind] == Maximization) {
                        value = -value;
                    }
                    outputData[i][j] = value;
                    boxplotdata[i][j] = getBoxplotEntry(elem, ctypes[ind]);
                }

            } else if(m_dataMaps[j].m_unusedInd >= 0) {
                // Unused
                TVector<OptimizationType> utypes = m_ipset->unusedVecOptimizationTypes();
                int ind = m_dataMaps[j].m_unusedInd;
                for(int i = 0; i < nSolutions; i++) {
                    IMappingSPtr imap = m_displayedSet->at(i);
                    IElementSPtr elem = imap->unusedVar(ind);
                    m_elemData[i][j] = elem;
                    qreal value = elem->value<qreal>();
                    // Account for the optimization types
                    if(utypes[ind] == Maximization) {
                        value = -value;
                    }
                    outputData[i][j] = value;
                    boxplotdata[i][j] = getBoxplotEntry(elem, NonOptimization);
                }

            } else if(m_dataMaps[j].m_varInd >= 0) {
                // Decision variables
                int ind = m_dataMaps[j].m_varInd;
                for(int i = 0; i < nSolutions; i++) {
                    IMappingSPtr imap = m_displayedSet->at(i);
                    IElementSPtr elem = imap->decisionVar(ind);
                    m_elemData[i][j] = elem;
                    qreal value = elem->value<qreal>();
                    outputData[i][j] = value;
                    boxplotdata[i][j] = getBoxplotEntry(elem, NonOptimization);
                }

            } else if(m_dataMaps[j].m_paramInd >= 0) {
                // Parameters
                int ind = m_dataMaps[j].m_paramInd;
                for(int i = 0; i < nSolutions; i++) {
                    IMappingSPtr imap = m_displayedSet->at(i);
                    IElementSPtr elem = imap->parameterVar(ind);
                    m_elemData[i][j] = elem;
                    double value = elem->value();
                    outputData[i][j] = value;
                    boxplotdata[i][j] = getBoxplotEntry(elem, NonOptimization);
                }

            } else {
                qDebug() << "Error: data variable has no scope";
                return;
            }
        }

        m_widget->setData(outputData);
        m_widget->setBoxPlotData(boxplotdata);
    }
}

void QVizNode::extractNamesAndCategories()
{
    QStringList allNames;
    QVariantList categories;

    Tigon::TStringList objNames     = m_ipset->objectiveVecNames();
    Tigon::TStringList constNames   = m_ipset->constraintVecNames();
    Tigon::TStringList unusedNames  = m_ipset->unusedVecNames();
    Tigon::TStringList varNames     = m_ipset->decisionVecNames();
    Tigon::TStringList paramNames   = m_ipset->parameterVecNames();

    int nElements = m_dataMaps.size();

    // Set the catecories: 0.obj, 1.obj+const, 2.const, 3.unused, 4.var, 5.param
    for(int i=0; i<nElements; i++) {
        if(m_dataMaps[i].m_objInd >= 0) {
            int oInd = m_dataMaps[i].m_objInd;
            int cInd = m_dataMaps[i].m_constInd;
            allNames.append(QString::fromStdString(objNames[oInd]));
            if(cInd < 0) {
                // objective
                categories.append(QVariant::fromValue(0));
            } else {
                // objective and constraint
                categories.append(QVariant::fromValue(1));
            }
        } else if(m_dataMaps[i].m_constInd >= 0) {
            // constraint
            int ind = m_dataMaps[i].m_constInd;
            categories.append(QVariant::fromValue(2));
            allNames.append(QString::fromStdString(constNames[ind]));
        } else if(m_dataMaps[i].m_unusedInd >= 0) {
            // unused
            int ind = m_dataMaps[i].m_unusedInd;
            categories.append(QVariant::fromValue(3));
            allNames.append(QString::fromStdString(unusedNames[ind]));
        } else if(m_dataMaps[i].m_varInd >= 0) {
            // decision variable
            int ind = m_dataMaps[i].m_varInd;
            allNames.append(QString::fromStdString(varNames[ind]));
            categories.append(QVariant::fromValue(4));
        } else if(m_dataMaps[i].m_paramInd >= 0) {
            // parameter
            int ind = m_dataMaps[i].m_paramInd;
            allNames.append(QString::fromStdString(paramNames[ind]));
            categories.append(QVariant::fromValue(5));
        }
    }

    m_widget->setNames(allNames);
    m_widget->setCategories(categories);
}

void QVizNode::extractGoalsAndThresholds()
{
    QVariantList goals, thresholds, prefDirections;
    TVector<IElementSPtr> goalVec      = m_ipset->goalVec();
    TVector<IElementSPtr> thresholdVec = m_ipset->thresholdVec();
    TVector<ElementProperties> oprts   = m_ipset->objectiveVecProperties();
    TVector<ElementProperties> cprts   = m_ipset->constraintVecProperties();

    int nElements = m_dataMaps.size();
    prefDirections.reserve(nElements);

    for(int i=0; i<nElements; i++) {
        if(m_dataMaps[i].m_objInd >= 0) {
            int oInd = m_dataMaps[i].m_objInd;
            int cInd = m_dataMaps[i].m_constInd;

            OptimizationType type = oprts[oInd].optimizationType();
            bool goalDefined = isGoalDefined(goalVec[oInd]);
            if(cInd < 0) {
                // objective
                thresholds.append(QVariant());

                if(type == Minimization) {
                    if(goalDefined) {
                        goals.append(QVariant::fromValue(goalVec[oInd]->value()));
                    } else {
                        goals.append(QVariant());
                    }
                    prefDirections.append(1);

                } else if(type == Maximization) {
                    if(goalDefined) {
                        goals.append(QVariant::fromValue(-goalVec[oInd]->value()));
                    } else {
                        goals.append(QVariant());
                    }
                    prefDirections.append(-1);

                } else {
                    goals.append(QVariant());
                    prefDirections.append(QVariant());
                }

            } else {
                // objective and constraint
                OptimizationType type = oprts[oInd].optimizationType();
                bool goalDefined = isGoalDefined(goalVec[oInd]);
                bool thresholdDefined = isThresholdDefined(thresholdVec[cInd]);
                if(type == Minimization) {
                    if(goalDefined) {
                        goals.append(QVariant::fromValue(goalVec[oInd]->value()));
                    } else {
                        goals.append(QVariant());
                    }
                    if(thresholdDefined) {
                        thresholds.append(QVariant::fromValue(thresholdVec[cInd]->value()));
                    } else {
                        thresholds.append(QVariant());
                    }
                    prefDirections.append(1);

                } else if(type == Maximization) {
                    if(goalDefined) {
                        goals.append(QVariant::fromValue(-goalVec[oInd]->value()));
                    } else {
                        goals.append(QVariant());
                    }
                    if(thresholdDefined) {
                        thresholds.append(QVariant::fromValue(-thresholdVec[cInd]->value()));
                    } else {
                        thresholds.append(QVariant());
                    }
                    prefDirections.append(-1);

                } else {
                    goals.append(QVariant());
                    thresholds.append(QVariant());
                    prefDirections.append(QVariant());
                }
            }
        } else if(m_dataMaps[i].m_constInd >= 0) {
            // constraint
            goals.append(QVariant());

            int ind = m_dataMaps[i].m_constInd;
            bool isThredDefined = isThresholdDefined(thresholdVec[ind]);
            OptimizationType type = cprts[ind].optimizationType();
            QVariant threshold = QVariant();
            if(type == Minimization) {
                prefDirections.append(1);
                if(isThredDefined) {
                    threshold = QVariant::fromValue(thresholdVec[ind]->value());
                }
            } else if(type == Maximization) {
                prefDirections.append(-1);
                if(isThredDefined) {
                    threshold = QVariant::fromValue(-thresholdVec[ind]->value());
                }
            } else {
                prefDirections.append(QVariant());
            }
            thresholds.append(threshold);

        } else if(m_dataMaps[i].m_varInd >= 0) {
            // decision variable
            goals.append(QVariant());
            thresholds.append(QVariant());
            prefDirections.append(QVariant());
        }
    }

    m_widget->setGoals(goals);
    m_widget->setThresholds(thresholds);
    m_widget->setPreferenceDirerctions(prefDirections);
}

void QVizNode::saveAsJsonFormat(QFile * ofile, ISet *solset)
{
    QTextStream txtOStream(ofile);
    Json::JsonArray jSet = Tigon::toJsonArray(solset);
    Json::JsonObject jObj;
    jObj[Tigon::Log::LSinglePop] = jSet;
    Json::JsonDocument jdoc(jObj);
    txtOStream << QString::fromStdString(jdoc.toJson());
}

/*!
 * \brief Save the solutions in pure txt file.
 * Format:
 *     Var1Name, ..., VarNName, Obj1Name, ..., ObjMNames
 *     Var1,     ..., VarN,     Obj1,     ..., ObjM
 * \param File handler
 * \param solution set
 */
void QVizNode::saveAsTxtFormat(QFile *ofile, ISet *solset)
{
    ///\todo Save constraints. @Yiming Modify the following code
    QTextStream txtOStream(ofile);

    int nVar = m_ipset->decisionVecSize();
    int nObj = m_ipset->objectiveVecSize();
    txtOStream << "#Var, " << nVar << "\n";
    txtOStream << "#Obj, " << nObj << "\n";
    // txtOStream << "# (Obj AND Constraint), " << ?? << "\n";
    // txtOStream << "# (Constraint Only), "    << ?? << "\n";

    // Names
    Tigon::TStringList varNames = m_ipset->decisionVecNames();
    Tigon::TStringList objNames = m_ipset->objectiveVecNames();
    txtOStream << QString::fromStdString(join(varNames, ",")
                                         + "," + join(objNames, ",")) << "\n";

    // Solutions
    int nSols = solset->size();
    for(int l=0; l<nSols; l++) {
        IMappingSPtr imap = solset->at(l);

        // Decision variables
        TVector<IElementSPtr> vars = imap->decisionVec();
        for(const IElementSPtr &var : vars) {
            txtOStream << var->value<double>() << ",";
        }

        // Objectives
        vars = imap->objectiveVec();
        int counter = 0;
        for(const IElementSPtr &var : vars) {
            OptimizationType type = m_ipset->objectiveVecProperties()[counter]
                    .optimizationType();

            double value = var->value<double>();
            if(type == Maximization) {
                value = -1.0*value;
            }
            txtOStream << value;

            if(counter != nObj-1) {
                txtOStream << ",";
            } else {
                txtOStream << "\n";
            }
            ++counter;
        }
    }
}

void QVizNode::displaySet(ISet *displayedSet)
{
    // force the filtered set to contain the solutions of the displayed set
    if(displayedSet && (displayedSet != m_filteredSet)) {
        m_filteredSet->define(displayedSet->all());
    }

    m_displayedSet = displayedSet;
    setData();

    // reset the brushed indices to the full list
    if(m_displayedSet == m_selectedSet) {
        m_widget->resetBrushedIndices();
    }
}

void QVizNode::setSelectedSet(ISet* currentPop)
{
    m_selectedSet = currentPop;
    displaySet(m_selectedSet);
}

void QVizNode::setFilteredSet(ISet* filteredSet)
{
    delete m_filteredSet;
    m_filteredSet = filteredSet;
    displaySet(m_filteredSet);
}

void QVizNode::updateFilteredSetFromBrushing()
{
    QList<int> brushedIndices = m_widget->brushedIndices();
    ISet* tmp = new ISet(m_filteredSet);
    m_filteredSet->clearMappings();

    /// \bug This is a bug: brushedIndices can contain indices that are out of
    /// the boundaries of m_filteredSet/tmp. Especially when resetBrushedIndices is
    /// called.

    /// \note ad-hoc solution: if tmp has different size as brushedIndices
    /// set m_filteredSet to the displayed set. This will stop liger from crashing.
    /// However, I believe there must exist a better resolution.
    /// \attention @shaul: I think the logic of handling filteredSet and brushed
    /// indices needs to be clarified.

    /// Ad hoc fix Starts =========
    int maxIdx = *std::max_element(brushedIndices.cbegin(), brushedIndices.cend());
    if(m_displayedSet && tmp->size() <= maxIdx) {
        qDebug() << "Ad-hoc solution. Reset m_filteredSet to m_diplayedSet";
        m_filteredSet->define(m_displayedSet->all());
        return;
    }
    /// Ad hoc fix Ends ===========

    for(int i = 0; i < brushedIndices.size(); i++) {
        m_filteredSet->append(tmp->at(brushedIndices[i]));
    }
    delete tmp;
}

void QVizNode::setSelectSetsList()
{
    if(m_selectSetsCombo && m_ipset) {
        m_selectSetsCombo->clear();

        // List of available sets to choose for display
        QStringList list;
        if(m_ipset->tagExists(Tigon::TMainOptimizationSet)) {
            list << m_selectSetsOptions[0];
        }
        if(m_ipset->tagExists(Tigon::TNonDominatedArchive)) {
            list << m_selectSetsOptions[1];
        }

        m_selectSetsCombo->addItems(list);
    }
}

void QVizNode::resetButtons()
{
    displaySet(m_selectedSet);

    resetBrushedButton();
    m_toggleDominatedButton->setChecked(true);
    m_toggleFeasibleButton->setChecked(true);
    m_togglePertinentButton->setChecked(true);
}

void QVizNode::syncBrushing()
{
    LDesignerWidget* designerWidget =
            Designer::Internal::DesignEditorW::instance()->designerEditor()->widget();

    QList<QGraphicsItem*> items = designerWidget->scene()->items();

    /// Search for all visualisation objects
    foreach(QGraphicsItem* p, items) {
        QVizNode* cnode = dynamic_cast<QVizNode*>(p);
        if(cnode && cnode != this) {
            if(cnode->isWidgetVisible() && cnode->isInSync()) {
                cnode->displaySet(m_filteredSet);
            }
        }
    }
}

void QVizNode::syncNetworkBrushing()
{
    QVector<QVector<qreal>> rawData = m_widget->rawData();
    QStringList allNames = m_widget->allNames();
    QMap<QString, QVector<double>> dataMap;

    int nSols = rawData.size();
    for(int i=0; i<allNames.size(); i++) {
        QVector<double> vec;
        vec.reserve(nSols);
        for(int j=0; j<nSols; j++) {
            vec.append(rawData[j][i]);
        }

        // remove duplications
        std::sort(vec.begin(), vec.end());
        auto last = std::unique(vec.begin(), vec.end());
        vec.erase(last, vec.end());
        dataMap[allNames[i]] = vec;
    }

    emit requestToUpdateDisplayedSolutions(dataMap);
}

QVector<qreal> QVizNode::getBoxplotEntry(IElementSPtr elm, OptimizationType type)
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

} // namespace Visualisation
