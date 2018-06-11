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
#ifndef FEEDFORWARDSURROAGTE_H
#define FEEDFORWARDSURROAGTE_H

#include <tigon/tigon_global.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Functions/FannNetwork.h>

namespace Tigon {
namespace Representation {


class LIGER_TIGON_EXPORT FeedforwardSurrogate : public IFunction
{
public:
    FeedforwardSurrogate();
    FeedforwardSurrogate(TString networkPath);
    ~FeedforwardSurrogate();

    void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs);

    void defineNetwork(FannNetwork* network);
    FannNetwork* network();

protected:
    void defineInputPrpts();
    void defineOutputPrpts();

    FannNetwork* m_network;

};

} // namespace Representation
} // namespace Tigon

#endif // FEEDFORWARDSURROAGTE_H
