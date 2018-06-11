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
#include <designer/iengine/icountinglabel.h>
#include <QMouseEvent>

namespace Designer {

static const int LABEL_HEIGHT = 15;
//static const int DEFAULT_FONTSIZE = 9;
static const int DEFAULT_LABEL_WIDTH = 120;

ICountingLabel::ICountingLabel(qint64 count)
    : m_maxCount(0)
    , m_currentCount(count)
    , m_labelFormat(LABEL_FORMAT::CountOnly)
{
    setAlignment(Qt::AlignCenter);
    updateCount(m_currentCount);
    setStyleSheet("color: white;");
    //QFont font = this->font();
    //font.setPointSize(DEFAULT_FONTSIZE);
    //setFont(font);
    setMargin(0);
    setFixedHeight(LABEL_HEIGHT);
    setFixedWidth(DEFAULT_LABEL_WIDTH);
}

void ICountingLabel::updateCount(int count)
{
    m_currentCount = count;

    QString content;
    switch(m_labelFormat) {
    case LABEL_FORMAT::CountVsMax:
        content = QString::number(count) + "/" + QString::number(m_maxCount);
        break;
    case LABEL_FORMAT::Percentage:
        content = QString::number(qint64(100*count*1.0/m_maxCount)) + "%";
        break;
    case LABEL_FORMAT::CountOnly:
        content = QString::number(count);
        break;
    default:
        content = QString::number(count);
        break;
    }

    int textSpace = this->rect().width()-8;
    content = this->fontMetrics().elidedText(content, Qt::ElideRight, textSpace);
    this->setText(content);
}

void ICountingLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        int formatInt = int(m_labelFormat);
        ++formatInt;
        if( formatInt > LABEL_FORMAT::Percentage) {
            m_labelFormat = LABEL_FORMAT::CountOnly;
        } else {
            m_labelFormat = static_cast<LABEL_FORMAT>(formatInt);
        }
        setLabelFormat(m_labelFormat);
    }
}

qint64 ICountingLabel::currentCount() const
{
    return m_currentCount;
}

qint64 ICountingLabel::maxCount() const
{
    return m_maxCount;
}

void ICountingLabel::setMaxCount(const qint64 &maxCount)
{
    m_maxCount = maxCount;
}

LABEL_FORMAT ICountingLabel::labelFormat() const
{
    return m_labelFormat;
}

void ICountingLabel::setLabelFormat(const LABEL_FORMAT &labelFormat)
{
    m_labelFormat = labelFormat;
}

}
