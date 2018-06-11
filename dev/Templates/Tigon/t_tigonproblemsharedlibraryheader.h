#ifndef {% filter upper %}{{ ProblemName }}{% endfilter %}PROBLEM_H
#define {% filter upper %}{{ ProblemName }}{% endfilter %}PROBLEM_H

/*=============================================
 * Do NOT change the following lines of codes
 *=============================================*/
#include <QtCore/qglobal.h>
#if !defined(TIGON_FUNCTION_EXPORT)
#  define TIGON_FUNCTION_EXPORT extern "C" Q_DECL_EXPORT
#endif

// Qt includes
#include <QString>
#include <QVector>
#include <{{ path_to_tigon }}/Tigon.h>
#include <{{ path_to_tigon }}/tigonconstants.h>

using namespace Tigon;
using namespace Tigon::Representation;

// Exported functions
TIGON_FUNCTION_EXPORT ProblemProperties          problemProperties();
TIGON_FUNCTION_EXPORT QVector<ElementProperties> decisionVecProperties();
TIGON_FUNCTION_EXPORT QVector<ElementProperties> objectiveVecProperties();
TIGON_FUNCTION_EXPORT QVector<ElementProperties> parameterVecProperties();
TIGON_FUNCTION_EXPORT BoxConstraintsData*        boxConstraints();

TIGON_FUNCTION_EXPORT QVector<QVector<bool> >    variables2ObjectivesMap();
TIGON_FUNCTION_EXPORT QVector<QVector<bool> >    parameters2ObjectivesMap();

//TODO: constraints
TIGON_FUNCTION_EXPORT LinearConstraintsData*     linearEqConstraints();
TIGON_FUNCTION_EXPORT LinearConstraintsData*     linearIneqConstraints();
TIGON_FUNCTION_EXPORT NonLinearConstraintsData*  nonLinearEqConstraints();
TIGON_FUNCTION_EXPORT NonLinearConstraintsData*  nonLinearIneqConstraints();

TIGON_FUNCTION_EXPORT int                        evaluateObjs(QVector<IMappingSPtr> cmaps);

// Nonexported function
QVector<ElementProperties> libCreateNewElementProperties(int nElem,
                                                          QStringList elemNames,
                                                          QStringList elemDescription,
                                                          QVector<ElementType> elemVecType,
                                                          QStringList elemUnits);
/*=============================================
 * Do NOT change the above lines of codes
 *=============================================*/

#endif // {% filter upper %}{{ ProblemName }}{% endfilter %}PROBLEM_H