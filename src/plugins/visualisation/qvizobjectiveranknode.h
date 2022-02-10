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
#ifndef QVIZOBJECTIVERANKNODE_H
#define QVIZOBJECTIVERANKNODE_H
#include <visualisation/visualisation_global.h>
#include <visualisation/visualisationconstants.h>
#include <visualisation/qviznode.h>

#include <visualisation/core/objectiverankwidget.h>

#include <QObject>
#include <QComboBox>

namespace Visualisation {

class QVizObjectiveRankNode: public QVizNode
{
    Q_OBJECT

public:
    QVizObjectiveRankNode();
    ~QVizObjectiveRankNode();

signals:
    void requestRunObjectiveReduction(ObjectiveRankWidget* widget);

private slots:
    void runObjectiveReduction();
    void selectVisualisation(const QString &setName);
    void selectAlgorithm(const QString &setName);
#ifdef CSDP_LIBRARY
    void standardizeCheckboxUpdate(bool s);
#endif

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    void customiseWidget(VisualisationWidget* widget);

    void setupAlgorithmSelection();
    void setupVisualisationSelection();

    void selectVisualisationComboPopup();
    void selectAlgorithmComboPopup();

    QStringList m_selectVisualisationOptions;
    enum VisualisationType { Ranking, VarCorr, VarPCs };
    VisualisationType m_selectedVisualisation;
    QComboBox* m_visualSelectCombo;

    QStringList m_selectAlgorithmOptions;
    enum ObjectiveReductionAlgorithm { L_PCA, NL_MVU_PCA };
    ObjectiveReductionAlgorithm m_selectedAlgorithm;
    QComboBox* m_algorithmSelectCombo;

    QCheckBox* m_standardizeButton;

#ifdef CSDP_LIBRARY
    void setupStandardizeCheckbox(QString checkboxStyle);
    bool m_standardize;
#endif
};

} // namespace Visualisation

#endif // QVIZOBJECTIVERANKNODE_H
