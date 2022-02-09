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
#include "designersidetoolbar.h"

//#include "coreconstants.h"
#include <utils/hostosinfo.h>
#include <utils/stylehelper.h>
#include <utils/styledbar.h>

#include <QDebug>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
//#include <QWindowsStyle>
#include <QStyleFactory>
#include <QPainter>
#include <QSplitter>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolButton>
#include <QToolTip>
#include <QAnimationGroup>
#include <QPropertyAnimation>

using namespace Designer;
//using namespace Internal;

const int DesignerSideToolBarPrivate::m_rounding    = 22;
const int DesignerSideToolBarPrivate::m_textPadding = 4;
const int DesignerSideToolBarPrivate::m_tabWidth    = 30;
const int m_tabWidth = 30;
void FancySideTab::fadeIn()
{
    animator.stop();
    animator.setDuration(80);
    animator.setEndValue(40);
    animator.start();
}

void FancySideTab::fadeOut()
{
    animator.stop();
    animator.setDuration(160);
    animator.setEndValue(0);
    animator.start();
}

void FancySideTab::setFader(float value)
{
    m_fader = value;
    tabbar->update();
}

DesignerSideToolBarPrivate::DesignerSideToolBarPrivate(QWidget *parent)
    : QWidget(parent),
      m_linkChecked(false)
{
    m_hoverIndex = -1;
    m_currentIndex = -1;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setStyle(QStyleFactory::create(QLatin1String("windows")));
    //    setMinimumWidth(qMax(2 * m_rounding, 40));
    setMaximumWidth(30);//qMax(2 * m_rounding, 40));
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true); // Needed for hover events
    m_triggerTimer.setSingleShot(true);

    // We use a zerotimer to keep the sidebar responsive
    connect(&m_triggerTimer, SIGNAL(timeout()), this, SLOT(emitCurrentIndex()));
}

DesignerSideToolBarPrivate::~DesignerSideToolBarPrivate()
{
    delete style();
}

QSize DesignerSideToolBarPrivate::tabSizeHint(bool minimum) const
{
    QFont boldFont(font());
    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    QFontMetrics fm(boldFont);
    int spacing = 8;
    //    int width = 60 + spacing + 2;
    // Ugly hack.
    //    int maxLabelwidth = m_tabWidth;
    //    for (int tab=0 ; tab<count() ;++tab) {
    //        int width = fm.width(tabText(tab));
    //        if (width > maxLabelwidth)
    //            maxLabelwidth = width;
    //    }
    int iconHeight = minimum ? 0 : 32;
    //    return QSize(qMax(width, maxLabelwidth + 4), iconHeight + spacing + fm.height());
    return QSize(m_tabWidth, iconHeight + spacing + fm.height());
}

void DesignerSideToolBarPrivate::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);

    for (int i = 0; i < count(); ++i)
        if (i != currentIndex())
            paintTab(&p, i);

    // paint active tab last, since it overlaps the neighbors
    if (currentIndex() != -1)
        paintTab(&p, currentIndex());
}

// Handle hover events for mouse fade ins
void DesignerSideToolBarPrivate::mouseMoveEvent(QMouseEvent *e)
{
    int newHover = -1;
    for (int i = 0; i < count(); ++i) {
        QRect area = tabRect(i);
        if (area.contains(e->pos())) {
            newHover = i;
            break;
        }
    }
    if (newHover == m_hoverIndex)
        return;

    if (validIndex(m_hoverIndex))
        m_tabs[m_hoverIndex]->fadeOut();

    m_hoverIndex = newHover;

    if (validIndex(m_hoverIndex)) {
        m_tabs[m_hoverIndex]->fadeIn();
        m_hoverRect = tabRect(m_hoverIndex);
    }
}

bool DesignerSideToolBarPrivate::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        if (validIndex(m_hoverIndex)) {
            QString tt = tabToolTip(m_hoverIndex);
            if (!tt.isEmpty()) {
                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tt, this);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

// Resets hover animation on mouse enter
void DesignerSideToolBarPrivate::enterEvent(QEvent *e)
{
    Q_UNUSED(e)
    m_hoverRect = QRect();
    m_hoverIndex = -1;
}

// Resets hover animation on mouse enter
void DesignerSideToolBarPrivate::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)
    m_hoverIndex = -1;
    m_hoverRect = QRect();
    for (int i = 0 ; i < m_tabs.count() ; ++i) {
        m_tabs[i]->fadeOut();
    }
}

QSize DesignerSideToolBarPrivate::sizeHint() const
{
    QSize sh = tabSizeHint();
    return QSize(sh.width(), sh.height() * m_tabs.count());
}

QSize DesignerSideToolBarPrivate::minimumSizeHint() const
{
    QSize sh = tabSizeHint(true);
    return QSize(sh.width(), sh.height() * m_tabs.count());
}

QRect DesignerSideToolBarPrivate::tabRect(int index) const
{
    QSize sh = tabSizeHint();

    if (sh.height() * m_tabs.count() > height())
        sh.setHeight(height() / m_tabs.count());

    return QRect(0, index * sh.height(), sh.width(), sh.height());

}

// This keeps the sidebar responsive since
// we get a repaint before loading the
// mode itself
void DesignerSideToolBarPrivate::emitCurrentIndex()
{
    emit currentChanged(m_currentIndex);
}

void DesignerSideToolBarPrivate::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    for (int index = 0; index < m_tabs.count(); ++index) {
        if (tabRect(index).contains(e->pos())) {
            if (isTabEnabled(index)) {
                m_currentIndex = index;
                update();
                m_triggerTimer.start(0);
            }
            if (isTabEnabled(index) && index==0) {
                m_linkChecked      = false;
            } else if (isTabEnabled(index) && index==1) {
                m_linkChecked      = true;
            } else {
                m_linkChecked      = false;
            }
            break;
        }
    }
}

void DesignerSideToolBarPrivate::paintTab(QPainter *painter, int tabIndex) const
{
    if (!validIndex(tabIndex)) {
        qWarning("invalid index");
        return;
    }
    painter->save();

    QRect rect = tabRect(tabIndex);
    rect.setWidth(m_tabWidth);
    bool selected = (tabIndex == m_currentIndex);
    bool enabled = isTabEnabled(tabIndex);

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

    QString tabText(this->tabText(tabIndex));
    QRect tabTextRect(rect);
    const bool drawIcon = rect.height() > 20;
    QRect tabIconRect(tabTextRect);
    //    tabTextRect.translate(0, drawIcon ? -2 : 1);
    QFont boldFont(painter->font());
    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(selected ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 110));
    const int textFlags = Qt::AlignCenter | (drawIcon ? Qt::AlignBottom : Qt::AlignVCenter) | Qt::TextWordWrap;
    if (enabled) {
        painter->drawText(tabTextRect, textFlags, tabText);
        painter->setPen(selected ? QColor(60, 60, 60) : Utils::StyleHelper::panelTextColor());
    } else {
        painter->setPen(selected ? Utils::StyleHelper::panelTextColor() : QColor(255, 255, 255, 120));
    }
    if (!Utils::HostOsInfo::isMacHost() && !selected && enabled) {
        painter->save();
        int fader = int(m_tabs[tabIndex]->fader());
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
        Utils::StyleHelper::drawIconWithShadow(tabIcon(tabIndex), tabIconRect, painter, enabled ? QIcon::Normal : QIcon::Disabled);
    }

    painter->translate(0, -1);
    painter->drawText(tabTextRect, textFlags, tabText);
    painter->restore();
}

void DesignerSideToolBarPrivate::setCurrentIndex(int index) {
    if (isTabEnabled(index)) {
        m_currentIndex = index;
        update();
        emit currentChanged(m_currentIndex);
    }
}

void DesignerSideToolBarPrivate::setTabEnabled(int index, bool enable)
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0) {
        m_tabs[index]->enabled = enable;
        update(tabRect(index));
    }
}

bool DesignerSideToolBarPrivate::isTabEnabled(int index) const
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0)
        return m_tabs[index]->enabled;

    return false;
}


//////
// FancyColorButton
//////

class FancyColorButton : public QWidget
{
public:
    FancyColorButton(QWidget *parent)
        : m_parent(parent)
    {
        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    }

    void mousePressEvent(QMouseEvent *ev)
    {
        if (ev->modifiers() & Qt::ShiftModifier) {
            QColor color = QColorDialog::getColor(Utils::StyleHelper::requestedBaseColor(), m_parent);
            if (color.isValid())
                Utils::StyleHelper::setBaseColor(color);
        }
    }
private:
    QWidget *m_parent;
};

//////
// DesignerSideToolBar
//////

DesignerSideToolBar::DesignerSideToolBar(QWidget *parent)
    : QWidget(parent)
{
    d = new DesignerSideToolBarPrivate(this);

    m_selectionWidget = new QWidget(this);
    QVBoxLayout *selectionLayout = new QVBoxLayout;
    selectionLayout->setSpacing(0);
    selectionLayout->setMargin(0);
    //    selectionLayout->setSizeConstraint(QLayout::SetFixedSize);

    //    Utils::StyledBar *bar = new Utils::StyledBar;
    //    QHBoxLayout *layout = new QHBoxLayout(bar);
    //    layout->setMargin(0);
    //    layout->setSpacing(0);
    //    layout->addWidget(new FancyColorButton(this));
    //    selectionLayout->addWidget(bar);

    selectionLayout->addWidget(d, 1);
    m_selectionWidget->setLayout(selectionLayout);
    m_selectionWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    insertTab(0, QIcon(":/designer/images/cursor-mouse.png"),"");
    insertTab(1, QIcon(":/designer/images/link.png"),"");
    setTabEnabled(0, true);
    setTabEnabled(1, true);
    setTabToolTip(0, "Selection");
    setTabToolTip(1, "Add Link");

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(m_selectionWidget);
    //    mainLayout->addLayout(vlayout);
    setLayout(mainLayout);

    // Keep the width fixed.
    this->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    this->setMinimumWidth(m_tabWidth);

    connect(d, SIGNAL(currentChanged(int)), this, SLOT(showWidget(int)));
}

void DesignerSideToolBar::setSelectionWidgetHidden(bool hidden) {
    m_selectionWidget->setHidden(hidden);
}

void DesignerSideToolBar::insertTab(int index, const QIcon &icon, const QString &label)
{
    //    m_modesStack->insertWidget(index, tab);
    d->insertTab(index, icon, label);
}

void DesignerSideToolBar::removeTab(int index)
{
    //    m_modesStack->removeWidget(m_modesStack->widget(index));
    d->removeTab(index);
}

void DesignerSideToolBar::setBackgroundBrush(const QBrush &brush)
{
    QPalette pal = d->palette();
    pal.setBrush(QPalette::Mid, brush);
    d->setPalette(pal);
    pal = m_cornerWidgetContainer->palette();
    pal.setBrush(QPalette::Mid, brush);
    m_cornerWidgetContainer->setPalette(pal);
}

void DesignerSideToolBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);

    // Ugly hack to set tab width.
    m_selectionWidget->rect().setWidth(m_tabWidth);

    QRect rect = m_selectionWidget->rect().adjusted(0, 0, 1, 0);

    rect = style()->visualRect(layoutDirection(), geometry(), rect);
    Utils::StyleHelper::verticalGradient(&painter, rect, rect);
    painter.setPen(Utils::StyleHelper::borderColor());
    //    qDebug() << "rect.topRight(): "    << rect.topRight();
    //    qDebug() << "rect.bottomRight(): " << rect.bottomRight();
    painter.drawLine(rect.topRight(), rect.bottomRight());

    QColor light = Utils::StyleHelper::sidebarHighlight();
    painter.setPen(light);
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());
}

void DesignerSideToolBar::insertCornerWidget(int pos, QWidget *widget)
{
    QVBoxLayout *layout = static_cast<QVBoxLayout *>(m_cornerWidgetContainer->layout());
    layout->insertWidget(pos, widget);
}

int DesignerSideToolBar::cornerWidgetCount() const
{
    return m_cornerWidgetContainer->layout()->count();
}

void DesignerSideToolBar::addCornerWidget(QWidget *widget)
{
    m_cornerWidgetContainer->layout()->addWidget(widget);
}

int DesignerSideToolBar::currentIndex() const
{
    return d->currentIndex();
}

QStatusBar *DesignerSideToolBar::statusBar() const
{
    return m_statusBar;
}

void DesignerSideToolBar::setCurrentIndex(int index)
{
    if (d->isTabEnabled(index))
        d->setCurrentIndex(index);
}

void DesignerSideToolBar::showWidget(int index)
{
    emit currentAboutToShow(index);
    //    m_modesStack->setCurrentIndex(index);
    emit currentChanged(index);
}

void DesignerSideToolBar::setTabToolTip(int index, const QString &toolTip)
{
    d->setTabToolTip(index, toolTip);
}

void DesignerSideToolBar::setTabEnabled(int index, bool enable)
{
    d->setTabEnabled(index, enable);
}

bool DesignerSideToolBar::isTabEnabled(int index) const
{
    return d->isTabEnabled(index);
}
