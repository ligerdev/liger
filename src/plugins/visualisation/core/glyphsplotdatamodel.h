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
#ifndef GLYPHSPLOTDATAMODEL_H
#define GLYPHSPLOTDATAMODEL_H

#include <visualisation/core/visualisationdatamodel.h>

namespace Visualisation {

class GlyphsPlotDataModel : public VisualisationDataModel
{
    Q_OBJECT

public:
    explicit GlyphsPlotDataModel(QObject *parent = nullptr);
};

} // namespace Visualisation
#endif // GLYPHSPLOTDATAMODEL_H
