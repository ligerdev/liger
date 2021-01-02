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
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace tinyxml2 {
class XMLElement;
}

namespace Tigon {
class TObject;
class TigonOperatorFactory;
class TigonFunctionFactory;

namespace Representation {
class DistributionFactory;
class IPSet;
class IDistribution;
}
}

namespace Tigon {
class OptimizationLinearFlow;

/*!
 * \brief The TigonEngine class follows a singleton pattern, which
 * defines a unique instance that holds the tigon execution engine.
 */
class LIGER_TIGON_EXPORT TigonEngine
{
public:
    static TigonEngine* instance();
    ~TigonEngine();

    /// File I/O
    /*!
     * \brief Load Liger/Tigon workflow to the engine and generate
     * \param file Path to the workflow
     * \return true on success; otherwise, false
     */
    bool loadFlowFromFile(const TString &file);
    /*!
     * \brief Load and create mutiple workflow at the same time.
     * \param files, pathes to the workflow files
     * \return true on success; otherwise, false
     */
    bool loadMultipleFlowsFromFiles(const TStringList &files);

    ///\todo save workflow from tigon directly.
    /// This workflow cannot be loaded from Liger as it have no
    /// GUI element properties.
    void saveFlowToFile(const TString &file);
    void saveFlowsToFiles(const TStringList &files);

    /// Flow control
    void appendFlow(OptimizationLinearFlow* flow);
    void setCurrentIndex(int index);
    TVector<OptimizationLinearFlow*> workflows() const;
    OptimizationLinearFlow* currentFlow() const;
    void evaluateFlow(int index);
    void clearFlow();

    /// Creators
    Representation::IPSet* createOperator(const TString &name,
                                          Representation::IPSet* parent);
    Representation::IFunction* createFunction(const TString &name);
    Representation::IDistribution* createParametricDistribution(const DistributionType& type,
                                                                const TVector<double>& parameters);
private:
    TigonEngine();
    // Helper functions
    void assignPropertyToObject(Tigon::TObject *object, tinyxml2::XMLElement *xmlElm);
    void propertiesToXml(Tigon::TObject *object, tinyxml2::XMLElement *xmlElm);

    void loadTigonOperator(OptimizationLinearFlow* workflow,
                           tinyxml2::XMLElement *xmlElm);
    void loadTigonAlgorithm(OptimizationLinearFlow* workflow,
                            tinyxml2::XMLElement *xmlElm);
    void loadTigonProblem(OptimizationLinearFlow* workflow,
                          tinyxml2::XMLElement *xmlElm);
    void loadTigonMasterStart(OptimizationLinearFlow* workflow,
                              tinyxml2::XMLElement *xmlElm);

    // Private data
    TVector<OptimizationLinearFlow*> m_flows;
    int                              m_currentIndex;
    TigonOperatorFactory*            m_operatorFactory;
    TigonFunctionFactory*            m_functionFactory;
    Representation::DistributionFactory* m_distFactory;
};

} // namespace Tigon
