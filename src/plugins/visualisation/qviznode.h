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
#ifndef QVIZNODE_H
#define QVIZNODE_H
#include <visualisation/visualisation_global.h>
#include <visualisation/visualisationconstants.h>
#include <designer/iengine/iprocessnode.h>

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

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

    bool isInSync() const;
    bool isWidgetVisible() const;

    void setupInteractions(IProcessNode *node);
    void dataUpdatedByEngine();

signals:
    void requestToUpdateDisplayedSolutions(const QMap<QString, QVector<double>> & dataMap);
    void requestToReset();

protected slots:
    void refreshPlotRequested();

    /*!
     * \brief This slot is triggered when requestToUpdateDisplayedSolutions signal
     * is received.
     * \param dataMap
     */
    void autoBrushPlot(const QMap<QString, QVector<double>> & dataMap);
    virtual void updateRobustness(const QStringList& indicators,
                                  const QVector<qreal>& parameters);

private slots:
    bool isRefreshable();
    void refreshPlot();
    void receivedBrushedBounds(int index, qreal lb, qreal ub);
    void updateGoal(int index, qreal lb, qreal ub);
    void saveBrushedSolutions();
    void saveAllSolutions();
    void selectSetsToDisplay(const QString & setName);
    void selectSetsComboPopup();
    // activated by pushbuttons
    void filterBrushedSolutions();
    void applyFilters();
    void filterNonDominated();
    void filterFeasible();
    void filterPertinent();

    void resetBrushedButton();
    void updateTrackingInterval(const QString& interval);
    void resetWidget();
    void inSyncCheckboxUpdate(bool insync);
    void recordGoals(bool toggled);

protected:
    void initialiseVizNode(VisualisationWidget* widget);

    virtual void showWidget() const;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void customiseWidget(VisualisationWidget* widget) = 0;

private:
    void setWidget(VisualisationWidget* widget);
    QString getSaveFileName(const QString &title = QString("Save Solutions"));
    bool saveToFile(const QString &file,
                    Tigon::Representation::ISet* solset);
    void setDataMaps();
    void setSelectedIndices();
    void setData();
    void extractNamesAndCategories();
    void extractGoalsAndThresholds();

    void saveAsJsonFormat(QFile*ofile, Tigon::Representation::ISet* solset);
    void saveAsTxtFormat(QFile* ofile, Tigon::Representation::ISet* solset);

    void displaySet(Tigon::Representation::ISet* displayedSet);

    ///\brief if there is no unique set to select, selectedSet is set(0)
    void setSelectedSet(Tigon::Representation::ISet* selectedSet);
    void setFilteredSet(Tigon::Representation::ISet* filteredSet);
    void updateFilteredSetFromBrushing();
    void setSelectSetsList();
    void resetButtons();

    void syncBrushing();
    void syncNetworkBrushing();

    QVector<qreal> getBoxplotEntry(Tigon::Representation::IElementSPtr elm,
                                   Tigon::OptimizationType type);

    VisualisationWidget* m_widget;
    Tigon::Representation::IPSet* m_ipset;
    QVector<QVector<Tigon::Representation::IElementSPtr> > m_elemData;
    QVector<DataMap> m_dataMaps;

    Tigon::Representation::ISet* m_selectedSet;
    Tigon::Representation::ISet* m_filteredSet;
    Tigon::Representation::ISet* m_displayedSet;
    QComboBox*   m_selectSetsCombo;
    QStringList  m_selectSetsOptions;
    QPushButton* m_toggleBrushedButton;
    QCheckBox*   m_toggleDominatedButton;
    QCheckBox*   m_toggleFeasibleButton;
    QCheckBox*   m_togglePertinentButton;
    QCheckBox*   m_toggleDispPreferencesButton;
    bool         m_isInSync;
    bool         m_recordGoals;
    QTimer*      m_timer;
    bool         m_refreshRequested;
};
}
#endif // QVIZNODE_H
