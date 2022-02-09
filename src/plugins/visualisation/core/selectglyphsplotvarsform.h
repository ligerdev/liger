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
#ifndef SELECTGLYPHSPLOTVARSFORM_H
#define SELECTGLYPHSPLOTVARSFORM_H

#include <QDialog>

namespace Ui {
class SelectGlyphsPlotVarsForm;
}

class SelectGlyphsPlotVarsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SelectGlyphsPlotVarsForm(QWidget *parent = nullptr);
    ~SelectGlyphsPlotVarsForm();

    void setSelectedIndices(const QVariantList &selected);

    void setXNames(const QStringList &names);
    void setYNames(const QStringList &names);
    void setTopNames(const QStringList &names);
    void setBottomNames(const QStringList &names);
    void setRightNames(const QStringList &names);
    void setLeftNames(const QStringList &names);

    QVariantList selectedIndex() const;

private:
    Ui::SelectGlyphsPlotVarsForm *ui;
    QVector<int> m_selectedIndices;
};

#endif // SELECTGLYPHSPLOTVARSFORM_H
