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
#ifndef QVIZNODE_H
#define QVIZNODE_H
#include <visualisation/visualisation_global.h>
#include <visualisation/visualisationconstants.h>
#include <designer/iengine/iprocessnode.h>

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <visualisation/iterationinteruptor.h>

#include <QVector>

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QPushButton;
class QCheckBox;
class QFile;
class QTimer;
QT_END_NAMESPACE

namespace Tigon {
namespace Representation {
class IPSet;
class ISet;
}
}
namespace Visualisation {

class VisualisationWidget;
class DataMap {
public:
    DataMap(int obj=-1, int cnst=-1, int unused=-1, int var=-1, int param=-1)
        : m_objInd(obj), m_constInd(cnst), m_unusedInd(unused)
        , m_varInd(var), m_paramInd(param){ }

    int m_objInd, m_constInd, m_unusedInd, m_varInd, m_paramInd;
};

class VISUALISATION_EXPORT QVizNode : public Designer::IProcessNode
{
     Q_OBJECT

public:
    QVizNode();
    virtual ~QVizNode();

    void setupInteractions(IProcessNode *node);
    void dataUpdatedByEngine();

signals:
    void requestToUpdateDisplayedSolutions(const QMap<QString, QVector<double>> & dataMap);
    void requestToReset();

protected slots:
    void refreshPlotRequested();
    void autoBrushPlot(const QMap<QString, QVector<double>> & dataMap);
    void receivedBrushedBounds(int index, qreal lb, qreal ub);
    void resetBrushedButton();

private slots:
    void refreshPlot();
    void updateGoal(int index, qreal lb, qreal ub);
    void saveBrushedSolutions();
    void saveAllSolutions();
    void selectSetsToDisplay(const QString & setName);
    void selectSetsComboPopup();
    void selectIterComboPopup();

    // activated by pushbuttons
    void filterBrushedSolutions();
    void applyFilters();

    void updateTimedTrackingInterval(const QString& interval);
    void updateIterationTrackingInterval(const QString& interval);
    void resetWidget();
    void inSyncCheckboxUpdate(bool insync);
    void recordGoals(bool toggled);

protected:
    virtual void setData();
    void initialiseVizNode(VisualisationWidget* widget);

    bool isInSync() const;
    Tigon::Representation::IPSet* IPSetObj() const;
    VisualisationWidget* widgetObj() const;
    Tigon::Representation::ISet* displayedSet() const;
    QVector<DataMap> dataMaps() const;

    void extractNamesAndCategories();
    void extractGoalsAndThresholds();

    void setupSaveBrushedSolutionsFileOption();
    void setupSaveAlllSolutionsFileOption();
    void setupSelectVariablesEditOptions();
    void setupZoomToBrushedButton();

    void setupDominatedCheckbox(QString checkboxStyle);
    void setupInfeasibleCheckbox(QString checkboxStyle);
    void setupNonPertientCheckbox(QString checkboxStyle);
    void setupRecordGoalsCheckbox(QString checkboxStyle);
    void setupInSyncCheckbox(QString checkboxStyle);

    void setupSetSelection();
    void setupTimedTracking();
    void setupIterationTracking();
    void setupIterationSelection();

    virtual void showWidget() const;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void customiseWidget(VisualisationWidget* widget) = 0;

private:
    void setupWidget(VisualisationWidget* widget);
    void setupInSync(QString checkboxStyle);

    void initializeWidgetData();
    void initializeWidgetDisplay();
    QString getSaveFileName(const QString &title = QString("Save Solutions"));
    bool saveToFile(const QString &file,
                    Tigon::Representation::ISet* solset);
    void setDataMaps();
    void resetSelectedVariablesToDisplay();

    void saveAsJsonFormat(QFile*ofile, Tigon::Representation::ISet* solset);
    void saveAsTxtFormat(QFile* ofile, Tigon::Representation::ISet* solset);

    void setDisplaySet(Tigon::Representation::ISet* displayedSet);

    void updateFilteredSetFromBrushing();
    void setSelectSetsList();
    void setAvailableIterationList();
    void resetButtons();

    void selectIterationSet(int iterationNumber);
    void filterNonDominated();
    void filterFeasible();
    void filterPertinent();
    void setSelectedSet(Tigon::Representation::ISet* selectedSet);
    void setFilteredSet(Tigon::Representation::ISet* filteredSet);

    void syncBrushing();
    void resetSyncBrushing();
    void syncNetworkBrushing();

    bool isRefreshable();
    bool isInitialized();
    bool areVariablesToDisplaySelected();

    bool isWidgetVisible() const;



    VisualisationWidget* m_widget;
    Tigon::Representation::IPSet* m_ipset;

    QVector<DataMap> m_dataMaps;

    Tigon::Representation::ISet* m_selectedSet;
    Tigon::Representation::ISet* m_filteredSet;
    Tigon::Representation::ISet* m_displayedSet;

    QComboBox*   m_selectSetsCombo;
    QStringList  m_selectSetsOptions;
    QPushButton* m_toggleBrushedButton;
    QCheckBox*   m_toggleDominatedButton;
    QCheckBox*   m_toggleInFeasibleButton;
    QCheckBox*   m_toggleNonPertinentButton;
    bool         m_isInSync;
    bool         m_recordGoals;
    QTimer*      m_timer;
    IterationInteruptor* m_iterationInteruptor;

    bool         m_refreshRequested;
    int          m_selectedIteration;

    QComboBox* m_iterationOptions;
};
}
#endif // QVIZNODE_H
