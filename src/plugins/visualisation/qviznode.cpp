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
#include <tigon/Log/LogManager.h>

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
    , m_toggleInFeasibleButton(0)
    , m_toggleNonPertinentButton(0)
    , m_isInSync(false)
    , m_recordGoals(false)
    , m_timer(new QTimer)
    , m_iterationInteruptor(new IterationInteruptor)
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
    connect(m_iterationInteruptor, &IterationInteruptor::iterationReached,
            this, &QVizNode::refreshPlotRequested);
    connect(this, &QVizNode::nodeStateChangedSignal,
            this, &QVizNode::refreshPlotRequested);

    m_selectSetsOptions << "Main Optimization Set"
                        << "Archived Non-Dominated Solutions";
}

QVizNode::~QVizNode()
{
    disconnect(this, 0, 0, 0);
    if(m_timer) delete m_timer;
    if(m_widget) delete m_widget;
    if(m_filteredSet) delete m_filteredSet;
    if(m_iterationInteruptor) delete m_iterationInteruptor;
}

IPSet* QVizNode::IPSetObj() const
{
    return m_ipset;
}

VisualisationWidget* QVizNode::widgetObj() const
{
    return m_widget;
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
    if(isInitialized() && isWidgetVisible() && isInSync()) {

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
        m_displayedSet = m_filteredSet->clone();
        qDebug() << "m_filteredSet->size(): " << m_filteredSet->size();

        ///\todo Need to figure out how to handle the empty display set.
        ///
        setData();
    }
}

void QVizNode::refreshPlot()
{
    if(isRefreshable()){
        initializeWidgetData();
        applyFilters();
        m_refreshRequested = false;

    }
    if(isInitialized() && !areVariablesToDisplaySelected()){
        initializeWidgetDisplay();
        m_widget->reloadView();
    }
    if(isWidgetVisible() && isInitialized()){
        m_iterationInteruptor->updateIteration(m_ipset->currentIteration());
    }
}

void QVizNode::initialiseVizNode(VisualisationWidget* widget)
{
    setupWidget(widget);
    initializeWidgetData();
    if(isInitialized()) {
        initializeWidgetDisplay();
    }
}

void QVizNode::initializeWidgetData()
{
    m_ipset = static_cast<IPSet*>(data());
    if(m_ipset) {
        m_selectedSet = m_ipset->set(0)->clone();
        m_displayedSet = m_selectedSet->clone();
        m_filteredSet = m_selectedSet->clone();
    }
}

void QVizNode::initializeWidgetDisplay()
{
    setDataMaps();
    extractNamesAndCategories();
    customiseWidget(m_widget);
    resetSelectedVariablesToDisplay();
}

void QVizNode::setupTimedTracking()
{
    QLabel* timedTrackingLabel = new QLabel("Tracking [sec]: ", m_widget->centralWidget());
    m_widget->addItemToToolBar(timedTrackingLabel);
    QComboBox* timedTrackingOptions = new QComboBox(m_widget->centralWidget());
    m_widget->addItemToToolBar(timedTrackingOptions);
    QStringList items({"", "1s", "5s", "10s", "1min", "5min"});
    timedTrackingOptions->insertItems(0, items);
    connect(timedTrackingOptions, &QComboBox::currentTextChanged,
            this, &QVizNode::updateTimedTrackingInterval);
}

void QVizNode::setupIterationTracking()
{
    QLabel* iterationTrackingLabel = new QLabel("Tracking [iter]: ", m_widget->centralWidget());
    m_widget->addItemToToolBar(iterationTrackingLabel);
    QComboBox* iterationTrackingOptions = new QComboBox(m_widget->centralWidget());
    m_widget->addItemToToolBar(iterationTrackingOptions);
    QStringList items({"0", "1", "5", "10", "20", "30", "50"});
    iterationTrackingOptions->insertItems(0, items);
    connect(iterationTrackingOptions, &QComboBox::currentTextChanged,
            this, &QVizNode::updateIterationTrackingInterval);
}

void QVizNode::setupIterationSelection()
{
    QLabel* iterationLabel = new QLabel("Iteration: ", m_widget->centralWidget());
    m_widget->addItemToToolBar(iterationLabel);
    m_iterationOptions = new QComboBox(m_widget->centralWidget());
    m_widget->addItemToToolBar(m_iterationOptions);
    QStringList iterations({""});
    m_iterationOptions->insertItems(0, iterations);
    connect(m_iterationOptions, &QComboBox::currentTextChanged,
            this, &QVizNode::selectIterComboPopup);

    setAvailableIterationList();
}

void QVizNode::setupSetSelection()
{
    m_selectSetsCombo = new QComboBox(m_widget->centralWidget());
    m_widget->addItemToToolBar(m_selectSetsCombo);
    connect(m_selectSetsCombo, &QComboBox::currentTextChanged,
            this, &QVizNode::selectSetsToDisplay);

    QAction* selSetAct = m_widget->addActionToMenuEdit("Select Set to Display");
    connect(selSetAct, &QAction::triggered,
            this, &QVizNode::selectSetsComboPopup);
    setSelectSetsList();
}

void QVizNode::setupInSyncCheckbox(QString checkboxStyle)
{
    QCheckBox* inSyncButton = new QCheckBox("InSync", m_widget->centralWidget());
    inSyncButton->setChecked(false);
    inSyncButton->setStyleSheet(checkboxStyle);
    m_widget->addItemToToolBar(inSyncButton);
    connect(inSyncButton, &QCheckBox::toggled,
            this, &QVizNode::inSyncCheckboxUpdate);
}

void QVizNode::setupRecordGoalsCheckbox(QString checkboxStyle)
{
    QCheckBox* recordGoalsButton = new QCheckBox("Record Goals", m_widget->centralWidget());
    recordGoalsButton->setChecked(false);
    recordGoalsButton->setStyleSheet(checkboxStyle);
    m_widget->addItemToToolBar(recordGoalsButton);
    connect(recordGoalsButton, &QCheckBox::toggled,
            this, &QVizNode::recordGoals);
}

void QVizNode::setupNonPertientCheckbox(QString checkboxStyle)
{
    m_toggleNonPertinentButton =
            new QCheckBox("Non Pertinent", m_widget->centralWidget());
    m_toggleNonPertinentButton->setChecked(true);
    m_toggleNonPertinentButton->setStyleSheet(checkboxStyle);
    m_widget->addItemToToolBar(m_toggleNonPertinentButton);
    connect(m_toggleNonPertinentButton, &QCheckBox::toggled,
            this, &QVizNode::applyFilters);
}

void QVizNode::setupInfeasibleCheckbox(QString checkboxStyle)
{
    m_toggleInFeasibleButton =
            new QCheckBox("Infeasible", m_widget->centralWidget());
    m_toggleInFeasibleButton->setChecked(true);
    m_toggleInFeasibleButton->setStyleSheet(checkboxStyle);
    m_widget->addItemToToolBar(m_toggleInFeasibleButton);
    connect(m_toggleInFeasibleButton, &QCheckBox::toggled,
            this, &QVizNode::applyFilters);
}

void QVizNode::setupDominatedCheckbox(QString checkboxStyle)
{
    m_toggleDominatedButton =
            new QCheckBox("Dominated", m_widget->centralWidget());
    m_toggleDominatedButton->setChecked(true);
    m_toggleDominatedButton->setStyleSheet(checkboxStyle);
    m_widget->addItemToToolBar(m_toggleDominatedButton);
    connect(m_toggleDominatedButton, &QCheckBox::toggled,
            this, &QVizNode::applyFilters);
}

void QVizNode::setupSaveAlllSolutionsFileOption()
{
    QAction* saveAllSols =
            m_widget->addActionToMenuFile(tr("Save All Solutions"));
    connect(saveAllSols, &QAction::triggered, this, &QVizNode::saveAllSolutions);
}

void QVizNode::setupSaveBrushedSolutionsFileOption()
{
    QAction* saveBrushedSols =
            m_widget->addActionToMenuFile(tr("Save Brushed Solutions"));
    connect(saveBrushedSols, &QAction::triggered,
            this, &QVizNode::saveBrushedSolutions);
}

void QVizNode::setupSelectVariablesEditOptions()
{
    QAction* selectVariables =
            m_widget->addActionToMenuEdit(tr("Select Variables"));
    connect(selectVariables, &QAction::triggered,
            m_widget, &VisualisationWidget::showVariableSelectionForm);
}

void QVizNode::setupZoomToBrushedButton()
{
    m_toggleBrushedButton = new QPushButton(ButtonZoomToBrushed, m_widget->centralWidget());
    m_widget->addItemToToolBar(m_toggleBrushedButton);
    connect(m_toggleBrushedButton, &QPushButton::clicked,
            this, &QVizNode::filterBrushedSolutions);
}

void QVizNode::setupWidget(VisualisationWidget *widget)
{
    if(m_widget != widget) {
        m_timer->stop();
        if(m_widget) {
            disconnect(m_widget, 0, 0, 0);
            delete m_widget;
        }

        m_widget = widget;
        m_widget->setAttribute(Qt::WA_DeleteOnClose);

        connect(static_cast<QObject*>(m_widget), &QObject::destroyed,
                m_timer, &QTimer::stop);        
        connect(static_cast<QObject*>(m_widget), &QObject::destroyed,
                this, &QVizNode::resetWidget);
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
        m_ipset->defineSetGoalVec(idxIPset, true);
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

//    filterBrushedSolutions();

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

void QVizNode::selectIterComboPopup()
{
    QString selIterOptText = m_iterationOptions->currentText();
    int selIter = selIterOptText.toInt();
    selectIterationSet(selIter);
}

void QVizNode::selectIterationSet(int iterationNumber)
{
    Json::JsonObject popLogObj =  this->m_ipset->log()->populationsLog();
    JsonArray jarray;

    if(popLogObj.contains(Log::LPops)){
        JsonArray allPopLogArr = popLogObj[Log::LPops].toArray();
        if(!allPopLogArr.empty()){
            jarray = allPopLogArr.at(iterationNumber-1).toObject()[Log::LSolutions].toArray();

            ISet* tempSet = m_selectedSet->clone();
            tempSet->clearMappings();
            while(!jarray.empty()){
                JsonObject jObj = jarray.takeAt(0).toObject();
                IMappingSPtr imap = IMappingSPtr(new IMapping(m_ipset->problem()));
                bool jsonOK = fromJSonObj(imap, jObj);
                if(jsonOK) {
                    tempSet->append(imap);
                }
            }
            setSelectedSet(tempSet);
            delete tempSet;
        }
    }
}

void QVizNode::setAvailableIterationList()
{
    int numIterLogs = 0;
    Json::JsonObject popLogObj = this->m_ipset->log()->populationsLog();
    if(popLogObj.contains(Log::LPops)){
        JsonArray allPopLogArr = popLogObj[Log::LPops].toArray();
        numIterLogs = allPopLogArr.size();
    }

    QStringList iterationsList;
    for(int i=numIterLogs-1; i>=0; i--){
        iterationsList.append(QString::number(i+1));
    }
    m_iterationOptions->clear();
    m_iterationOptions->insertItems(0, iterationsList);    
}



bool QVizNode::isRefreshable()
{
    if(isInitialized() && isWidgetVisible()){
        return m_refreshRequested;
    } else {
        return false;
    }
}

bool QVizNode::isInitialized()
{
    if(m_widget && m_ipset){
        return true;
    } else {
        return false;
    }
}

bool QVizNode::areVariablesToDisplaySelected()
{
    if(m_widget->selectedIndices().length() > 1){
        return true;
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
        setDisplaySet(m_filteredSet);

        if(m_isInSync) {
            // Sync with linked nodes
            syncNetworkBrushing();
            // Sync with other local plots
            syncBrushing();
        }
    } else {
        resetButtons();
        if(m_isInSync) {
            // Sync with linked nodes
            emit requestToReset();
            resetSyncBrushing();
        }
    }
}

void QVizNode::applyFilters()
{
    setDisplaySet(m_selectedSet);
    if(m_toggleInFeasibleButton) {
        if (!m_toggleInFeasibleButton->isChecked()) filterFeasible();
    }
    if(m_toggleNonPertinentButton) {
        if (!m_toggleNonPertinentButton->isChecked()) filterPertinent();
    }
    if(m_toggleNonPertinentButton) {
        if (!m_toggleDominatedButton->isChecked()) filterNonDominated();
    }
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
    if(m_toggleBrushedButton) {
        m_toggleBrushedButton->setText(ButtonZoomToBrushed);
    }
}

void QVizNode::updateTimedTrackingInterval(const QString &interval)
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

void QVizNode::updateIterationTrackingInterval(const QString& interval)
{
    if(interval.isEmpty()) {
        return;
    }

    int iterationToInterupt = interval.toInt();

    m_iterationInteruptor->setInteruptionIteration(iterationToInterupt);
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

void QVizNode::resetSelectedVariablesToDisplay()
{
    if(!areVariablesToDisplaySelected()){
        QVariantList selectedIndices;
        for(int i = 0; i < m_dataMaps.size(); i++) {
            if((m_dataMaps[i].m_objInd >= 0) || (m_dataMaps[i].m_constInd >= 0)) {
                selectedIndices.append(QVariant::fromValue(i));
            }
        }
        m_widget->setSelectedIndices(selectedIndices);
    }
}

void QVizNode::setData()
{
    if(!m_displayedSet) {
        return;
    }

    int nSolutions   = m_displayedSet->size();
    QVector<QVector<qreal>> outputData(nSolutions);
    TVector<OptimizationType> otypes = m_ipset->objectiveVecOptimizationTypes();
    TVector<OptimizationType> ctypes = m_ipset->constraintVecOptimizationTypes();
    TVector<OptimizationType> utypes = m_ipset->unusedVecOptimizationTypes();

    // Populate the data
    for(auto dataMap : m_dataMaps) {

        outputData.reserve(nSolutions);
        if(dataMap.m_objInd >= 0) {
            // Objective
            int ind = dataMap.m_objInd;
            for(int i = 0; i < nSolutions; i++){
                IMappingSPtr imap = m_displayedSet->at(i);
                qreal value = imap->doubleObjectiveVar(ind);
                if(otypes[ind] == Maximization) {
                    value = -value;
                }
                outputData[i].push_back(value);
            }
        } else if(dataMap.m_constInd >= 0) {
            // Constraint
            int ind = dataMap.m_constInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = m_displayedSet->at(i);
                qreal value = imap->doubleConstraintVar(ind);
                // Account for the optimization types
                if(ctypes[ind] == Maximization) {
                    value = -value;
                }
                outputData[i].push_back(value);
            }

        } else if(dataMap.m_unusedInd >= 0) {
            // Unused
            int ind = dataMap.m_unusedInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = m_displayedSet->at(i);
                qreal value = imap->doubleUnusedVar(ind);
                // Account for the optimization types
                if(utypes[ind] == Maximization) {
                    value = -value;
                }
                outputData[i].push_back(value);
            }

        } else if(dataMap.m_varInd >= 0) {
            // Decision variables
            int ind = dataMap.m_varInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = m_displayedSet->at(i);
                qreal value = imap->doubleDecisionVar(ind);
                outputData[i].push_back(value);
            }

        } else if(dataMap.m_paramInd >= 0) {
            // Parameters
            int ind = dataMap.m_paramInd;
            for(int i = 0; i < nSolutions; i++) {
                IMappingSPtr imap = m_displayedSet->at(i);
                qreal value = imap->doubleParameterVar(ind);
                outputData[i].push_back(value);
            }

        } else {
            qDebug() << "Error: data variable has no scope";
            return;
        }
    }

    m_widget->setData(outputData);
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
    TVector<bool> setGoalVec           = IPSetObj()->setGoalVec();
    TVector<IElementSPtr> goalVec      = IPSetObj()->goalVec();
    TVector<IElementSPtr> thresholdVec = IPSetObj()->thresholdVec();
    TVector<ElementProperties> oprts   = IPSetObj()->objectiveVecProperties();
    TVector<ElementProperties> cprts   = IPSetObj()->constraintVecProperties();

    int nElements = dataMaps().size();
    prefDirections.reserve(nElements);

    for(int i=0; i<nElements; i++) {
        if(dataMaps()[i].m_objInd >= 0) {
            int oInd = dataMaps()[i].m_objInd;
            int cInd = dataMaps()[i].m_constInd;

            OptimizationType type = oprts[oInd].optimizationType();
            bool goalDefined = setGoalVec[oInd];
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
                bool goalDefined = setGoalVec[oInd];
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
        } else if(dataMaps()[i].m_constInd >= 0) {
            // constraint
            goals.append(QVariant());

            int ind = dataMaps()[i].m_constInd;
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

        } else if(dataMaps()[i].m_varInd >= 0) {
            // decision variable
            goals.append(QVariant());
            thresholds.append(QVariant());
            prefDirections.append(QVariant());
        }
    }

    widgetObj()->setGoals(goals);
    widgetObj()->setThresholds(thresholds);
    widgetObj()->setPreferenceDirerctions(prefDirections);
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

void QVizNode::setDisplaySet(ISet *displayedSet)
{
    m_displayedSet = displayedSet->clone();
    setData();

    // reset the brushed indices to the full list
    if(m_displayedSet->size() == m_selectedSet->size()) {
        m_widget->resetBrushedIndices();
    }
}

ISet* QVizNode::displayedSet() const
{
    return m_displayedSet;
}

QVector<DataMap> QVizNode::dataMaps() const
{
    return m_dataMaps;
}


void QVizNode::setSelectedSet(ISet* currentPop)
{
    m_selectedSet = currentPop->clone();
    setDisplaySet(m_selectedSet);
}

void QVizNode::setFilteredSet(ISet* filteredSet)
{
    m_filteredSet->clearMappings();
    m_filteredSet = filteredSet->clone();
    setDisplaySet(m_filteredSet);
}

void QVizNode::updateFilteredSetFromBrushing()
{
    QList<int> brushedIndices = m_widget->brushedIndices();

    if(brushedIndices.empty()) {
        return;
    }

    int maxIdx = *std::max_element(brushedIndices.cbegin(), brushedIndices.cend());
    if(m_displayedSet->size() <= maxIdx) {
        return;
    }

    m_filteredSet->clearMappings();
    for(auto idx : brushedIndices) {
        m_filteredSet->append(m_displayedSet->at(idx));
    }
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
    setDisplaySet(m_selectedSet);
    m_filteredSet = m_selectedSet->clone();

    resetBrushedButton();
    if(m_toggleBrushedButton) {
        m_toggleDominatedButton->setChecked(true);
    }
    if(m_toggleInFeasibleButton) {
        m_toggleInFeasibleButton->setChecked(true);
    }
    if(m_toggleNonPertinentButton) {
        m_toggleNonPertinentButton->setChecked(true);
    }
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
                cnode->setDisplaySet(m_filteredSet);
            }
        }
    }
}

void QVizNode::resetSyncBrushing()
{
    LDesignerWidget* designerWidget =
            Designer::Internal::DesignEditorW::instance()->designerEditor()->widget();

    QList<QGraphicsItem*> items = designerWidget->scene()->items();

    /// Search for all visualisation objects
    foreach(QGraphicsItem* p, items) {
        QVizNode* cnode = dynamic_cast<QVizNode*>(p);
        if(cnode && cnode != this) {
            if(cnode->isWidgetVisible() && cnode->isInSync()) {
                cnode->resetButtons();
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



} // namespace Visualisation
