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
#ifndef SELECTSCATTERPLOTVARSFORM_H
#define SELECTSCATTERPLOTVARSFORM_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class SelectScatterPlotVarsForm;
}

class SelectScatterPlotVarsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SelectScatterPlotVarsForm(QWidget *parent = nullptr);
    ~SelectScatterPlotVarsForm();

    void setSelectedIndices(const QVariantList &selected);

    void setXNames(const QStringList &names);
    void setYNames(const QStringList &names);
    void setSizeNames(const QStringList &names);
    void setColorNames(const QStringList &names);

    void setShowColor(bool flag);
    void setShowSize(bool flag);

    bool showSize() const;
    bool showColor() const;

    QVariantList selectedIndex() const;

private:
    Ui::SelectScatterPlotVarsForm *ui;
    QVector<int> m_selectedIndices;
};

#endif // SELECTSCATTERPLOTVARSFORM_H
