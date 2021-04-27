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
#ifndef INPUTOUTPUTPRIVATEDATA_H
#define INPUTOUTPUTPRIVATEDATA_H
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <QModelIndex>
#include <QString>
#include <QList>
#include <QDebug>

namespace QTigon {
/*!
 * \brief The InputPrivateData class holds a local copy of the properties of
 * the input varaibles/parameters. Formalise it when its data structure is stable.
 *
 * \note When introduce new data into this class, remember to add it to the copy
 * constructor and assignment operator.
 */
class InputPrivateData
{
public:
    InputPrivateData()
        : m_uncertainMapping(Q_NULLPTR)
        , m_isExternal(false)
        , m_groupID(-1)
    {

    }

    InputPrivateData(const InputPrivateData& data)
    {
        if(data.m_uncertainMapping != Q_NULLPTR) {
            this->m_uncertainMapping = data.m_uncertainMapping->clone();
        } else {
            this->m_uncertainMapping = Q_NULLPTR;
        }
        this->prts   = data.prts;
        this->value  = data.value;
        this->varLbound = data.varLbound;
        this->varUbound = data.varUbound;
        this->scope  = data.scope;
        this->viewIndex   = data.viewIndex;
        this->funcIndices = data.funcIndices;
        this->m_isExternal = data.m_isExternal;
        this->m_groupID = data.m_groupID;
        this->m_groupDataPath = data.m_groupDataPath;
    }

    InputPrivateData& operator=(const InputPrivateData& other)
    {
        if(this != &other) {
            delete m_uncertainMapping;
            if(other.m_uncertainMapping != Q_NULLPTR) {
                m_uncertainMapping = other.m_uncertainMapping->clone();
            } else {
                m_uncertainMapping = Q_NULLPTR;
            }
            prts = other.prts;
            value = other.value;
            varLbound = other.varLbound;
            varUbound = other.varUbound;
            scope     = other.scope;
            viewIndex   = other.viewIndex;
            funcIndices = other.funcIndices;
            m_isExternal = other.m_isExternal;
            m_groupID = other.m_groupID;
            m_groupDataPath = other.m_groupDataPath;
        }
        return *this;
    }

    /*!
     * \brief clone the uncertainty mapping when passing to dVecUncertainties
     */
    Tigon::Representation::UncertaintyMapping* cloneUncertainMapping() const
    {
        if(m_uncertainMapping != Q_NULLPTR) {
            return m_uncertainMapping->clone();
        } else {
            return Q_NULLPTR;
        }
    }

    Tigon::Representation::UncertaintyMapping* uncertainMapping() const
    {
       return m_uncertainMapping;
    }

    void setUncertainMapping(Tigon::Representation::UncertaintyMapping* mapping)
    {
        if(m_uncertainMapping) {
            delete m_uncertainMapping;
        }
        m_uncertainMapping = mapping;
    }

    QString name() const
    {
        return QString::fromStdString(prts.name());
    }

    QString description() const
    {
        return QString::fromStdString(prts.description());
    }

    QString ID() const
    {
        return QString::fromStdString(prts.ID());
    }

    Tigon::Representation::IElement               value;
    Tigon::Representation::IElement               varLbound;
    Tigon::Representation::IElement               varUbound;
    QString scope;
    QModelIndex viewIndex;
    QList<QModelIndex> funcIndices;

    bool isExternal() const {return m_isExternal;}
    void setExternal(bool isExternal) {m_isExternal = isExternal;}

    Tigon::Representation::ElementProperties      prts;

    int groupID() const {return m_groupID;}
    void setGroupID(int groupID) {
        if(isExternal()) {
            m_groupID = groupID;
        }
    }

    QString groupDataPath() const {return m_groupDataPath;}
    void setGroupDataPath(const QString &path) {
        if(isExternal()) {
            m_groupDataPath = path;
        }
    }

private:
    Tigon::Representation::UncertaintyMapping*    m_uncertainMapping;
    bool m_isExternal;
    int  m_groupID;
    QString m_groupDataPath;
};

/*!
 * \brief The OutputPrivateData class holds a local copy of the properties of the
 * output objectives/constraints
 */
class OutputPrivateData
{
public:
    OutputPrivateData() : setGoal(false), goal(-Tigon::Highest), priority(1),
                          threshold(Tigon::Highest) {
                                m_uncertainMapping = Q_NULLPTR; }
    OutputPrivateData(const OutputPrivateData& data)
    {
        if(data.m_uncertainMapping != Q_NULLPTR) {
            this->m_uncertainMapping = data.m_uncertainMapping->clone();
        } else {
            this->m_uncertainMapping = Q_NULLPTR;
        }
        this->prts     = data.prts;
        this->scopes   = data.scopes;
        this->setGoal  = data.setGoal;
        this->goal     = data.goal;
        this->priority = data.priority;
        this->threshold       = data.threshold;
        this->objViewIndex    = data.objViewIndex;
        this->cnstrViewIndex  = data.cnstrViewIndex;
        this->unusedViewIndex = data.unusedViewIndex;
        this->funcIndices = data.funcIndices;
    }

    OutputPrivateData& operator=(const OutputPrivateData& other)
    {
        if(this != &other) {
            delete m_uncertainMapping;
            if(other.m_uncertainMapping != Q_NULLPTR) {
                m_uncertainMapping = other.m_uncertainMapping->clone();
            } else {
                m_uncertainMapping = Q_NULLPTR;
            }
            prts     = other.prts;
            scopes   = other.scopes;
            setGoal  = other.setGoal;
            goal     = other.goal;
            priority = other.priority;
            threshold = other.threshold;
            objViewIndex    = other.objViewIndex;
            cnstrViewIndex  = other.cnstrViewIndex;
            unusedViewIndex = other.unusedViewIndex;
            funcIndices = other.funcIndices;
        }
        return *this;
    }

    Tigon::Representation::UncertaintyMapping* cloneUncertainMapping() const
    {
        if(m_uncertainMapping == Q_NULLPTR) {
            return Q_NULLPTR;
        } else {
            return m_uncertainMapping->clone();
        }
    }

    Tigon::Representation::UncertaintyMapping* uncertainMapping() const
    {
        return m_uncertainMapping;
    }

    void setUncertainMapping(Tigon::Representation::UncertaintyMapping *mapping)
    {
        if(m_uncertainMapping) {
            delete m_uncertainMapping;
        }
        m_uncertainMapping = mapping;
    }

    QString name() const
    {
        return QString::fromStdString(prts.name());
    }

    QString description() const
    {
        return QString::fromStdString(prts.description());
    }

    QString ID() const
    {
        return QString::fromStdString(prts.ID());
    }

    Tigon::Representation::ElementProperties prts;
    QStringList scopes;
    bool setGoal;
    Tigon::Representation::IElement goal;
    int priority;
    Tigon::Representation::IElement threshold;
    QModelIndex objViewIndex;
    QModelIndex cnstrViewIndex;
    QModelIndex unusedViewIndex;
    QList<QModelIndex> funcIndices;

private:
    Tigon::Representation::UncertaintyMapping*    m_uncertainMapping;
};

}
#endif // INPUTOUTPUTPRIVATEDATA_H
