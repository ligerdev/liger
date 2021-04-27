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
#ifndef VISUALISATIONVIEWMODEL_H
#define VISUALISATIONVIEWMODEL_H

#include <QWebEngineView>
#include <QString>

namespace Visualisation {
class VisualisationDataModel;

/*!
 * \brief The View Model
 */
class VisualisationViewModel : public QWebEngineView
{

public:
    explicit VisualisationViewModel(QWidget *parent = nullptr);
    virtual void setDataModel(VisualisationDataModel* dataModel);
    VisualisationDataModel *dataModel() const;
    virtual void linkWithJavaScript();
    virtual ~VisualisationViewModel();

private:
    VisualisationDataModel* m_dataModel;
};
}
#endif // VISUALISATIONVIEWMODEL_H
