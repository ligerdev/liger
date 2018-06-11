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
#ifndef VISUALISATIONWIDGET_H
#define VISUALISATIONWIDGET_H

#include <QMainWindow>
#include <QVariantList>
#include <QVector>
#include <QGridLayout>

namespace Ui {
class VisualisationWidget;
}

namespace Visualisation {
class VisualisationDataModel;
class VisualisationViewModel;

class VisualisationWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit VisualisationWidget(QWidget *parent = 0);
    virtual ~VisualisationWidget();

    /*!
     * \brief Access to the menubar
     * This function is created for loading user defined actions/menus to
     * the menubar.
     */
    QMenuBar* menuBar() const;

    /*!
     * \brief Access to the File menu in menubar.
     * This function is created for loading user defined actions to
     * File menu.
     */
    QMenu*   menuFile() const;

    /*!
     * \brief Access to the Edit menu in menubar.
     * This function is created for loading user defined actions to
     * Edit menu.
     */
    QMenu*   menuEdit() const;

    /*!
     * \brief Add action to file menu.
     * The pointer to the newly created action is managed by the visualisation
     * widget.
     * \param Name of the action.
     * \return The pointer to the newly created action.
     * Use this pointer to connect to user defined response function (SLOT)
     */
    QAction* addActionToMenuFile(const QString& actionName);

    /*!
     * \brief Add action to edit menu.
     * The pointer to the newly created action is managed by the visualisation
     * widget.
     * \param Name of the action.
     * \return The pointer to the newly created action.
     * Use this pointer to connect to user defined response function (SLOT)
     */
    QAction* addActionToMenuEdit(const QString& actionName);

    void addItemToToolBar(QWidget* item);
    void addSpacerToToolBar();

    void setView(VisualisationViewModel *view);

    virtual void showVariableSelectionForm();
    virtual void setData(const QVector<QVector<qreal> >& dataVec,
                         const QStringList& names);
    virtual void setData(const QVector<QVector<qreal> >& dataVec);
    virtual void setNames(const QStringList& names);
    virtual void setCategories(const QVariantList &colCategories);
    virtual void setSelectedIndices(const QVariantList &selected);
    virtual void setBoxPlotData(const QVector<QVector<QVector<qreal> > > &boxplotData);
    virtual void setGoals(const QVariantList& goals);
    virtual void setThresholds(const QVariantList& thresholds);
    virtual void setPreferenceDirerctions(const QVariantList& preferenceDirections);

    QVector<QVector<qreal>> rawData() const;
    QStringList allNames() const;
    QVariantList categories() const;
    QVariantList selectedIndices() const;
    QList<int> brushedIndices() const;
    QMap<int, QVector<qreal> > brushedBounds() const;
    void resetBrushedIndices();

    void reloadView();

signals:
    void brushedBoundsUpdated(const int& index, const qreal& lb, const qreal& ub);
    void brushedIndicesUpdated(const QList<int>& brushedIndices);
    void setDisplayPreferences(bool disp);

private slots:
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionSelect_Vars_triggered();

protected:
    VisualisationViewModel *view() const;
    VisualisationDataModel *data() const;

private:
    Ui::VisualisationWidget *ui;
    VisualisationDataModel  *m_data;
    VisualisationViewModel  *m_view;
};

}

#endif // VISUALISATIONWIDGET_H
