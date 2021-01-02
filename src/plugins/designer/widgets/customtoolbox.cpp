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
#include "customtoolbox.h"

#include <utils/hostosinfo.h>
#include <utils/stylehelper.h>
#include <utils/styledbar.h>

#include <QApplication>
#include <QStyleOptionToolBox>
#include <QtGui>
#include <QDebug>
#include <QBrush>

using namespace Designer;

//const int m_tabWidth = 30;
void CustomToolBoxButton::fadeIn()
{
    animator.stop();
    animator.setDuration(80);
    animator.setEndValue(40);
    animator.start();
}

void CustomToolBoxButton::fadeOut()
{
    animator.stop();
    animator.setDuration(160);
    animator.setEndValue(0);
    animator.start();
}

void CustomToolBoxButton::setFader(float value)
{
    m_fader = value;
//    tabbar->update();
}





//void CustomToolBoxButton::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event)
//    QPainter p(this);

////    for (int i = 0; i < count(); ++i)
////        if (i != currentIndex())
//            paintTab(&p);

//    // paint active tab last, since it overlaps the neighbors
////    if (currentIndex() != -1)
////        paintTab(&p, currentIndex());
//}

// Handle hover events for mouse fade ins
void CustomToolBoxButton::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e)

//    int newHover = -1;
//    for (int i = 0; i < count(); ++i) {
//        QRect area = tabRect(i);
//        if (area.contains(e->pos())) {
//            newHover = i;
//            break;
//        }
//    }
//    if (newHover == m_hoverIndex)
//        return;

//    if (validIndex(m_hoverIndex))
//        m_tabs[m_hoverIndex]->fadeOut();

//    m_hoverIndex = newHover;

//    if (validIndex(m_hoverIndex)) {
//        m_tabs[m_hoverIndex]->fadeIn();
//        m_hoverRect = tabRect(m_hoverIndex);
//    }
}

//bool CustomToolBoxButton::event(QEvent *event)
//{
//    if (event->type() == QEvent::ToolTip) {
//        if (validIndex(m_hoverIndex)) {
//            QString tt = tabToolTip(m_hoverIndex);
//            if (!tt.isEmpty()) {
//                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tt, this);
//                return true;
//            }
//        }
//    }
//    return QWidget::event(event);
//}

// Resets hover animation on mouse enter
void CustomToolBoxButton::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

//    m_hoverRect = QRect();
//    m_hoverIndex = -1;
    this->fadeIn();
}

// Resets hover animation on mouse enter
void CustomToolBoxButton::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)
//    m_hoverIndex = -1;
//    m_hoverRect = QRect();
//    for (int i = 0 ; i < m_tabs.count() ; ++i) {
//        m_tabs[i]->fadeOut();
//    }
    this->fadeOut();

}

void CustomToolBoxButton::paintTab(QPainter *painter) const
{
    QRect rect = this->rect();

    QColor light = Utils::StyleHelper::sidebarHighlight();
//    QColor dark  = Utils::StyleHelper::sidebarShadow();
    // Set the maximum width for the left side bar.
    painter->setPen(light);
    painter->drawLine(rect.topLeft(), rect.topRight());
//    painter->setPen(dark);
//    painter->drawLine(rect.topLeft() + QPoint(1,1), rect.topRight() + QPoint(0,1));
//    QBrush bckbrush(Qt::SolidPattern);
    QColor baseColor = Utils::StyleHelper::baseColor();
    QLinearGradient bgrad(rect.topLeft(), rect.topRight());
    bgrad.setColorAt(0, QColor(baseColor.red(),baseColor.green(),baseColor.blue(),210));
    bgrad.setColorAt(0, QColor(baseColor.red(),baseColor.green(),baseColor.blue(),140));
//    bckbrush.setColor(Utils::StyleHelper::baseColor());
//    painter->fillRect(rect.adjusted(0, 0, 0, -1), bckbrush);
    painter->fillRect(rect.adjusted(0,0,0,-1), bgrad);
    //shadows
    painter->setPen(QColor(0, 0, 0, 110));
    painter->drawLine(rect.topLeft() + QPoint(1,-1), rect.topRight() - QPoint(0,1));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->setPen(QColor(0, 0, 0, 40));
    painter->drawLine(rect.topLeft(), rect.bottomLeft());

//    //highlights
//    painter->setPen(QColor(255, 255, 255, 40));
//    painter->drawLine(rect.topLeft() + QPoint(0, -2), rect.topRight() - QPoint(0,2));
//    painter->drawLine(rect.bottomLeft() + QPoint(0, 1), rect.bottomRight() + QPoint(0,1));
//    painter->setPen(QColor(255, 255, 255, 50));
//    painter->drawLine(rect.topLeft() + QPoint(0, 0), rect.topRight());
//    painter->drawLine(rect.topRight() + QPoint(0, 1), rect.bottomRight() - QPoint(0, 1));
//    painter->drawLine(rect.bottomLeft() + QPoint(0,-1), rect.bottomRight()-QPoint(0,1));

    painter->save();

//    rect.setWidth(m_tabWidth);

    if (selected) {
        //background
        painter->save();
        QLinearGradient grad(rect.topLeft(), rect.topRight());
        grad.setColorAt(0, QColor(255, 255, 255, 210));
        grad.setColorAt(1, QColor(255, 255, 255, 140));
        painter->fillRect(rect.adjusted(0, 0, 0, -1), grad);
        painter->restore();

        //shadows
        painter->setPen(QColor(0, 0, 0, 110));
        painter->drawLine(rect.topLeft() + QPoint(1,-1), rect.topRight() - QPoint(0,1));
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        painter->setPen(QColor(0, 0, 0, 40));
        painter->drawLine(rect.topLeft(), rect.bottomLeft());

        //highlights
        painter->setPen(QColor(255, 255, 255, 40));
        painter->drawLine(rect.topLeft() + QPoint(0, -2), rect.topRight() - QPoint(0,2));
        painter->drawLine(rect.bottomLeft() + QPoint(0, 1), rect.bottomRight() + QPoint(0,1));
        painter->setPen(QColor(255, 255, 255, 50));
        painter->drawLine(rect.topLeft() + QPoint(0, 0), rect.topRight());
        painter->drawLine(rect.topRight() + QPoint(0, 1), rect.bottomRight() - QPoint(0, 1));
        painter->drawLine(rect.bottomLeft() + QPoint(0,-1), rect.bottomRight()-QPoint(0,1));
    }

    QString tabText(this->text());

    QRect tabTextRect(rect);
    const bool drawIcon = rect.height() > 20;
    QRect tabIconRect(tabTextRect);
//    tabTextRect.translate(0, drawIcon ? -2 : 1);
    QFont boldFont(painter->font());
    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(selected ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 110));
    const int textFlags = Qt::AlignCenter | (drawIcon ? Qt::AlignHCenter : Qt::AlignVCenter) | Qt::TextWordWrap;
    if (enabled) {
        painter->drawText(tabTextRect, textFlags, tabText);
        painter->setPen(selected ? QColor(60, 60, 60) : Utils::StyleHelper::panelTextColor());
    } else {
//        painter->setPen(selected ? Utils::StyleHelper::panelTextColor() : QColor(255, 255, 255, 120));
        painter->setPen(selected ? QColor(0,0,0) : QColor(84,84,84));
    }
    if (!Utils::HostOsInfo::isMacHost() && !selected && enabled) {
        painter->save();
        int fader = int(m_fader);
        QLinearGradient grad(rect.topLeft(), rect.topRight());
        grad.setColorAt(0, Qt::transparent);
        grad.setColorAt(0.5, QColor(255, 255, 255, fader));
        grad.setColorAt(1, Qt::transparent);
        painter->fillRect(rect, grad);
        painter->setPen(QPen(grad, 1.0));
        painter->drawLine(rect.topLeft(), rect.topRight());
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        painter->restore();
    }

    if (!enabled)
        painter->setOpacity(0.7);

    if (drawIcon) {
        int textHeight = painter->fontMetrics().boundingRect(QRect(0, 0, width(), height()), Qt::TextWordWrap, tabText).height();
        tabIconRect.adjust(0, 4, 0, -textHeight);
        Utils::StyleHelper::drawIconWithShadow(this->icon(), tabIconRect, painter, enabled ? QIcon::Normal : QIcon::Disabled);
    }

    painter->translate(0, -1);
    painter->drawText(tabTextRect, textFlags, tabText);
    painter->restore();
}


////////////////////////////////////////


CustomToolBoxButton::CustomToolBoxButton(QWidget *parent)
        : QAbstractButton(parent),
          enabled(true),
          selected(false),
          tabbar(0),
          m_fader(0)
{
   setBackgroundRole(QPalette::Background);
   setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
   setFocusPolicy(Qt::NoFocus);
   animator.setPropertyName("fader");
   animator.setTargetObject(this);
}

QSize CustomToolBoxButton::sizeHint() const
{
    QSize iconSize(8, 8);
    if (!icon().isNull()) {
        int icone = style()->pixelMetric(QStyle::PM_SmallIconSize);
        iconSize += QSize(icone + 2, icone);
    }
    QSize textSize = fontMetrics().size(Qt::TextShowMnemonic, text()) + QSize(0, 8);

    QSize total(iconSize.width() + textSize.width(), qMax(iconSize.height(), textSize.height()));
    return total.expandedTo(QApplication::globalStrut());
}

QSize CustomToolBoxButton::minimumSizeHint() const
{
    if (icon().isNull())
        return QSize();
    int icone = style()->pixelMetric(QStyle::PM_SmallIconSize);
    return QSize(icone + 8, icone + 8);
}

void CustomToolBoxButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter paint(this);

    QString text = QAbstractButton::text();
    QPainter *p = &paint;
    const QPalette &pal = palette();
    QStyleOptionToolBox opt;
    opt.init(this);
    if (selected)
        opt.state |= QStyle::State_Selected;
    if (isDown())
        opt.state |= QStyle::State_Sunken;
    opt.text = text;
    opt.icon = icon();
    // Using CE_PushButton instead of CE_ToolBoxTab because this way looks the same on all platforms,
    // while, CE_ToolBoxTab looks awful on MAC OS X.
    style()->drawControl(QStyle::CE_PushButton, &opt, p, parentWidget());

    QPixmap pm = icon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize), isEnabled() ? QIcon::Normal : QIcon::Disabled);

    QRect cr = style()->subElementRect(QStyle::SE_ToolBoxTabContents, &opt, this);
    QRect tr, ir;
    int ih = 0;
    if (pm.isNull()) {
        tr = cr;
        tr.adjust(4, 0, -8, 0);
    } else {
        int iw = pm.width() + 4;
        ih = pm.height();
        ir = QRect(cr.left() + 4, cr.top(), iw + 2, ih);
        tr = QRect(ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height());
    }

    if (selected && style()->styleHint(QStyle::SH_ToolBox_SelectedPageTitleBold, &opt, this)) {
        QFont f(p->font());
        f.setBold(true);
        p->setFont(f);
    }

    QString txt;
    if (p->fontMetrics().width(text) < tr.width()) {
        txt = text;
    } else {
        txt = text.left(1);
        int ew = p->fontMetrics().width("...");
        int i = 1;
        while (p->fontMetrics().width(txt) + ew +
                p->fontMetrics().width(text[i])  < tr.width())
            txt += text[i++];
        txt += "...";
    }

    if (ih)
        p->drawPixmap(ir.left(), (height() - ih) / 2, pm);

    int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic;
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, 0, this))
        alignment |= Qt::TextHideMnemonic;
    style()->drawItemText(p, tr, alignment, pal, isEnabled(), txt, foregroundRole());

    if (!txt.isEmpty() && hasFocus()) {
        QStyleOptionFocusRect opt;
        opt.rect = tr;
        opt.palette = pal;
        opt.state = QStyle::State_None;
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p, this);
    }

    paintTab(&paint);
}
void CustomToolBoxButton::setSelected(bool b)
{
    selected = b;
    update();
}


CustomToolBox::CustomToolBox(QWidget *parent, Qt::WFlags f)
    : QFrame(parent,f)
{
   layout = new QVBoxLayout(this);
   layout->setMargin(0);
   layout->setSpacing(0);
   setBackgroundRole(QPalette::Button);
   currentPage=0;
}

CustomToolBox::~CustomToolBox()
{
}

CustomToolBox::Page *CustomToolBox::page(QWidget *widget) const
{
    if (!widget)
        return 0;

    for (QList<Page>::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i)
        if ((*i).widget == widget)
            return (Page*) &(*i);
    return 0;
}

CustomToolBox::Page *CustomToolBox::page(int index)
{
    if (index >= 0 && index < pageList.size())
        return &pageList[index];
    return 0;
}

const CustomToolBox::Page *CustomToolBox::page(int index) const
{
    if (index >= 0 && index < pageList.size())
        return &pageList.at(index);
    return 0;
}

void CustomToolBox::updateTabs()
{
    CustomToolBoxButton *lastButton = currentPage ? currentPage->button : 0;
    bool after = false;
    for (QList<Page>::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i) {
        CustomToolBoxButton *tB = (*i).button;
        QWidget *tW = (*i).widget;
        if (after) {
            QPalette p = tB->palette();
            p.setColor(tB->backgroundRole(), tW->palette().color(tW->backgroundRole()));
            tB->setPalette(p);
            tB->update();
        } else if (tB->backgroundRole() != QPalette::Background) {
            tB->setBackgroundRole(QPalette::Background);
            tB->update();
        }
        after = (*i).button == lastButton;
    }
}

int CustomToolBox::addItem(QWidget *item, const QString &text)
{
   return insertItem(-1, item, QIcon(), text);
}

int CustomToolBox::addItem(QWidget *item, const QIcon &iconSet,
                              const QString &text)
{
    return insertItem(-1, item, iconSet, text);
}

int CustomToolBox::insertItem(int index, QWidget *item, const QString &text)
{
   return insertItem(index, item, QIcon(), text);
}

int CustomToolBox::insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text)
{
    if (!widget)
        return -1;


    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));

    Page c;
    c.widget = widget;

    c.button = new CustomToolBoxButton(this);

    // Hide the standard text as its redrawn later.
    QPalette* b_pal = new QPalette();
    b_pal->setColor(QPalette::ButtonText,Qt::transparent);
//    b_pal->setColor(QPalette::Button,Qt::transparent);
    c.button->setPalette(*b_pal);

    connect(c.button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    c.sv = new QScrollArea(this);
    c.sv->setWidget(widget);
    c.sv->setWidgetResizable(true);
    c.sv->hide();
    c.sv->setFrameStyle(QFrame::NoFrame);
    c.sv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    c.setText(text);
    c.setIcon(icon);

    if(index < 0 || index >= (int)pageList.count()) {
        index = pageList.count();
        pageList.append(c);
        layout->addWidget(c.button);
        layout->addWidget(c.sv);
        if (index == 0) {
            setCurrentIndex(index);
        }
    } else {
        pageList.insert(index, c);
        relayout();
        if (currentPage) {
            QWidget *current = currentPage->widget;
            int oldindex = indexOf(current);
            if (index <= oldindex) {
                currentPage = 0; // trigger change
                setCurrentIndex(oldindex);
            }
        }
    }

    c.button->show();

    updateTabs();

    itemInserted(index);
    return index;
}

void CustomToolBox::buttonClicked()
{
    CustomToolBoxButton *tb = ::qobject_cast<CustomToolBoxButton*>(sender());
    QWidget* item = 0;
    for (QList<Page>::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i){
        if ((*i).button == tb) {
            item = (*i).widget;
            break;
        }
    }

    setCurrentIndex(indexOf(item));
}

/*!
    \property CustomToolBox::count
    \brief The number of items contained in the toolbox.
*/

int CustomToolBox::count() const
{
    return pageList.count();
}

void CustomToolBox::setCurrentIndex(int index)
{
    Page *c = page(index);
    if (!c || currentPage == c)
        return;

    c->button->setSelected(true);
    if (currentPage) {
        currentPage->sv->hide();
        currentPage->button->setSelected(false);
    }

    currentPage = c;

    currentPage->sv->show();

    updateTabs();

    emit currentChanged(index);
}

void CustomToolBox::relayout()
{
    delete layout;
    layout = new QVBoxLayout(this);
    for (QList<Page>::ConstIterator i = pageList.constBegin(); i != pageList.constEnd(); ++i) {
        layout->addWidget((*i).button);
        layout->addWidget((*i).sv);
    }
}

void CustomToolBox::widgetDestroyed(QObject *object)
{
    // no verification - vtbl corrupted already
    QWidget *p = (QWidget*)object;

    Page *c = page(p);
    if (!p || !c)
        return;

    layout->removeWidget(c->sv);
    layout->removeWidget(c->button);
    c->sv->deleteLater(); // page might still be a child of sv
    delete c->button;

    bool removeCurrent = c == currentPage;
    pageList.removeAll(*c);

    if (!pageList.count()) {
        currentPage = 0;
        emit currentChanged(-1);
    } else if (removeCurrent) {
        currentPage = 0;
        setCurrentIndex(0);
    }
}

/*!
    Removes the item at position \a index from the toolbox. Note that
    the widget is \e not deleted.
*/

void CustomToolBox::removeItem(int index)
{
    if (QWidget *w = widget(index)) {
        disconnect(w, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed(QObject*)));
        w->setParent(this);
        // destroy internal data
        widgetDestroyed(w);
        itemRemoved(index);
    }
}

/*!
    \property CustomToolBox::currentIndex
    \brief The index of the current item, or -1 if the toolbox is empty.

    \sa indexOf(), widget()
*/


int CustomToolBox::currentIndex() const
{
    return currentPage ? indexOf(currentPage->widget) : -1;
}

/*!
    Returns a pointer to the current widget, or 0 if there is no such item.

    \sa currentIndex(), setCurrentWidget()
*/

QWidget * CustomToolBox::currentWidget() const
{
    return currentPage ? currentPage->widget : 0;
}

/*!
  Makes\a widget the current widget. The \a widget must be an item in this tool box.

  \sa addItem(), setCurrentIndex(), currentWidget()
 */
void CustomToolBox::setCurrentWidget(QWidget *widget)
{
    Q_ASSERT_X(indexOf(widget) >= 0, "CustomToolBox::setCurrentWidget", "widget not contained in tool box");
    setCurrentIndex(indexOf(widget));
}
/*!
    Returns the widget at position \a index, or 0 if there is no such
    item.
*/

QWidget *CustomToolBox::widget(int index) const
{
    if (index < 0 || index >= (int) pageList.size())
        return 0;
    return pageList.at(index).widget;
}

/*!
    Returns the index of \a widget, or -1 if the item does not
    exist.
*/

int CustomToolBox::indexOf(QWidget *widget) const
{
    Page *c = page(widget);
    return c ? pageList.indexOf(*c) : -1;
}

/*!
    If \a enabled is true then the item at position \a index is enabled; otherwise
    the item at position \a index is disabled.
*/

void CustomToolBox::setItemEnabled(int index, bool enabled)
{
    Page *c = page(index);
    if (!c)
        return;

    c->button->setEnabled(enabled);
    if (!enabled && c == currentPage) {
        int curIndexUp = index;
        int curIndexDown = curIndexUp;
        const int count = pageList.count();
        while (curIndexUp > 0 || curIndexDown < count-1) {
            if (curIndexDown < count-1) {
                if (page(++curIndexDown)->button->isEnabled()) {
                    index = curIndexDown;
                    break;
                }
            }
            if (curIndexUp > 0) {
                if (page(--curIndexUp)->button->isEnabled()) {
                    index = curIndexUp;
                    break;
                }
            }
        }
        setCurrentIndex(index);
    }
}


/*!
    Sets the text of the item at position \a index to \a text.
*/

void CustomToolBox::setItemText(int index, const QString &text)
{
    Page *c = page(index);
    if (c)
        c->setText(text);
}

/*!
    Sets the icon of the item at position \a index to \a icon.
*/

void CustomToolBox::setItemIcon(int index, const QIcon &icon)
{
    Page *c = page(index);
    if (c)
        c->setIcon(icon);
}

/*!
    Sets the tooltip of the item at position \a index to \a toolTip.
*/

void CustomToolBox::setItemToolTip(int index, const QString &toolTip)
{
    Page *c = page(index);
    if (c)
        c->setToolTip(toolTip);
}

/*!
    Returns true if the item at position \a index is enabled; otherwise returns false.
*/

bool CustomToolBox::isItemEnabled(int index) const
{
    const Page *c = page(index);
    return c && c->button->isEnabled();
}

/*!
    Returns the text of the item at position \a index, or an empty string if
    \a index is out of range.
*/

QString CustomToolBox::itemText(int index) const
{
    const Page *c = page(index);
    return (c ? c->text() : QString());
}

/*!
    Returns the icon of the item at position \a index, or a null
    icon if \a index is out of range.
*/

QIcon CustomToolBox::itemIcon(int index) const
{
    const Page *c = page(index);
    return (c ? c->icon() : QIcon());
}

/*!
    Returns the tooltip of the item at position \a index, or an
    empty string if \a index is out of range.
*/

QString CustomToolBox::itemToolTip(int index) const
{
    const Page *c = page(index);
    return (c ? c->toolTip() : QString());
}

/*! \reimp */
void CustomToolBox::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}

/*! \reimp */
void CustomToolBox::changeEvent(QEvent *ev)
{
    if(ev->type() == QEvent::StyleChange)
        updateTabs();
    QFrame::changeEvent(ev);
}

/*!
  This virtual handler is called after a new item was added or
  inserted at position \a index.

  \sa itemRemoved()
 */
void CustomToolBox::itemInserted(int index)
{
    Q_UNUSED(index)
}

/*void CustomToolBox::currentChanged(int index)
{
   Q_UNUSED(index)
}*/

/*!
  This virtual handler is called after an item was removed from
  position \a index.

  \sa itemInserted()
 */
void CustomToolBox::itemRemoved(int index)
{
    Q_UNUSED(index)
}

/*! \reimp */
bool CustomToolBox::event(QEvent *e)
{
    return QFrame::event(e);
}
