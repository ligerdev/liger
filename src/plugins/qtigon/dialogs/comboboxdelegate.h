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
#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H
#include <QStyledItemDelegate>
#include <QStringList>
#include <QVector>

class ComboboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboboxDelegate(QObject *parent = 0, const int& index = 0);
    ComboboxDelegate(QObject *parent, const int& index,
                     const QVector<QStringList>& names);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;

    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const Q_DECL_OVERRIDE;
    int acceptedIndex() const;
    void setAcceptedIndex(int acceptedIndex);

    QVector<QStringList> names() const;
    void setNames(const QVector<QStringList> &names);

private:
    int m_acceptedIndex;
    QVector<QStringList> m_names;
};

#endif // COMBOBOXDELEGATE_H
