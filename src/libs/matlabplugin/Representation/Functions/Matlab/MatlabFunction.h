/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#pragma once

#include <tigon/tigon_global.h>
#include <tigon/tigonengineregistry.h>

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Elements/IElement.h>

namespace Tigon {
class MatlabEngine;

namespace Representation {

const TString HEADER_LABEL("%#");

/*!
 * \brief Matlab function template contains two parts:
 *   - "The Header". Start with special characters and end with special characters.
 *     This should contain all information needed in defineIOPrpts.
 *   - "The Boday". The actual implementation part. This part should contain
 *     a function whcih has the same name as the M-file and this function should
 *     be used to perform the function evaluation.
 *   - Supported element types: real, int, ordinal, nominal
 */
class LIGER_TIGON_EXPORT MatlabFunction : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::MatlabFunction)

public:
    MatlabFunction();
    ~MatlabFunction();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);
    void batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
                       const TVector<TVector<IElementSPtr> > &outputs);

    /// Accessor Functions
    /*!
     * \brief Define the path to a Matlab file that contains the evaluation function.
     * The file name and the function name **must** be the same.
     * \param path
     */
    void definePath(const TString &path);

    /*!
     * \brief defineCommand function is used when directly injecting the
     * function evaluation command
     * \param command
     */
    void defineCommand (const TString &command);
    TString command() const;                            // M-function command

    void defineInteractive(const bool visible);         // Engine visibility for interactivity
    bool interactive() const;

private:
    /// I/O Property arrays handling
    // These are part of the IFunction interface. If the user attempts to set the number of Inputs or Outputs we
    // ignore them and find out the true number of inputs and outputs. If this happens before a command is given
    // the inputs and outputs remain empty.
    void defineInputPrpts();
    void defineOutputPrpts();
    void defineIOPrpts();
    //TString readMxStringField(mxArray* array, TString fieldName, int index = 0);// Reads a string from the property struct
    //double readMxRealField(mxArray* array, TString fieldName, int index = 0);    // Read a number field in the property struct
    ElementType stringToElemTypeMATLAB(const TString &type);

    bool m_engineVisible;       // class owned, determines if the MATLAB engine should be visible

    TString m_workDir;          // work directory, if m_workDir is not empty, Matlab will work in this directory
    TString m_command;          // The m-function to be called
};

} // namepsace Representation
} // namespace Tigon

FUNCTION_FACTORY(MatlabFunction)
