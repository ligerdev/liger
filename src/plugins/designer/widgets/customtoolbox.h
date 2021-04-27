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
#ifndef CUSTOMTOOLBOX_H
#define CUSTOMTOOLBOX_H

#include <QFrame>
#include <QIcon>
#include <QLayout>
#include <QStyle>
#include <QList>
#include <QEventLoop>
#include <QScrollArea>
#include <QAbstractButton>
#include <QPropertyAnimation>

namespace Designer {

class CustomToolBoxButton : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(float fader READ fader WRITE setFader)
public:
    CustomToolBoxButton(QWidget *parent);


    float fader() { return m_fader; }
    void setFader(float value);

    void fadeIn();
    void fadeOut();

    bool    enabled;
//    QIcon   icon;
//    QString text;
//    QString toolTip;

    void setSelected(bool b);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void paintEvent(QPaintEvent *event);
    void paintTab(QPainter *painter) const;
//    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

protected:
//    void paintEvent(QPaintEvent *);

private:
    bool selected;

    QPropertyAnimation animator;
    QWidget* tabbar;
    float    m_fader;
};

class CustomToolBox : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
    Q_PROPERTY(int count READ count)

public:
    CustomToolBox(QWidget *parent = 0, Qt::WFlags f = 0);
    ~CustomToolBox();

    int addItem(QWidget *widget, const QString &text);
    int addItem(QWidget *widget, const QIcon &icon, const QString &text);
    int insertItem(int index, QWidget *widget, const QString &text);
    int insertItem(int index, QWidget *widget, const QIcon &icon, const QString &text);

    void removeItem(int index);

    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void setItemText(int index, const QString &text);
    QString itemText(int index) const;

    void setItemIcon(int index, const QIcon &icon);
    QIcon itemIcon(int index) const;

    void setItemToolTip(int index, const QString &toolTip);
    QString itemToolTip(int index) const;

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;


public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

Q_SIGNALS:
    void currentChanged(int index);

private slots:
    void buttonClicked();
    void widgetDestroyed(QObject*);

protected:
    bool event(QEvent *e);
    virtual void itemInserted(int index);
    virtual void itemRemoved(int index);
    void showEvent(QShowEvent *e);
    void changeEvent(QEvent *);

private:
    struct Page
    {
        CustomToolBoxButton *button;
        QScrollArea *sv;
        QWidget *widget;

        inline void setText(const QString &text) { button->setText(text); }
        inline void setIcon(const QIcon &is) { button->setIcon(is); }
        inline void setToolTip(const QString &tip) { button->setToolTip(tip); }
        inline QString text() const { return button->text(); }
        inline QIcon icon() const { return button->icon(); }
        inline QString toolTip() const { return button->toolTip(); }

        inline bool operator==(const Page& other) const
        {
            return widget == other.widget;
        }
    };

    QList<Page> pageList;

    Page *page(QWidget *widget) const;
    const Page *page(int index) const;
    Page *page(int index);
    Page *currentPage;

    void updateTabs();
    void relayout();

    QVBoxLayout *layout;

};


} // namespace Core

#endif // CUSTOMTOOLBOX_H
