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
#ifndef PARAMCONNECT_H
#define PARAMCONNECT_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ParamConnect
{
public:
    explicit ParamConnect();
    ~ParamConnect();

    void setIsConnected(bool isConnected);
    void setOutputIdx(size_t outputIdx);
    void setOutputType(OutputType outputType);
    void setIsMaximization(bool isMaximization);

    bool isConnected() const;
    size_t outputIdx() const;
    OutputType outputType() const;
    bool isMaximization() const;

private:
    bool m_isConnected;
    size_t m_outputIdx;
    OutputType m_outputType;
    bool m_isMaximization;
};

} // namespace Representation
} // namespace Tigon

#endif // PARAMCONNECT_H
