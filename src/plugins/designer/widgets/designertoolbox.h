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
#ifndef DESIGNERTOOLBOX_H
#define DESIGNERTOOLBOX_H
#include <designer/designer_global.h>

#include <QWidget>
#include <QMenu>
#include <QGridLayout>
#include <QToolBox>
#include <QToolButton>
#include <QButtonGroup>

#include <utils/styledbar.h>
#include <designer/widgets/customtoolbox.h>

namespace Designer {
class IProcessNodeFactory;

class DESIGNER_EXPORT DesignerToolBox : public CustomToolBox// public QToolBox
{
    Q_OBJECT

public:
    DesignerToolBox(QWidget *parent=0);
    ~DesignerToolBox();

    void saveSettings(int position);
    void restoreSettings(int position);

    QToolButton* registerAlgorithm(Designer::IProcessNodeFactory *process_factory,
                                   QString tooltip, QString iconPath);
    QToolButton* registerOperator(Designer::IProcessNodeFactory *process_factory,
                                  QString tooltip, QString iconPath);
    QToolButton* registerVisualization(Designer::IProcessNodeFactory *process_factory,
                                       QString tooltip, QString iconPath);
    //QToolButton* registerExternal(Designer::IProcessNodeFactory *process_factory,
    //                              QString tooltip, QString iconPath);

    QButtonGroup* getAglorithmGroup();
    QButtonGroup* getOperatorsGroup();
    QButtonGroup* getVisualizationsGroup();
    //QButtonGroup* getExternalsGroup();

protected:
    bool eventFilter(QObject* target, QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void algorithmsGroupClicked(int id);
    void operatorsGroupClicked(int id);
    void visualizationsGroupClicked(int id);
    //void externalsGroupClicked(int id);

private:
    QToolButton* createNewButton(IProcessNodeFactory* processFactory,
                                 QString tooltip,
                                 QString iconPath);
    QButtonGroup* currentButtonGroup() const;
    QToolButton* checkedButton()       const;

    // TODO Remove the following two member variables
    int m_groupId;
    int m_itemId;

    bool            m_syncWithDesigner;
    int             m_position;
    QList<QToolButton *> m_toolsList;
    QMap<QString, int> m_algorithmsMap;
    QMap<QString, int> m_operatorsMap;
    QMap<QString, int> m_visualizationsMap;
    //QMap<QString, int> m_externalsMap;
    QButtonGroup* m_algorithmsGroup;
    QButtonGroup* m_operatorsGroup;
    QButtonGroup* m_visualizationsGroup;
    //QButtonGroup* m_externalsGroup;
    QGridLayout*  m_algorithmsLayout;
    QGridLayout*  m_operatorsLayout;
    QGridLayout*  m_visualizationsLayout;
    //QGridLayout*  m_externalsLayout;
    QWidget*      m_algorithmsWidget;
    QWidget*      m_operatorsWidget;
    QWidget*      m_visualizationsWidget;
    //QWidget*      m_externalsWidget;
    int           m_numOfColumns;
    int           m_toolButtonWidth;
    int           m_toolButtonHeight;
    int           m_toolBoxWidth;
    QString       m_toolButtonStyle;
};

} // namespace Core



#endif // DESIGNERTOOLBOX_H
