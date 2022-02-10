/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QOPPROBFORMULATIONNODEFORM_H
#define QOPPROBFORMULATIONNODEFORM_H
#include <tigon/tigon_global.h>
#include <tigon/Representation/Properties/ElementProperties.h>

#include <QMainWindow>
#include <QList>
#include <QByteArray>
#include <QModelIndex>
#include <QColor>
#include <QFont>

namespace Ui {
class QOpProbFormulationNodeForm;
}

namespace Tigon {
namespace Representation {
class Problem;
class UncertaintyMapping;
}

namespace Operators {
class ProblemGenerator;
}
}

namespace QTigon {
class InputOutputCheckingForm;
class InputPropertiesDialog;
class OutputPropertiesDialog;
class InputPrivateData;
class OutputPrivateData;

class QOpProbFormulationNodeForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit QOpProbFormulationNodeForm(QWidget *parent = 0);
    ~QOpProbFormulationNodeForm();
    void setup(Tigon::Operators::ProblemGenerator* thisOp);
    void setInputDisabled(bool b = true);

signals:
    void opDataChanged();

public slots:
    void closeEvent(QCloseEvent *);

private slots:
    /*!
     * \brief loadSelectedFunction
     * \param The actual class name of the function is passed, e.g.
     *        Tigon::Representation::DTLZ1, not just DTLZ1. This name can be
     *        obtained by calling function->metaObject()->className();
     */
    void processSelectedFunction(const QString& className,
                                 const QString& funcType,
                                 const int &ninput,
                                 const int &noutput);
    void loadFunction(Tigon::Representation::IFunctionSPtr func);
    void inputOutputReceived();
    void setProblemChanged();

    /*!
     * \brief Slot that receives updated data from InputPropertiesDialog
     * \param InputPrivateData data
     */
    void updateInputData(const InputPrivateData& data);
    void updateOutputData(const OutputPrivateData& data);
    void clearContent();
    void clearHightlight();

    /// Actions
    void on_actionRestore_triggered();
    void on_loadButton_clicked();
    void on_removeButton_clicked();
    void on_varsListWidget_doubleClicked(const QModelIndex &index);
    void on_paramsListWidget_doubleClicked(const QModelIndex &index);
    void on_objsListWidget_doubleClicked(const QModelIndex &index);
    void on_consListWidget_doubleClicked(const QModelIndex &index);
    void on_unusedListWidget_doubleClicked(const QModelIndex &index);
    void on_savButton_clicked();
    void on_functionListWidget_clicked(const QModelIndex &index);

    /// Context menu
    void showVarContextMenu(const QPoint &point);
    void showParamContextMenu(const QPoint &point);
    void showVarPrptForm();
    void showParamPrptForm();
    void renameVar();
    void renameParam();

    // The generated code by the designed calls connectSlotsByName().
    // This function automatically connects the following slots to the
    // corresponding UI variables once a call to setupui() is placed.
    void on_actionSave_triggered();
    void on_actionLoad_triggered();

private:
    void createDockWidgets();
    void loadProblemToViews();

    Tigon::TVector<Tigon::Representation::UncertaintyMapping *> cloneDVecUncertaintyMapping() const;
    Tigon::TVector<Tigon::TVector<Tigon::Representation::UncertaintyMapping*> > cloneFuncOutUncertainties() const;

    bool checkFunction(const Tigon::TString& id);
    Tigon::ProblemDefinitionStatus
    configureProblem(Tigon::Representation::ProblemSPtr problem,
                     Tigon::TVector<Tigon::Representation::IFunctionSPtr> funcs,
                     Tigon::TVector<Tigon::TVector<int> > f2pmap,
                     Tigon::TVector<Tigon::TVector<int> > f2omap,
                     Tigon::TVector<Tigon::TVector<int> > f2cmap,
                     Tigon::TVector<Tigon::TVector<int> > f2umap = Tigon::TVector<Tigon::TVector<int>>()) const;
    void variableToParameter(const InputPrivateData& oldData,
                             const InputPrivateData& data);
    void parameterToVariable(const InputPrivateData& oldData,
                             const InputPrivateData& data);
    void replaceVariableData(const InputPrivateData& data);
    void replaceParameterData(const InputPrivateData& data);

    /// Append to view list
    QModelIndex appendToFuncsList(const QString& name, const QString& description);
    QModelIndex appendToVarsList(const QString& name, const QString& description);
    QModelIndex appendToParamsList(const QString& name, const QString& description);
    QModelIndex appendToObjsList(const QString& name, const QString& description);
    QModelIndex appendToConstrList(const QString& name, const QString& description);
    QModelIndex appendToUnusedList(const QString& name, const QString& description);

    bool inputNameExist(const QString& name);
    bool outputNameExist(const QString& name);

    /*!
     * \brief Check if the given input has already be loaded.
     * \param The element properties of the given input
     * \return -1 is there is no duplication; an int which greater than -1 otherwise
     */
    int checkInputDuplication(const Tigon::Representation::ElementProperties& prt);

    /*!
     * \brief Check if the given output has already be loaded.
     * \param The element properties of the given output
     * \return -1 is there is no duplication; an int which greater than -1 otherwise
     */
    int checkOutputDuplication(const Tigon::Representation::ElementProperties& prt);

    QColor m_highlightColor;
    QColor m_defaultColor;

    QByteArray                              m_dockProperties;
    Ui::QOpProbFormulationNodeForm*         ui;
    InputOutputCheckingForm*                m_inputoutputCheckingForm;
    ///\todo remove memebr variables m_inputPropertiesDialog and m_outputPropertiesDialog.
    /// Using loocal variables on stack and connect the singal finish() to the
    /// slot deleteLater() instead.
    InputPropertiesDialog*                  m_inputPropertiesDialog;
    OutputPropertiesDialog*                 m_outputPropertiesDialog;
    Tigon::Operators::ProblemGenerator*     m_pg;  // not responsible for deleteion
    Tigon::Representation::ProblemSPtr      m_prob;

    QStringList    m_varIDs;
    QStringList    m_paramIDs;
    QStringList    m_objIDs;
    QStringList    m_cnstrIDs;
    QStringList    m_unusedID;

    //! Container for private data
    QMap<QString, InputPrivateData>     m_inputData;
    QMap<QString, OutputPrivateData>    m_outputData;

    bool m_problemChanged;
};

}
#endif // QOPPROBFORMULATIONNODEFORM_H
