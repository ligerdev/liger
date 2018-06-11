#include "{{ ProblemName }}.h"

ProblemProperties problemProperties()
{
    /*=============================================
     * Define your problem in the following
     *      name        : Problem name - required
     *      description : Problem description - required
     *      ptype       : Problem Type - if known
     *=============================================
     */
    QString  name("{{ ProblemName }}");
    QString description("{{ ProblemDescription }}");
    //ProblemType ptype = ProblemType::NonConvex;

    ProblemPropertiesFactory* probFct = ProblemPropertiesFactory::instance();
    ProblemProperties problem = probFct->createProblemProperties(name,description);
    //ProblemProperties* problem = probFct->createProblemProperties(name,description,ptype);
    return problem;

}

QVector<ElementProperties> decisionVecProperties()
{
    /*=============================================
     * Define your decision variables in the following
     *      nVar            : Number of decision variables,   required
     *      varNames        : Decision variable names,        required
     *      varDescriptions : Decision variable descriptions, required
     *      varVecTypes     : Decision variable vector types, required
     *      varUnits        : Decision variable units,        required
     *=============================================
     */
    int nVar;
    QStringList varNames;
    QStringList varDescriptions;
    QVector<ElementType> decisionVecTypes;
    QStringList varUnits;

    /*=============================================
     * Do NOT touch the above lines of codes
     * Start implementing.
     *=============================================
     */

    nVar = {{ nVar }};
    for(int i = 0; i < nVar; i++) {
        varNames.append("SharedLib_Var_" + QString::number(i));
        varDescriptions.append("SharedLib_VarDesc_" + QString::number(i));
        decisionVecTypes.append(RealType);
        varUnits.append("None");
    }

    /*=============================================
     * End implementing.
     * Do Not touch the following lines of codes
     * =============================================
     */
    QVector<ElementProperties> decisionVecProperties =
            libCreateNewElementProperties(nVar,varNames,varDescriptions,decisionVecTypes,varUnits);

    return decisionVecProperties;
}

QVector<ElementProperties> objectiveVecProperties()
{
    /*=============================================
     * Define your objectives in the following
     *      nObj              : Number of objectives,   required
     *      objNames          : Objective names,        required
     *      objDescriptions   : Objective descriptions, required
     *      objectiveVecTypes : Objective vector types, required
     *      optimTypes        : Optimizations types,    required
     *      objUnits          : Objectives units,       required
     *=============================================
     */
    int nObj;
    QStringList objNames;
    QStringList objDescriptions;
    QVector<ElementType> objectiveVecTypes;
    QStringList objUnits;
    QVector<OptimizationType> optimTypes;

    /*=============================================
     * Do NOT touch the above lines of codes
     * Start implementing
     *=============================================
     */

    nObj = {{ nObj}};  // Number of objectives
    for(int i = 0; i < nObj; i++) {
        objNames.append("SharedLib_Obj_" + QString::number(i));           // Objective names
        objDescriptions.append("SharedLib_ObjDes_" + QString::number(i)); // Objective descriptions
        objectiveVecTypes.append(RealType);                               // Objective vector types
        optimTypes.append(OptimizationType::Minimization);                // Optimization types
        objUnits.append("None");                                          // Unites
    }

    /*=============================================
     * End implementing.
     * Do Not touch the following lines of codes
     * =============================================
     */

    QVector<ElementProperties> objectiveVecProperties;
    objectiveVecProperties =
            libCreateNewElementProperties(nObj,
                                          objNames,
                                          objDescriptions,
                                          objectiveVecTypes,
                                          objUnits);
    for(int i = 0; i<objectiveVecProperties.size(); i++) {
        objectiveVecProperties[i].setOptimizationType(optimTypes.at(i));
    }
    return objectiveVecProperties;
}

QVector<ElementProperties> parameterVecProperties()
{
    /*=============================================
     * Define your parameters in the following
     *      nParam            : Number of parameters,   required
     *      paramNames        : Parameter names,        required
     *      paramDescriptions : Parameter descriptions, required
     *      paramVecTypes     : Parameter vector types, required
     *      paramUnits        : Parameter units,        required
     *=============================================
     */
    int nParam;
    QStringList paramNames;
    QStringList paramDescriptions;
    QVector<ElementType> paramVecTypes;
    QStringList paramUnits;

    /*=============================================
     * Do NOT touch the above lines of codes
     * Start implementing
     *=============================================
     */

    nParam = {{ nParam }};
    for(int i = 0; i < nParam; i++) {
        paramNames.append("SharedLib_Parameter_" + QString::number(i));
        paramDescriptions.append("SharedLib_ParameterDes_" + QString::number(i));
        paramVecTypes.append(RealType);
        paramUnits.append("s");
    }

    QVector<ElementProperties> parameterVecProperties;
    parameterVecProperties =
            libCreateNewElementProperties(nParam,
                                          paramNames,
                                          paramDescriptions,
                                          paramVecTypes,
                                          paramUnits);
    return parameterVecProperties;
}

BoxConstraintsData* boxConstraints()
{
    BoxConstraintsData* box = new BoxConstraintsData;
    QVector<qreal> lbounds;
    QVector<qreal> ubounds;

    /*
     * Define lower and upper bounds here
     */
    int nVar = {{ nVar }};
    for(int i = 0; i < nVar; i++) {
        lbounds.append(0);
        ubounds.append(1);
    }

    box->setLowerBounds(lbounds);
    box->setUpperBounds(ubounds);

    return box;
}

QVector<QVector<bool>> variables2ObjectivesMap()
{
    QVector<QVector<bool>> var2ObjMap;

    // Provide variables and objectives connectivity matrix here
    // if known.
    QVector<ElementProperties> dvecPrpts =
            decisionVecProperties();
    QVector<ElementProperties> objPrpts =
            objectiveVecProperties();
    int nVar = dvecPrpts.size();
    int nObj = objPrpts.size();

    for(int i = 0; i < nVar; i++) {
        QVector<bool> tmp;
        for(int j = 0; j < nObj; j++) {
            tmp.append(true);
        }
        var2ObjMap.append(tmp);
    }

    return var2ObjMap;
}

QVector<QVector<bool>> parameters2ObjectivesMap()
{
    QVector<QVector<bool>> param2ObjMap;

    // Provide parameters and objectives connectivity matrix here
    // if known.
    QVector<ElementProperties> paramVecPrpts =
            parameterVecProperties();
    QVector<ElementProperties> objPrpts =
            objectiveVecProperties();
    int nParam = paramVecPrpts.size();
    int nObj = objPrpts.size();

    for(int i = 0; i < nParam; i++) {
        QVector<bool> tmp;
        for(int j = 0; j < nObj; j++) {
            if(j >= i) {
                tmp.append(true);
            } else {
                tmp.append(false);
            }
        }
        param2ObjMap.append(tmp);
    }

    return param2ObjMap;
}

int evaluateObjs(QVector<IMappingSPtr> cmaps)
{
    /* -------------------------------------------------
     * Implementation for evaluating objective functions
     * -------------------------------------------------
     */

    for(int i = 0; i < cmaps.size(); i++) {
        IMappingSPtr cmap = cmaps.at(i);
        int nVar = cmap->inputVec().size();
        int nObj = cmap->outputVec().size();

        double* x  = new double[nVar];
        double* y  = new double[nObj];

        QVector<IElementSPtr> dvec = cmap->inputVec();
        for(int i = 0; i < nVar; i++) {
            x[i] = dvec.at(i)->value().toReal();
        }

        for (int i = 0; i < nObj; i++) {
            double sum = 0;
            for(int j = 0; j < nVar; j++) {
                sum = sum + x[j];
            }
            y[i] = sum / ((double) i+1);
        }

        for(int i = 0; i < nObj; i++) {
            cmap->setOutputVec(i, QVariant(y[i]));
        }
    }
    return 0;
}


/*=============================================
 * Internal functions - do NOT change
 *=============================================
 */
QVector<ElementProperties> libCreateNewElementProperties(int nElem,
                                                          QStringList elemNames,
                                                          QStringList elemDescription,
                                                          QVector<ElementType> elemVecType,
                                                          QStringList elemUnits)
{
    QVector<ElementProperties> elemVecPrpts;
    ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();
    for(int i = 0; i < nElem; i++) {
        ElementProperties newEle = fct->createElementProperties(i,
                                                                elemNames.at(i),
                                                                elemDescription.at(i),
                                                                elemVecType.at(i));
        newEle.setUnits(elemUnits.at(i));
        elemVecPrpts.append(newEle);
    }
    return elemVecPrpts;
}
