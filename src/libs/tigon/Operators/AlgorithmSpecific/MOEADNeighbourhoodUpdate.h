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
#ifndef MOEADNEIGHBOURHOODUPDATE_H
#define MOEADNEIGHBOURHOODUPDATE_H
#include <tigon/Operators/Directions/IDirection.h>

namespace Tigon {
namespace Operators {


class LIGER_TIGON_EXPORT MOEADNeighbourhoodUpdate : public IDirection
{

    //Q_PROPERTY(Tigon::ScalarisationType ScalarisingFunction
    //           READ                     TP_scalarisingFunction
    //           WRITE                    TP_defineScalarisingFunction)

    HANDLE_READ_PROPERTIES_BEGIN(IDirection)
    READ(MaxNumberReplacements, TP_maxNumberReplacements)
    READ(IsGeneralisedDecomposition, TP_isGeneralisedDecomposition)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IDirection)
    WRITE(MaxNumberReplacements, int, TP_defineMaxNumberReplacements)
    WRITE(IsGeneralisedDecomposition, bool, TP_defineIsGeneralisedDecomposition)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::MOEADNeighbourhoodUpdate)

public:
    MOEADNeighbourhoodUpdate();
    MOEADNeighbourhoodUpdate(Tigon::Representation::IPSet* ipset);
    virtual ~MOEADNeighbourhoodUpdate();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    void  TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType TP_scalarisingFunction() const;

    void TP_defineMaxNumberReplacements(int r=2);
    int TP_maxNumberReplacements() const;

    void TP_defineIsGeneralisedDecomposition(bool b=false);
    bool TP_isGeneralisedDecomposition() const;

    double pNorm() const;

private:
    void initialise();

    TString m_name;
    TString m_description;
    int m_maxNumberReplacements;

    Tigon::ScalarisationType m_scalarisingFunc;
    double                       m_pNorm;

    bool   m_isGeneralisedDecomposition;
};

} // namespace Operators
} // namespace Tigon


#endif // MOEADNEIGHBOURHOODUPDATE_H
