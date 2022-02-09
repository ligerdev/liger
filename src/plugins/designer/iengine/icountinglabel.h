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
#ifndef ICOUNTINGLABEL_H
#define ICOUNTINGLABEL_H
#include <QObject>
#include <QLabel>

namespace Designer {

// Label format
enum LABEL_FORMAT {
    CountOnly  = 0,
    CountVsMax = 1,
    Percentage = 2
};

class ICountingLabel : public QLabel
{
public:
    explicit ICountingLabel(qint64 count = 0);

    LABEL_FORMAT labelFormat() const;
    void setLabelFormat(const LABEL_FORMAT &labelFormat);

    qint64 maxCount() const;
    void setMaxCount(const qint64 &maxCount);

    qint64 currentCount() const;

public slots:
    void updateCount(int count);

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    qint64 m_maxCount;
    qint64 m_currentCount;
    LABEL_FORMAT m_labelFormat;
};

}

#endif // ICOUNTINGLABEL_H
