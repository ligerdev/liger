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
#include <visualisation/core/visualisationviewmodel.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <QWebChannel>

using namespace Visualisation;

VisualisationViewModel::VisualisationViewModel(QWidget *parent)
    : QWebEngineView(parent)
    , m_dataModel(0)
{

}

void VisualisationViewModel::setDataModel(VisualisationDataModel *dataModel)
{
    m_dataModel = dataModel;
}

void VisualisationViewModel::linkWithJavaScript()
{
    if(m_dataModel) {
        QWebChannel* channel = new QWebChannel(this);
        channel->registerObject(QStringLiteral("dataModel"), m_dataModel);
        this->page()->setWebChannel(channel);
    }
    return;
}

VisualisationViewModel::~VisualisationViewModel()
{

}

VisualisationDataModel *VisualisationViewModel::dataModel() const
{
    return m_dataModel;
}
