#ifndef STYLEDBARVERTICAL_H
#define STYLEDBARVERTICAL_H

#include "utils_global.h"

#include <QWidget>

namespace Utils {

class LIGER_UTILS_EXPORT StyledBarVertical : public QWidget
{
    Q_OBJECT
public:
    StyledBarVertical(QWidget *parent = 0);
    void setSingleColumn(bool singleColumn);
    bool isSingleColumn() const;

    void setLightColored(bool lightColored);
    bool isLightColored() const;

protected:
    void paintEvent(QPaintEvent *event);
};

class LIGER_UTILS_EXPORT VStyledSeparator : public QWidget
{
    Q_OBJECT
public:
    VStyledSeparator(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *event);
};

} // Utils

#endif // STYLEDBARVERTICAL_H
