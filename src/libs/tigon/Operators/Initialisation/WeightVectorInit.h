#ifndef WEIGHTVECTORINIT_H
#define WEIGHTVECTORINIT_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT WeightVectorInit : public IInitialisation
{
    //Q_PROPERTY(Tigon::RegularisationApproach RegularisationMethod READ TP_regularisationMethod WRITE TP_defineRegularisationMethod)

    HANDLE_READ_PROPERTIES_BEGIN(IInitialisation)
    READ(NumberPointsPerDimension, TP_numberPointsPerDimension)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IInitialisation)
    WRITE(NumberPointsPerDimension, int, TP_defineNumberPointsPerDimension)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::WeightVectorInit)

public:
    WeightVectorInit();
    WeightVectorInit(Representation::IPSet* ipset);
    ~WeightVectorInit();

    void initialise();

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    // Number of points per dimension (H)
    void TP_defineNumberPointsPerDimension(int H);
    int TP_numberPointsPerDimension()   const;

    void TP_defineRegularisationMethod(Tigon::RegularisationApproach r);
    Tigon::RegularisationApproach TP_regularisationMethod() const;

    void TP_defineDistanceMeasure(DistanceMeasure distance);
    DistanceMeasure TP_distanceMeasure() const;

private:
    bool isNewGoalVec();
    void updateGoalVec();

    bool                         m_determineWeightVectors;
    int                        m_numberPointsPerDimension;
    int                             m_fixedPopulationSize;
    Tigon::RegularisationApproach  m_regularisationMethod;
    TVector<TVector<double>>                     m_refSet;
    TVector<double>                         m_normGoalVec;
    DistanceMeasure                     m_distanceMeasure;
};

} // namespace Operators
} // namespace Tigon

#endif // WEIGHTVECTORINIT_H
