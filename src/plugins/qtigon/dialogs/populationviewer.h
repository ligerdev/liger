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
#ifndef POPULATIONVIEWER_H
#define POPULATIONVIEWER_H

#include <qtigon/dialogs/qoperatordiagtabitem.h>
#include <QJsonObject>

namespace Ui {
class PopulationViewer;
}

class PopulationViewer : public QOperatorDiagTabItem
{
    Q_OBJECT

public:
    explicit PopulationViewer(QWidget *parent = 0);
    ~PopulationViewer();

    void reset();
    void save();

    QVector<QVector<qreal> > data() const;
    QJsonObject jsonData() const;
    void setFilePath(QString file);

signals:
    void updateFromPoupluationViewer(const QVector<QVector<qreal> >& dVecData);
    void updateJsonFromPopViewr(const QJsonObject& json);
    void updateFilePath(const QString& filePath);

private slots:
    void on_loadPushButton_clicked();
    void on_clearPushButton_clicked();

private:
    void loadDataFromFile(const QString& file);
    void selectDefaultPopulation();

    Ui::PopulationViewer *ui;
    QVector<QVector<qreal> > m_data;
    QJsonObject m_json;
};

#endif // POPULATIONVIEWER_H
