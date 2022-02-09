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
#include <qtigon/dialogs/outputpropertiesdialog.h>
#include "ui_outputpropertiesdialog.h"
#include <qtigon/qtigonconstants.h>

#include <tigon/Factories/DistributionFactory.h>
#include <QDoubleValidator>
#include <QMessageBox>

using namespace QTigon;
using namespace Tigon;
using namespace Tigon::Representation;

OutputPropertiesDialog::OutputPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputPropertiesDialog)
{
    ui->setupUi(this);
    initialise();
    setupSignals();
}

void OutputPropertiesDialog::setData(const OutputPrivateData &data)
{
    /// [0] Set Data
    m_d = data; // assignment operator called. Uncertainty mapping is cloned

    /// [1] Reset ui elements
    ui->nameLineEdit->setText(QString::fromStdString(data.prts.name()));
    ui->descriptionPlainTextEdit->clear();
    ui->descriptionPlainTextEdit->setPlainText(QString::fromStdString(data.prts.description()));
    ui->funcsLineEdit->clear();
    ui->distributionComboBox->setCurrentIndex(0);
    m_uncertainMappingModelData.clear();

    /// [2] Append function list
    QString fns;
    for(int i=0; i<data.funcIndices.size(); i++) {
        fns.append(data.funcIndices[i].data().toString() + ";");
    }
    ui->funcsLineEdit->setText(fns);

    /// [3] Set Type Combobox
    switch(data.prts.type()) {
    case ElementType::RealType:
        ui->typeLineEdit->setText(QString::fromStdString(Tigon::ETReal));
        break;
    case ElementType::IntegerType:
        ui->typeLineEdit->setText(QString::fromStdString(Tigon::ETInteger));
        break;
    case ElementType::OrdinalType:
        ui->typeLineEdit->setText(QString::fromStdString(Tigon::ETOrdinal));
        break;
    case ElementType::NominalType:
        ui->typeLineEdit->setText(QString::fromStdString(Tigon::ETNominal));
        break;
    default:
        break;
    }

    /// [4] Define Optimization Type, goal, priority and threshold
    OptimizationType optType = data.prts.optimizationType();
    ui->optTypeComboBox->setCurrentIndex(optType);
    switch(optType) {
    case Minimization:
        ui->goalValueEdit->setText(QString::number(data.goal.value()));
        ui->thresholdLineEdit->setText(QString::number(data.threshold.value()));
        break;
    case Maximization:
        ui->goalValueEdit->setText(QString::number(-data.goal.value()));
        ui->thresholdLineEdit->setText(QString::number(-data.threshold.value()));
        break;
    default:
        break;
    }
    ui->setGoalcheckBox->setChecked(data.setGoal);
    ui->prioritySpinBox->setValue(data.priority);

    /// [5] Define scopes
    if(data.scopes.contains(Constants::OBJECTIVE)) {
        ui->objectiveCheckBox->setChecked(true);
    } else {
        ui->objectiveCheckBox->setChecked(false);
    }

    if(data.scopes.contains(Constants::CONSTRAINT)) {
        ui->constraintCheckBox->setChecked(true);
    } else {
        ui->constraintCheckBox->setChecked(false);
    }

    /// [6] Define uncertainty mappings
    if(data.uncertainMapping() == Q_NULLPTR) {
        // Deterministic
        ui->deterministicRadioButton->setChecked(true);
    } else {
        // Uncertain
        ui->uncertainRadioButton->setChecked(true);

        QStringList keys = m_parametricDistMap.keys();
        DistributionType type = data.uncertainMapping()->distributionType();
        for(int i=0; i<m_parametricDistMap.size(); i++) {
            QString typestr = keys[i];
            if(m_parametricDistMap[typestr] == type) {
                ui->distributionComboBox->setCurrentText(typestr);
                // This will triger a function to populate the distribution
                break;
            }
        }
    }
}

void OutputPropertiesDialog::setParametricDistMap(const QMap<QString, DistributionType> &parametricDistMap)
{
    m_parametricDistMap = parametricDistMap;
}

void OutputPropertiesDialog::setReadOnly(bool readOnly)
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach(QWidget* widget, widgets) {
        widget->setDisabled(readOnly);
    }
}

OutputPropertiesDialog::~OutputPropertiesDialog()
{
    delete ui;
}

void OutputPropertiesDialog::setObj(const bool &check)
{
    int index = m_d.scopes.indexOf(Constants::OBJECTIVE);
    if(check) {
        if(index == -1) {
            m_d.scopes.append(Constants::OBJECTIVE);
        }
    } else {
        if(index != -1) {
            m_d.scopes.removeAt(index);
        }
    }
}

void OutputPropertiesDialog::setCnstr(const bool &check)
{
    int index = m_d.scopes.indexOf(Constants::CONSTRAINT);
    if(check) {
        if(index == -1) {
            m_d.scopes.append(Constants::CONSTRAINT);
        }
    } else {
        if(index != -1) {
            m_d.scopes.removeAt(index);
        }
    }
}

void OutputPropertiesDialog::initialise()
{
    /// [0] Set read-only
    ui->descriptionPlainTextEdit->setReadOnly(true);
    ui->nameLineEdit->setReadOnly(true);
    ui->funcsLineEdit->setReadOnly(true);
    ui->typeLineEdit->setReadOnly(true);

    /// [1] set default visibility
    ui->goalFrame->setVisible(false);
    ui->thresholdFrame->setVisible(false);
    ui->goalSetFrame->setVisible(false);
    ui->uncertainGroupBox->setVisible(false);

    /// [2] Uncertainty Parameter view
    ui->uncertainParamTableView->setModel(&m_uncertainMappingModelData);

    ui->uncertainParamTableView->horizontalHeader()->setStretchLastSection(true);
    ui->uncertainParamTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    /// [3] Default distribution maps
    if(m_parametricDistMap.isEmpty()) {
        m_parametricDistMap[DTUniformDistType] = UniformDistType;
        m_parametricDistMap[DTNormalDistType]  = NormalDistType;
        m_parametricDistMap[DTLinearDistType]  = LinearDistType;
        m_parametricDistMap[DTPeakDistType]    = PeakDistType;
    }
    QStringList keys;
    keys.append("");
    keys.append(m_parametricDistMap.keys());
    ui->distributionComboBox->insertItems(0, keys);

    /// LineEdit validator
    ui->goalValueEdit->setValidator(new QDoubleValidator);
    ui->thresholdLineEdit->setValidator(new QDoubleValidator);
}

void OutputPropertiesDialog::setupSignals()
{
    connect(ui->objectiveCheckBox, SIGNAL(toggled(bool)), ui->goalFrame, SLOT(setVisible(bool)));
    connect(ui->constraintCheckBox, SIGNAL(toggled(bool)), ui->thresholdFrame, SLOT(setVisible(bool)));
    connect(ui->setGoalcheckBox, SIGNAL(toggled(bool)), ui->goalSetFrame, SLOT(setVisible(bool)));
    connect(ui->uncertainRadioButton, SIGNAL(toggled(bool)), ui->uncertainGroupBox, SLOT(setVisible(bool)));
}

void OutputPropertiesDialog::on_objectiveCheckBox_toggled(bool checked)
{
    if(checked) {
        ui->uncertainFrame->show();
        if(ui->uncertainRadioButton->isChecked()) {
            ui->uncertainGroupBox->show();
        }
        return;
    }
    if(!checked && !ui->constraintCheckBox->isChecked()) {
        ui->uncertainFrame->hide();
        ui->uncertainGroupBox->hide();
    }
}

void QTigon::OutputPropertiesDialog::on_constraintCheckBox_toggled(bool checked)
{
    if(checked) {
        ui->uncertainFrame->show();
        if(ui->uncertainRadioButton->isChecked()) {
            ui->uncertainGroupBox->show();
        }
        return;
    }
    if(!checked && !ui->objectiveCheckBox->isChecked()) {
        ui->uncertainFrame->hide();
        ui->uncertainGroupBox->hide();
    }
}

void OutputPropertiesDialog::on_buttonBox_accepted()
{
    /// [] Define scopes
    setObj(ui->objectiveCheckBox->isChecked());
    setCnstr(ui->constraintCheckBox->isChecked());

    /// [] Uncertainties
    if(ui->uncertainRadioButton->isChecked()) {
        QString uncertaintyTypeStr = ui->distributionComboBox->currentText();
        if(!uncertaintyTypeStr.isEmpty()) {
            Tigon::DistributionType type = m_parametricDistMap[uncertaintyTypeStr];
            if(m_d.uncertainMapping() == Q_NULLPTR) {
                qDebug() << "Create empty uncertainty mapping";
                m_d.setUncertainMapping(new UncertaintyMapping);
            }
            // Define type if the distribution type has changed
            if(m_d.uncertainMapping()->distributionType() != type) {
                qDebug() << "Change mapping to type " << type;
                m_d.uncertainMapping()->defineDistributionType(type);
            }

            int nrows = m_uncertainMappingModelData.rowCount();
            int ncols = m_uncertainMappingModelData.columnCount();

            TVector<qreal> linearTerms(ncols-1), constTerms(ncols-1);
            for(int i=0; i<nrows; i++) {
                for(int j=1; j<ncols; j++) {
                    QModelIndex index = m_uncertainMappingModelData.index(i,j);
                    if(i==0) {
                        constTerms[j-1] = m_uncertainMappingModelData.data(index).toReal();
                    }
                    if(i==1) {
                        linearTerms[j-1] = m_uncertainMappingModelData.data(index).toReal();
                    }
                }
            }

            m_d.uncertainMapping()->defineConstTerms(constTerms);
            m_d.uncertainMapping()->defineLinearTerms(linearTerms);
        }
    } else {
        // Deterministic
        if(m_d.uncertainMapping() != Q_NULLPTR) {
            m_d.setUncertainMapping(Q_NULLPTR);
        }
    }

    /// [] Define Optimization Type
    switch(ui->optTypeComboBox->currentIndex()) {
    case 0:
        // min
        m_d.prts.setOptimizationType(OptimizationType::Minimization);
        break;
    case 1:
        // max
        m_d.prts.setOptimizationType(OptimizationType::Maximization);
        break;
    default:
        // min
        m_d.prts.setOptimizationType(OptimizationType::Minimization);
        break;
    }
    qDebug() << "OptType  : "<< m_d.prts.optimizationType();

    /// [] Define goal, priority and threshold
    switch(m_d.prts.optimizationType()) {
    case Minimization:
        m_d.goal.defineValue(ui->goalValueEdit->text().toDouble());
        m_d.threshold.defineValue(ui->thresholdLineEdit->text().toDouble());
        break;
    case Maximization:
        m_d.goal.defineValue(-ui->goalValueEdit->text().toDouble());
        m_d.threshold.defineValue(-ui->thresholdLineEdit->text().toDouble());
        break;
    default:
        break;
    }
    m_d.priority = ui->prioritySpinBox->value();
    m_d.setGoal = ui->setGoalcheckBox->isChecked();

    qDebug() << "setGoal  : "<< m_d.setGoal;
    qDebug() << "goal     : "<< m_d.goal.value<qreal>();
    qDebug() << "priority : "<< m_d.priority;
    qDebug() << "threshold: "<< m_d.threshold.value<qreal>();

    emit updateData(m_d);
}

void OutputPropertiesDialog::on_distributionComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1.isEmpty()) {
        return;
    }

    m_uncertainMappingModelData.clear();

    Tigon::DistributionType type = m_parametricDistMap[arg1];
    TStringList names = DistributionFactory::instance()->parameaterNames(type);
    TVector<qreal> constTerm, linearTerm;
    if(m_d.uncertainMapping() != Q_NULLPTR) {
        constTerm = m_d.uncertainMapping()->constTerms();
        linearTerm = m_d.uncertainMapping()->linearTerms();
    }
    if(constTerm.size() != names.size()) {
        fill(constTerm, 0.0, names.size());
    }
    if(linearTerm.size() != names.size()) {
        fill(linearTerm, 0.0, names.size());
    }

    m_uncertainMappingModelData
            .setHorizontalHeaderItem(0, new QStandardItem(tr("Term")));
    m_uncertainMappingModelData
            .setItem(0, 0, new QStandardItem(tr("Constant Term")));
    m_uncertainMappingModelData
            .setItem(1, 0, new QStandardItem(tr("Linear Term")));
    m_uncertainMappingModelData.item(0,0)->setEditable(false);
    m_uncertainMappingModelData.item(1,0)->setEditable(false);
    for(int i=0; i<names.size(); i++) {
        m_uncertainMappingModelData
                .setHorizontalHeaderItem(i+1,
                                         new QStandardItem(QString::fromStdString(names[i])));
        m_uncertainMappingModelData
                .setItem(0, i+1
                         ,new QStandardItem(QString::number(constTerm[i])));
        m_uncertainMappingModelData
                .setItem(1, i+1
                         , new QStandardItem(QString::number(linearTerm[i])));

    }

    // Distribute the columns
    int nHeaders = ui->uncertainParamTableView->horizontalHeader()->count();
    for(int i=0; i<nHeaders; i++) {
        if(i != nHeaders-1) {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
        } else {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
        }
    }

    return;
}

void QTigon::OutputPropertiesDialog::on_optTypeComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == Constants::MIN) {
        if(ui->objectiveCheckBox->isChecked()) {
            ui->goalValueEdit->setText(QString::number(Tigon::Lowest));
        }
        if(ui->constraintCheckBox->isChecked()) {
            ui->thresholdLineEdit->setText(QString::number(Tigon::Lowest));
        }
    } else if(arg1 == Constants::MAX) {
        if(ui->objectiveCheckBox->isChecked()) {
            ui->goalValueEdit->setText(QString::number(Tigon::Highest));
        }
        if(ui->constraintCheckBox->isChecked()) {
            ui->thresholdLineEdit->setText(QString::number(Tigon::Highest));
        }
    } else {
        ui->goalValueEdit->setText(QVariant().toString());
        ui->thresholdLineEdit->setText(QVariant().toString());
    }
}
