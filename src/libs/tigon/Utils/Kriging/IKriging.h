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
#ifndef IKRIGING_H
#define IKRIGING_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {

class LIGER_TIGON_EXPORT IKriging
{
public:
    IKriging();
    virtual ~IKriging();

    virtual double interpolate(const TVector<double> & xstar) = 0;
    virtual double errorEstimate() = 0;

    virtual TVector<TVector<double> > inputData()  const = 0;
    virtual TVector<double>           outputData() const = 0;

    void defineEstimateErrors(bool est);
    bool estimateErrors()         const;

private:
    // Indicates if the error of the predictions are to be estimated or not
    bool  m_estimateErrors;
};

} // namespace Tigon

#endif // IKRIGING_H
