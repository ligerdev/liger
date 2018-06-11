#include "styledbarvertical.h"

#include "stylehelper.h"

#include <QVariant>
#include <QPainter>
#include <QPixmapCache>
#include <QStyle>
#include <QStyleOption>

using namespace Utils;

StyledBarVertical::StyledBarVertical(QWidget *parent)
    : QWidget(parent)
{
    setProperty("panelwidget", true);
    setProperty("panelwidget_singlecolumn", true);
    setProperty("lightColored", false);
}

void StyledBarVertical::setSingleColumn(bool singleColumn)
{
    setProperty("panelwidget_singlecolumn", singleColumn);
}

bool StyledBarVertical::isSingleColumn() const
{
    return property("panelwidget_singlecolumn").toBool();
}

void StyledBarVertical::setLightColored(bool lightColored)
{
    setProperty("lightColored", lightColored);
}

bool StyledBarVertical::isLightColored() const
{
    return property("lightColored").toBool();
}

void StyledBarVertical::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QStyleOption option;
    option.rect = rect();
//    option.state = QStyle::State_Horizontal;
    style()->drawControl(QStyle::CE_ToolBar, &option, &painter, this);
}

VStyledSeparator::VStyledSeparator(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(10);
}

void VStyledSeparator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QStyleOption option;
    option.rect = rect();
//    option.state = QStyle::State_Horizontal;
    option.palette = palette();
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &option, &painter, this);
}
