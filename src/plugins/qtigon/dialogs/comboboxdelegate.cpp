/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <qtigon/dialogs/comboboxdelegate.h>

#include <QComboBox>
#include <QDebug>

using namespace std;

ComboboxDelegate::ComboboxDelegate(QObject *parent, const int &index)
    : QStyledItemDelegate(parent)
    , m_acceptedIndex(index)
{
}

ComboboxDelegate::ComboboxDelegate(QObject *parent, const int &index,
                                   const QVector<QStringList> &names)
    : QStyledItemDelegate(parent)
    , m_acceptedIndex(index)
    , m_names(names)
{

}

QWidget *ComboboxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    int idx = index.row();
    if(index.column() == m_acceptedIndex && idx < m_names.size()) {
        QComboBox* editor = new QComboBox(parent);
        editor->setFrame(false);
        editor->setMinimumWidth(20);
        editor->setMinimumHeight(20);
        editor->insertItems(0, m_names[idx]);
        editor->setCurrentIndex(0);
        return editor;
    } else {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ComboboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == m_acceptedIndex) {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QComboBox* combo = static_cast<QComboBox*>(editor);
        combo->setCurrentText(value);
        combo->setToolTipDuration(500);
        combo->setToolTip(tr("Choose your action."));
    }  else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    if(index.column() == m_acceptedIndex) {
        QComboBox* combo = static_cast<QComboBox*>(editor);
        QString value = combo->currentText();
        model->setData(index, value, Qt::EditRole);
    } else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void ComboboxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if(index.column() == m_acceptedIndex) {
        editor->setGeometry(option.rect);
    } else {
        QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    }
}

int ComboboxDelegate::acceptedIndex() const
{
    return m_acceptedIndex;
}

void ComboboxDelegate::setAcceptedIndex(int acceptedIndex)
{
    m_acceptedIndex = acceptedIndex;
}

QVector<QStringList> ComboboxDelegate::names() const
{
    return m_names;
}

void ComboboxDelegate::setNames(const QVector<QStringList> &names)
{
    m_names = names;
}
