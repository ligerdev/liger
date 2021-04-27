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
#ifndef IOPERATOR_H
#define IOPERATOR_H
#include <tigon/tigon_global.h>
#include <tigon/Representation/PSets/IPSetDecorator.h>

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

/*!
 * \brief An IOperator makes operations on IMappings, stored in ISets. It uses input
 * sets, created by other IOperators, and can create and modify multiple ISets
 * to store its outputs.
 *
 * By convention, a set is considered as an input if it for read only, and as an
 * output set if it is modified. The IOperator has two iterators, one to
 * traverse over its output sets, and another to traverse over its input sets.
 * Output sets can also be appended, removed and cleared.
 *
 * All sets are stored in IPSet, and therefore, all other operators can access
 * them and process them. To support the flow of information between IOperators,
 * the sets are tagged. Every IOperator contains a list of input and output tags
 * which later can be searched for by other IOperators.
 *
 * Prior to the execution of the IOperator, the input and output sets are
 * updated according to the lists of input and output tags. Any change of tags
 * needs to be made before evaluate() is called. Every operator is initialised
 * with default tags according to its functionality. If the operator needs to be
 * specialised, the tags can be modified.
 */

class LIGER_TIGON_EXPORT IOperator : public IPSetDecorator
{
    HANDLE_READ_PROPERTIES_BEGIN(IPSetDecorator)
    READ(OutputTags, TP_outputTags)
    READ(AdditionalOutputTags, TP_additionalOutputTags)
    READ(InputTags, TP_inputTags)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IPSetDecorator)
    WRITE(OutputTags, TString, TP_defineOutputTags)
    WRITE(AdditionalOutputTags, TString, TP_defineAdditionalOutputTags)
    WRITE(InputTags, TString, TP_defineInputTags)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IOperator)

public:
    IOperator();
    IOperator(Tigon::Representation::IPSet* ipset);
    virtual ~IOperator();

    virtual void evaluateNode() = 0;

    /// Properties
    TStringList outputTags()         const;
    void defineOutputTags(const TStringList &tags);

    TString TP_outputTags()         const;
    void TP_defineOutputTags(const TString & tags);

    TStringList additionalOutputTags()          const;
    void defineAdditionalOutputTags(const TStringList &tags);

    TString TP_additionalOutputTags()         const;
    void TP_defineAdditionalOutputTags(const TString & tags);

    TStringList inputTags()          const;
    void defineInputTags(const TStringList &tags);

    TString TP_inputTags()         const;
    void TP_defineInputTags(const TString & tags);

    TStringList  supportedElementTypes()          const;
    void defineSupportedElementTypes(const TStringList &types);

    bool shufleInputSet() const;
    void defineShuffleInputSet(bool p);

    bool shufleOutputSet() const;
    void defineShuffleOutputSet(bool p);

    /// Input and Output Sets Tags
    void addOutputTag(const TString &tag);
    void removeOutputTag(const TString &tag);
    void clearOutputTags();
    void addAdditionalOutputTag(const TString  &tag);
    void removeAdditionalOutputTag(const TString &tag);
    void clearAdditionalOutputTags();
    void addInputTag(const TString &tag);
    void removeInputTag(const TString &tag);
    void clearInputTags();

    /// Output Sets
    // Read
    /*!
     * \brief Returns null if empty or iterator was resetted
     */
    ISet*   currentOutputSet()         const;
    /*!
     * \brief Returns the requested output set. The returned ISet* becomes the
     * current output set. Returns null if out of bounds
     */
    ISet*   outputSet(int             idx);
    TVector<ISet*> outputSets()        const;
    // Modify
    // The returned ISet* becomes the current output set
    ISet* appendOutputSet();                     // Creates an empty set
    ISet* appendOutputSet(ISet* existingSet);  // Responsibility over existingSet passes to this operator
    bool  removeOutputSet(int   idx);          // Returns true if the set was removed
    void  clearOutputSets();
    // Traverse
    ISet* nextOutputSet();           // Returns the next set. If it is the last, returns the last set.
    bool  hasNextOutputSet() const;
    void  resetOutputSetIterator();  // Points before the first set (-1)
    ISet* peepNextOutputSet() const; // If the set is the last, returns null.

    /// Input Sets
    /* Read
       The returned ISet* becomes the current input set. */
    ISet* currentInputSet()         const; // Returns null if empty or iterator was resetted
    ISet* inputSet(int          idx); // Returns null if out of bounds
    TVector<ISet*> inputSets()      const;
    // Traverse
    ISet* nextInputSet();
    bool  hasNextInputSet() const;
    void  resetInputSetIterator();
    ISet* peepNextInputSet() const; // If the set is the last, returns null.

    /// General functions
    bool isSupportedElementType(Tigon::ElementType type) const;
    bool isSupportedElementType(IElementSPtr e)          const;

private:
    void initialise();

    TVector<ISet*> m_outputSets;
    TVector<ISet*> m_inputSets;

    bool m_setsCreator;

    TStringList m_outputTags;
    TStringList m_addOutputTags;
    TStringList m_inputTags;

    int     m_outputSetIterator;
    int     m_inputSetIterator;
    ISet*   m_currentOutputSet;
    ISet*   m_currentInputSet;

    bool m_shuffleInputSet;
    bool m_shuffleOutputSet;

    TStringList m_supportedElementTypes;

protected:
    virtual void updateSets();

    IMappingSPtr createMapping();
    IMappingSPtr cloneMapping(const IMappingSPtr other);

    TMap<TString, Tigon::ElementType> m_elementTypeMap;
};

} // namespace Operators
} // namespace Tigon

#endif // IOPERATOR_H
