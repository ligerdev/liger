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
#ifndef DENORMALISATION_H
#define DENORMALISATION_H
#include <tigon/Operators/Transformations/ITransformation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT Denormalisation : public ITransformation
{

//    Q_PROPERTY(NormalisationBoundsType InputBoundsType
//               READ                    TP_inputBoundsType
//               WRITE                   TP_defineInputBoundsType)
//    Q_PROPERTY(NormalisationBoundsType OutputBoundsType
//               READ                    TP_outputBoundsType
//               WRITE                   TP_defineOutputBoundsType)
    DECLARE_CLASS(Tigon::Operators::Denormalisation)

public:
    Denormalisation();
    Denormalisation(Representation::IPSet* ipset);
    virtual ~Denormalisation();


    void evaluateNode();

    // Properties
    NormalisationBoundsType  TP_inputBoundsType()       const;
    NormalisationBoundsType  TP_outputBoundsType()      const;
    void TP_defineInputBoundsType( NormalisationBoundsType t);
    void TP_defineOutputBoundsType(NormalisationBoundsType t);

    // General functions
    BoxConstraintsDataSPtr   inputBounds()         const;
    BoxConstraintsDataSPtr   outputBounds()        const;
    BoxConstraintsDataSPtr   FitnessBounds()       const;
    void defineInputBounds(  BoxConstraintsDataSPtr box);
    void defineOutputBounds( BoxConstraintsDataSPtr box);
    void defineFitnessBounds(BoxConstraintsDataSPtr box);

    // Information about the node.
    TString     name();
    TString     description();
    
    

private:
    void initialise();
    void defineGlobalInputBounds();
    void defineGlobalOutputBounds();

    NormalisationBoundsType  m_inputBoundsType;
    NormalisationBoundsType  m_outputBoundsType;
    BoxConstraintsDataSPtr   m_inputBounds;
    BoxConstraintsDataSPtr   m_outputBounds;
    BoxConstraintsDataSPtr   m_fitnessBounds;
};

} // namespace Operators
} // namespace Tigon

#endif // DENORMALISATION_H
