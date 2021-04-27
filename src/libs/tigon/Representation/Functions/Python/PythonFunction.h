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
#pragma once
#include <tigon/tigon_global.h>
#include <tigon/Representation/Functions/IFunction.h>

namespace Tigon {
namespace Representation {

const TString PYHEADER_LABEL("##");

class LIGER_TIGON_EXPORT PythonFunction : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::PythonFunction)

public:
    PythonFunction();
    ~PythonFunction();

    /*!
     * \brief Define the path to the python template function
     * \param path to the python file
     */
    void definePath(const TString &path);
    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    void batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
                       const TVector<TVector<IElementSPtr> > &outputs);

    TString pythonFunctionName() const;
    TString pythonFunctionDirectory() const;

protected:
    // Default properties defined by the concrete sub-classes
    void defineInputPrpts();
    void defineOutputPrpts();

private:
    void loadProperties(const TString &path);
    ElementType stringToElemTypePython(const TString &type);
    TString m_funcName;
    TString m_directory;

};
}
}
