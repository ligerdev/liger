/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include "designertoolbox.h"
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>

#include <extensionsystem/pluginmanager.h>
#include <coreplugin/fancyactionbar.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>

#include <QtMath>

#include <QStyle>
#include <QMimeData>
#include <QImage>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QStackedWidget>
#include <QToolBox>
#include <QStyle>
#include <QMouseEvent>
#include <QDrag>
#include <QEvent>

#include <QDebug>


namespace Designer {

DesignerToolBox::DesignerToolBox(QWidget* parent) :
    CustomToolBox(parent),
    m_groupId(0),
    m_itemId(0),
    m_syncWithDesigner(true),
    m_position(-1),
    m_numOfColumns(3),
    m_toolButtonWidth(40),
    m_toolButtonHeight(50)
{
    // Designertoolbox should not accept drops
    setAcceptDrops(false);

    m_algorithmsGroup = new QButtonGroup(this);
    m_algorithmsGroup->setExclusive(false);
    connect(m_algorithmsGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(algorithmsGroupClicked(int)));

    m_operatorsGroup = new QButtonGroup(this);
    m_operatorsGroup->setExclusive(false);
    connect(m_operatorsGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(operatorsGroupClicked(int)));

    m_visualizationsGroup = new QButtonGroup(this);
    m_visualizationsGroup->setExclusive(false);
    connect(m_visualizationsGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(visualizationsGroupClicked(int)));

    //m_externalsGroup  = new QButtonGroup(this);
    //m_externalsGroup->setExclusive(false);
    //connect(m_externalsGroup, SIGNAL(buttonClicked(int)),
    //        this, SLOT(externalsGroupClicked(int)));

    m_algorithmsLayout    = new QGridLayout;
    m_operatorsLayout     = new QGridLayout;
    m_visualizationsLayout= new QGridLayout;
    //m_externalsLayout     = new QGridLayout;

    m_algorithmsLayout->setSpacing(0);
    m_algorithmsLayout->setContentsMargins(6,3,6,3);
    m_algorithmsLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_operatorsLayout->setSpacing(0);
    m_operatorsLayout->setContentsMargins(6,3,6,3);
    m_operatorsLayout->setSizeConstraint(QLayout::SetFixedSize);
    m_visualizationsLayout->setSpacing(0);
    m_visualizationsLayout->setContentsMargins(6,3,6,3);
    m_visualizationsLayout->setSizeConstraint(QLayout::SetFixedSize);
    //m_externalsLayout->setSpacing(0);
    //m_externalsLayout->setContentsMargins(6,3,6,3);
    //m_externalsLayout->setSizeConstraint(QLayout::SetFixedSize);

    m_algorithmsLayout->setRowStretch(3, 10);
    m_algorithmsLayout->setColumnStretch(2, 10);
    m_operatorsLayout->setRowStretch(3,10);
    m_operatorsLayout->setColumnStretch(2,10);
    m_visualizationsLayout->setRowStretch(3,10);
    m_visualizationsLayout->setColumnStretch(2,10);
    //m_externalsLayout->setRowStretch(3,10);
    //m_externalsLayout->setColumnStretch(2,10);

    m_algorithmsWidget    = new QWidget;
    m_operatorsWidget     = new QWidget;
    m_visualizationsWidget= new QWidget;
    //m_externalsWidget     = new QWidget;

    m_algorithmsWidget->setLayout(m_algorithmsLayout);
    m_operatorsWidget->setLayout(m_operatorsLayout);
    m_visualizationsWidget->setLayout(m_visualizationsLayout);
    //m_externalsWidget->setLayout(m_externalsLayout);

    this->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    m_toolBoxWidth = m_toolButtonWidth*m_numOfColumns + 45;

    this->setMinimumWidth(m_toolBoxWidth);
    this->addItem(m_algorithmsWidget,    tr("Algorithms")); //[0]
    this->addItem(m_operatorsWidget,     tr("Operators"));  //[1]
    this->addItem(m_visualizationsWidget,tr("Visualization")); //[2]
    //this->addItem(m_externalsWidget,     tr("External")); //[3]

    m_toolButtonStyle = QString("QToolButton {"
                                "margin: 1px;"
                                "border-color: #0c457e;"
                                "border-style: outset;"
                                "border-radius: 6px;"
                                "border-width: 0px;"
                                "color: black;"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(236,236,236), stop: 1 rgb(236,236,236));"
                                "}"
                                "QToolButton:pressed {"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(236,236,236), stop: 1 rgb(236,236,236));"
                                "}"
                                "QToolButton:checked {"
                                "border-radius: 6px;"
                                "border-width: 2px;"
                                "border-color: #0c457e;"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(226,226,226), stop: 1 rgb(226,226,226));"
                                "}"
                                "QToolButton:hover {"
                                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(250,250,250), stop: 1 rgb(250,250,250));"
                                "}");
}

DesignerToolBox::~DesignerToolBox()
{

}

QToolButton* DesignerToolBox::registerAlgorithm(Designer::IProcessNodeFactory *process_factory,
                                                QString tooltip, QString iconPath)
{
    int lastItem = 0;
    QToolButton* newButton = createNewButton(process_factory, tooltip, iconPath);
    m_algorithmsGroup->addButton(newButton);
    lastItem = -m_algorithmsGroup->id(newButton)-2;
    int row = qFloor(lastItem/(double)m_numOfColumns);
    int col = lastItem % m_numOfColumns;
    m_algorithmsLayout->addWidget(newButton, row, col);

    return newButton;
}

QToolButton* DesignerToolBox::registerOperator(Designer::IProcessNodeFactory *process_factory,
                                               QString tooltip, QString iconPath)
{
    int lastItem = 0;
    QToolButton* newButton = createNewButton(process_factory, tooltip, iconPath);
    m_operatorsGroup->addButton(newButton);
    lastItem = -m_operatorsGroup->id(newButton)-2;
    int row = qFloor(lastItem/(double)m_numOfColumns);
    int col = lastItem % m_numOfColumns;
    m_operatorsLayout->addWidget(newButton, row, col);

    return newButton;
}

//QToolButton* DesignerToolBox::registerExternal(Designer::IProcessNodeFactory *process_factory,
//                                               QString tooltip, QString iconPath)/
//{
//    int lastItem = 0;
//    QToolButton* newButton = createNewButton(process_factory, tooltip, iconPath);
//    m_externalsGroup->addButton(newButton);
//    lastItem = -m_externalsGroup->id(newButton)-2;
//    int row = qFloor(lastItem/(double)m_numOfColumns);
//    int col = lastItem % m_numOfColumns;
//    m_externalsLayout->addWidget(newButton, row, col);

//    return newButton;
//}

QToolButton* DesignerToolBox::registerVisualization(Designer::IProcessNodeFactory *process_factory,
                                                    QString tooltip, QString iconPath)
{
    int lastItem = 0;
    QToolButton* newButton = createNewButton(process_factory, tooltip, iconPath);
    m_visualizationsGroup->addButton(newButton);
    lastItem = -m_visualizationsGroup->id(newButton)-2;
    int row = qFloor(lastItem/(double)m_numOfColumns);
    int col = lastItem % m_numOfColumns;
    m_visualizationsLayout->addWidget(newButton, row, col);

    return newButton;
}

void DesignerToolBox::algorithmsGroupClicked(int id)
{
    m_itemId = id+1;
    m_groupId= 0;
    // First, reset all other groups.
    QList<QAbstractButton *> buttons = m_operatorsGroup->buttons();
    foreach(QAbstractButton* button, buttons) {
        button->setChecked(false);
    }

//    buttons = m_externalsGroup->buttons();
//    foreach(QAbstractButton* button, buttons) {
//        button->setChecked(false);
//    }

    buttons = m_visualizationsGroup->buttons();
    foreach(QAbstractButton* button, buttons) {
        button->setChecked(false);
    }

    // Now, have a look which button was clicked from the algorithms group.
    buttons = m_algorithmsGroup->buttons();
    foreach(QAbstractButton *button, buttons){
        if(m_algorithmsGroup->button(id) != button) {
            button->setChecked(false);
        }
    }
}

void DesignerToolBox::operatorsGroupClicked(int id)
{
    m_itemId = id+1;
    m_groupId= 1;
    // First, reset all other groups.
    QList<QAbstractButton *> buttons = m_algorithmsGroup->buttons();
    foreach(QAbstractButton* button, buttons)
        button->setChecked(false);

//    buttons = m_externalsGroup->buttons();
//    foreach(QAbstractButton* button, buttons)
//        button->setChecked(false);

    buttons = m_visualizationsGroup->buttons();
    foreach(QAbstractButton* button, buttons)
        button->setChecked(false);

    // Now, have a look which button was clicked from the algorithms group.
    buttons = m_operatorsGroup->buttons();
    foreach(QAbstractButton *button, buttons){
        if(m_operatorsGroup->button(id) != button) {
            button->setChecked(false);
        }
    }

}

//void DesignerToolBox::externalsGroupClicked(int id)
//{
//    m_itemId = id+1;
//    m_groupId= 2;
//    // First, reset all other groups.
//    QList<QAbstractButton *> buttons = m_operatorsGroup->buttons();
//    foreach(QAbstractButton* button, buttons) {
//        button->setChecked(false);
//    }

//    buttons = m_algorithmsGroup->buttons();
//    foreach(QAbstractButton* button, buttons) {
//        button->setChecked(false);
//    }

//    buttons = m_visualizationsGroup->buttons();
//    foreach(QAbstractButton* button, buttons) {
//        button->setChecked(false);
//    }

//    // Now, have a look which button was clicked from the algorithms group.
//    buttons = m_externalsGroup->buttons();
//    foreach(QAbstractButton *button, buttons){
//        if(m_externalsGroup->button(id) != button) {
//            button->setChecked(false);
//        }
//    }
//}

void DesignerToolBox::visualizationsGroupClicked(int id)
{
    m_itemId = id+1;
    m_groupId= 3;
    // First, reset all other groups.
    QList<QAbstractButton *> buttons = m_operatorsGroup->buttons();
    foreach(QAbstractButton* button, buttons) {
        button->setChecked(false);
    }

//    buttons = m_externalsGroup->buttons();
//    foreach(QAbstractButton* button, buttons) {
//        button->setChecked(false);
//    }

    buttons = m_algorithmsGroup->buttons();
    foreach(QAbstractButton* button, buttons) {
        button->setChecked(false);
    }

    // Now, have a look which button was clicked from the algorithms group.
    buttons = m_visualizationsGroup->buttons();
    foreach(QAbstractButton *button, buttons){
        if(m_visualizationsGroup->button(id) != button) {
            button->setChecked(false);
        }
    }
}

void DesignerToolBox::restoreSettings(int position)
{
    Q_UNUSED(position)

    //    m_position = position; // save it so that we can save/restore in updateCurrentDesigner

    //    QSettings *settings = Core::ICore::settings();
    //    const bool toggleSync = settings->value(outLineKey(position), true).toBool();
    //    toggleSyncButton()->setChecked(toggleSync);

    //    if (IOutlineWidget *outlineWidget = qobject_cast<IOutlineWidget*>(currentWidget())) {
    //        outlineWidget->restoreSettings(position);
    //    }
}

void DesignerToolBox::saveSettings(int position)
{
    Q_UNUSED(position)

    //    Q_ASSERT(position == m_position);

    //    QSettings *settings = Core::ICore::settings();
    //    settings->setValue(outLineKey(position), toggleSyncButton()->isEnabled());

    //    if (IOutlineWidget *outlineWidget = qobject_cast<IOutlineWidget*>(currentWidget())) {
    //        outlineWidget->saveSettings(position);
    //    }
}

QButtonGroup* DesignerToolBox::getAglorithmGroup()
{
    return m_algorithmsGroup;
}

QButtonGroup* DesignerToolBox::getOperatorsGroup()
{
    return m_operatorsGroup;
}

//QButtonGroup* DesignerToolBox::getExternalsGroup()
//{
//    return m_externalsGroup;
//}

QButtonGroup* DesignerToolBox::getVisualizationsGroup()
{
    return m_visualizationsGroup;
}

bool DesignerToolBox::eventFilter(QObject *target, QEvent *event)
{
    Q_UNUSED(target);
    if(event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* ev = static_cast<QMouseEvent*>(event);
        mousePressEvent(ev);
        return true;
    }
    return false;
}

void DesignerToolBox::mousePressEvent(QMouseEvent *event)
{
    QButtonGroup* currentGp = currentButtonGroup();
    if(currentGp) {
        for(int i=0; i<currentGp->buttons().size(); i++) {
            QToolButton* button = dynamic_cast<QToolButton*>(currentGp->buttons()[i]);
            QRect buttonRect = QRect(button->mapToGlobal(button->rect().topLeft()),
                                     button->mapToGlobal(button->rect().bottomRight()));
            QPoint mousePos(event->screenPos().x(), event->screenPos().y());
            if(buttonRect.contains(mousePos)) {
                button->setChecked(true);
            } else {
                button->setChecked(false);
            }
        }
    }
}

void DesignerToolBox::mouseMoveEvent(QMouseEvent *event)
{
    // Only accept left button
    if(event->buttons() & Qt::LeftButton) {
        QToolButton* currentButton = checkedButton();
        if(currentButton) {
            // Setup drag
            QDrag* drag = new QDrag(this);
            QMimeData* mime = new QMimeData;
            drag->setMimeData(mime);
            drag->setPixmap(currentButton->icon().pixmap(20.0, 20.0));
            drag->setHotSpot(QPoint(currentButton->width()/2.0,
                                    currentButton->height()/2.0));
            drag->exec();
        } else {
            return;
        }
    }
    return;
}

QToolButton* DesignerToolBox::createNewButton(IProcessNodeFactory* processFactory,
                                              QString tooltip,
                                              QString iconPath)
{
    QToolButton* newButton = new QToolButton(this);
    newButton->installEventFilter(this);
    // TODO: Add some style to the tool buttons. Below is a template using QStyle sheets.
    newButton->setStyleSheet(m_toolButtonStyle);
    newButton->setToolTip(tooltip);
    connect(newButton, SIGNAL(clicked()), processFactory, SLOT(processButtonTriggered()));
    newButton->setCheckable(true);
    if(!iconPath.isEmpty()) {
        newButton->setIcon(QIcon(QPixmap(iconPath)));
    } else {
        newButton->setIcon((QIcon(QPixmap(":/core/images/default_tool_image.png"))));
    }

    newButton->setIconSize(QSize(m_toolButtonWidth,m_toolButtonHeight));
    return newButton;
}

QButtonGroup* DesignerToolBox::currentButtonGroup() const
{
    switch (currentIndex()) {
    case 0:
        return m_algorithmsGroup;
    case 1:
        return m_operatorsGroup;
    case 2:
        return m_visualizationsGroup;
    //case 3:
    //    return m_externalsWidget;
    default:
        return 0;
    }
}

QToolButton* DesignerToolBox::checkedButton() const
{
    QButtonGroup* currentGp = currentButtonGroup();
    QToolButton* thisButton = 0;
    if(currentGp) {
        for(int i=0; i<currentGp->buttons().size(); i++) {
            if(currentGp->buttons()[i]->isChecked()) {
                thisButton = dynamic_cast<QToolButton*>(currentGp->buttons()[i]);
                break;
            }
        }
    }
    return thisButton;
}

} // namespace Designer
