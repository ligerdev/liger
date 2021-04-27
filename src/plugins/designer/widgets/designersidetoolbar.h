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
#ifndef DESIGNERSIDETOOLBAR_H
#define DESIGNERSIDETOOLBAR_H

#include <QIcon>
#include <QWidget>
#include <QAction>
#include <QTimer>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class QPainter;
class QStackedLayout;
class QStatusBar;
QT_END_NAMESPACE

namespace Designer {

class FancySideTab : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float fader READ fader WRITE setFader)
public:
    FancySideTab(QWidget *tabbar) :
        enabled(false),
        tabbar(tabbar),
        m_fader(0)
    {
        animator.setPropertyName("fader");
        animator.setTargetObject(this);
    }
    FancySideTab(QWidget *tabbar, QString tooltip) :
        enabled(false),
        toolTip(tooltip),
        tabbar(tabbar),
        m_fader(0)
    {
        animator.setPropertyName("fader");
        animator.setTargetObject(this);
    }
    float fader() { return m_fader; }
    void setFader(float value);

    void fadeIn();
    void fadeOut();

    bool    enabled;
    QIcon   icon;
    QString text;
    QString toolTip;    

private:
    QPropertyAnimation animator;
    QWidget* tabbar;
    float    m_fader;
};

class DesignerSideToolBarPrivate : public QWidget
{
    Q_OBJECT

public:
    DesignerSideToolBarPrivate(QWidget *parent = 0);
    ~DesignerSideToolBarPrivate();

    bool event(QEvent *event);

    void paintEvent(QPaintEvent *event);
    void paintTab(QPainter *painter, int tabIndex) const;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void insertTab(int index, const QIcon &icon, const QString &label) {
        FancySideTab *tab = new FancySideTab(this);
        tab->icon = icon;
        tab->text = label;
        m_tabs.insert(index, tab);        
    }
    void setEnabled(int index, bool enabled);
    void removeTab(int index) {
        FancySideTab *tab = m_tabs.takeAt(index);
        delete tab;
    }
    void setCurrentIndex(int index);
    int currentIndex() const { return m_currentIndex; }

    void setTabToolTip(int index, QString toolTip) { m_tabs[index]->toolTip = toolTip; }
    QString tabToolTip(int index) const { return m_tabs.at(index)->toolTip; }

    QIcon tabIcon(int index) const { return m_tabs.at(index)->icon; }
    QString tabText(int index) const { return m_tabs.at(index)->text; }
    int count() const {return m_tabs.count(); }
    QRect tabRect(int index) const;

    bool isLinkChecked()      { return m_linkChecked; }

signals:
    void currentChanged(int);

public slots:
    void emitCurrentIndex();

private:
    static const int m_rounding;
    static const int m_textPadding;
    static const int m_tabWidth;
    QRect m_hoverRect;
    int m_hoverIndex;
    int m_currentIndex;
    QList<FancySideTab*> m_tabs;
    QTimer m_triggerTimer;
    QSize tabSizeHint(bool minimum = false) const;

    bool m_linkChecked;
};

class DesignerSideToolBar : public QWidget
{
    Q_OBJECT

public:
    DesignerSideToolBar(QWidget *parent = 0);

    void insertTab(int index, const QIcon &icon, const QString &label);
    void removeTab(int index);
    void setBackgroundBrush(const QBrush &brush);
    void addCornerWidget(QWidget *widget);
    void insertCornerWidget(int pos, QWidget *widget);
    int  cornerWidgetCount() const;
    void setTabToolTip(int index, const QString &toolTip);

    void paintEvent(QPaintEvent *event);

    int currentIndex() const;
    QStatusBar *statusBar() const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    bool isLinkChecked()      { return d->isLinkChecked(); }

signals:
    void currentAboutToShow(int index);
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);
    void setSelectionWidgetHidden(bool hidden);

private slots:
    void showWidget(int index);

private:
    DesignerSideToolBarPrivate *d;
    QWidget *m_cornerWidgetContainer;
    QWidget *m_selectionWidget;
    QStatusBar *m_statusBar;
};

} // namespace Core

#endif // DESIGNERSIDETOOLBAR
