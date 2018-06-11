/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include "inputpropertiesdialog.h"
#include "ui_inputpropertiesdialog.h"
#include <qtigon/qtigonconstants.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Factories/DistributionFactory.h>

#include <QDebug>
#include <QMenu>
#include <QMessageBox>

using namespace QTigon;
using namespace Tigon;
using namespace Tigon::Representation;

InputPropertiesDialog::InputPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputPropertiesDialog)
{
    ui->setupUi(this);
    initialise();
    setupSignals();
}

void InputPropertiesDialog::setData(const InputPrivateData& data)
{
    /// [0] Set Data
    m_d = data; // assignment operator called. Uncertainty mapping is cloned

    /// [1] Reset ui elements
    ui->nameLineEdit->setText(m_d.name());
    ui->descriptionTextEdit->setText(m_d.description());
    ui->lboundLineEdit->setText(QString::number(m_d.varLbound.value()));
    ui->uBoundlineEdit->setText(QString::number(m_d.varUbound.value()));
    ui->funcsTextEdit->clear();
    ui->distComboBox->setCurrentIndex(0);
    ui->parameterValueLineEdit->setText(QString::number(m_d.value.value()));
    m_uncertainMappingModelData.clear();
    m_parameterDistModelData.clear();

    /// [2] Append function list
    for(int i=0; i<m_d.funcIndices.size(); i++) {
        ui->funcsTextEdit->append(m_d.funcIndices[i].data().toString() + "\n");
    }

    /// [3] Set Type Combobox
    switch(m_d.prts.type()) {
    case ElementType::RealType:
        ui->typeComboBox->setCurrentIndex(0);
        break;
    case ElementType::IntegerType:
        ui->typeComboBox->setCurrentIndex(1);
        break;
    case ElementType::OrdinalType:
        ui->typeComboBox->setCurrentIndex(2);
        break;
    case ElementType::NominalType:
        ui->typeComboBox->setCurrentIndex(3);
        break;
    default:
        break;
    }

    /// [4] Define uncertainty mappings for variables
    if(m_d.scope == Constants::VARIABLE) {
        ui->variableRadioButton->setChecked(true);
        if(m_d.uncertainMapping() == Q_NULLPTR) {
            // Deterministic
            ui->deterministicRadioButton->setChecked(true);
            showDetermisticVariablePrpt();
        } else {
            // Uncertain
            ui->uncertaintyRadioButton->setChecked(true);

            configureDistComboBox(m_parametricDistMap);

            QStringList keys = m_parametricDistMap.keys();
            DistributionType type = m_d.uncertainMapping()->distributionType();
            for(int i=0; i<m_parametricDistMap.size(); i++) {
                QString typestr = keys[i];
                if(m_parametricDistMap[typestr] == type) {
                    ui->distComboBox->setCurrentText(typestr);
                    break;
                }
            }
            showUncertainVariablePrpt();
        }
    }

    /// [5] Define distributions for parameters
    if(m_d.scope == Constants::PARAMETER) {
        ui->paramRadioButton->setChecked(true);
        ui->isExternalCheckBox->setChecked(m_d.isExternal());

        if(m_d.value.dist() == Q_NULLPTR) {
            if(m_d.isExternal()) {
                // External default: uncertain
                ui->uncertaintyRadioButton->setChecked(true);
                configureDistComboBox(m_allDistMap);
                showUncertainParameterPrpt();
            } else {
                // Paramter is deterministic
                qDebug() << "Paramter is deterministic";
                ui->deterministicRadioButton->setChecked(true);
                ui->parameterValueLineEdit->setText(QString::number(m_d.value.value<qreal>()));
                showDetermisticParameterPrpt();
            }

        } else {
            // Parameter is uncertain
            qDebug() << "Parameter is uncertain";
            ui->uncertaintyRadioButton->setChecked(true);
            configureDistComboBox(m_allDistMap);

            QStringList keys = m_allDistMap.keys();
            DistributionType type = m_d.value.dist()->type();
            for(int i=0; i<m_allDistMap.size(); i++) {
                QString typestr = keys[i];
                if(m_allDistMap[typestr] == type) {
                    // The following call also involves building the data model
                    ui->distComboBox->setCurrentText(typestr);
                    break;
                }
            }
            showUncertainParameterPrpt();
        }
    }
}

void InputPropertiesDialog::setDistributionMaps(const QMap<QString, DistributionType> &map)
{
    m_allDistMap = map;
    QStringList distributions = m_allDistMap.keys();
    distributions.insert(0,"");
    ui->distComboBox->clear();
    ui->distComboBox->addItems(distributions);
}

InputPropertiesDialog::~InputPropertiesDialog()
{
    delete ui;
}

void InputPropertiesDialog::on_buttonBox_accepted()
{
    bool bounsError = m_d.varUbound.value() < m_d.varLbound.value();
    // Check data
    if(bounsError) {
        QMessageBox::critical(this, tr("Inappropriate Bounds"),
                              QString("The upper bound must be >= the lower bound."),
                              QMessageBox::Ok);
        this->show();
        this->raise();
        return;
    }

    // Send data if no error
    emit updateData(m_d);
}

void InputPropertiesDialog::on_typeComboBox_currentIndexChanged(int index)
{
    switch(index) {
    case 0:
        m_d.prts.setType(ElementType::RealType);
        m_d.value.defineType(ElementType::RealType);
        break;
    case 1:
        m_d.prts.setType(ElementType::IntegerType);
        m_d.value.defineType(ElementType::IntegerType);
        break;
    case 2:
        m_d.prts.setType(ElementType::OrdinalType);
        m_d.value.defineType(ElementType::OrdinalType);
        break;
    case 3:
        m_d.prts.setType(ElementType::NominalType);
        m_d.value.defineType(ElementType::NominalType);
        break;
    default:
        break;
    }

    qDebug() << m_d.prts.type();
}

void InputPropertiesDialog::updateUncertaintyMappingParameters(const QModelIndex &topleft,
                                                               const QModelIndex &bottomright,
                                                               const QVector<int> &roles)
{
    Q_UNUSED(topleft)
    Q_UNUSED(bottomright)
    Q_UNUSED(roles)
    if(m_d.uncertainMapping() == Q_NULLPTR) {
        return;
    }

    int nrows = m_uncertainMappingModelData.rowCount();
    int ncols = m_uncertainMappingModelData.columnCount();
    for(int i=0; i<nrows; i++) {
        for(int j=1; j<ncols; j++) {
            QModelIndex index = m_uncertainMappingModelData.index(i, j);
            if(m_uncertainMappingModelData.data(index).isNull()) {
                qDebug() << "Not fully defined";
                return;
            }
        }
    }

    Tigon::TVector<qreal> lnterms = m_d.uncertainMapping()->linearTerms();
    Tigon::TVector<qreal> cnstterms = m_d.uncertainMapping()->constTerms();
    int nParams = m_d.uncertainMapping()->nParams();
    if(lnterms.empty()) {
        lnterms.resize(nParams);
    }

    for(int i=0; i<nrows; i++) {
        for(int j=1; j<ncols; j++) {
            QModelIndex index = m_uncertainMappingModelData.index(i,j);
            if(i==0) {
                cnstterms[j-1] = m_uncertainMappingModelData.data(index).toReal();
            }
            if(i==1) {
                lnterms[j-1] = m_uncertainMappingModelData.data(index).toReal();
            }
        }
    }
    m_d.uncertainMapping()->defineConstTerms(cnstterms);
    m_d.uncertainMapping()->defineLinearTerms(lnterms);
}

void InputPropertiesDialog::updateParameterDistData(const QModelIndex &topleft,
                                                    const QModelIndex &bottomright,
                                                    const QVector<int> &roles)
{
    Q_UNUSED(topleft)
    Q_UNUSED(bottomright)
    Q_UNUSED(roles)

    if(m_d.value.dist() == Q_NULLPTR) {
        qDebug() << "dist is null";
        return;
    }

    int nCols  = m_parameterDistModelData.columnCount();
    int nParam = m_d.value.dist()->parameterNames().size();

    if(nCols < nParam) {
        qDebug() << "nCols < nParam";
        return;
    }

    if(nParam > 0) {
        // Parametric distribution
        for(int i=0; i<nCols; i++) {
            QModelIndex index = m_parameterDistModelData.index(0, i);
            if(m_parameterDistModelData.data(index).isNull()) {
                qDebug() << "not fully defined - need " << nParam << "paramters";
                return;
            }
        }

        Tigon::TVector<qreal> param;
        for(int i=0; i<nCols; i++) {
            QModelIndex index = m_parameterDistModelData.index(0, i);
            param.push_back(m_parameterDistModelData.data(index).toReal());
        }
        m_d.value.dist()->defineParameters(param);

    } else {
        // Nonparametric
        /// \todo This is only for Sample distribution. Make it generic
        /// \todo Update this part after we finalise the structure of IDistruction
        int nSamples = ui->uncertainParamTableView->model()->rowCount();

        // Check inputs
        for(int i=0; i<nSamples; i++) {
            QModelIndex index1 = ui->uncertainParamTableView->model()->index(i,0);
            QModelIndex index2 = ui->uncertainParamTableView->model()->index(i,1);
            if(index1.data().isNull() && index2.data().isNull()) {
                return;
            }
            if(!index1.data().isNull() && index2.data().isNull()) {
                ui->uncertainParamTableView->model()->setData(index2, 1);
            }
            if(!index1.data().isNull() && !index2.data().isNull()) {
                if(index2.data().toInt() < 1) {
                    ui->uncertainParamTableView->model()->setData(index2, 1);
                }
            }
        }

        Tigon::TVector<qreal> samples, weights;
        for(int i=0; i<nSamples; i++) {
            QModelIndex index1 = ui->uncertainParamTableView->model()->index(i,0);
            QModelIndex index2 = ui->uncertainParamTableView->model()->index(i,1);
            samples.push_back(index1.data().toReal());
            weights.push_back(index2.data().toReal());
        }
        m_d.value.dist()->defineSamples(samples, weights);
    }
}

void InputPropertiesDialog::customContextMenuForNonParametricDist(const QPoint &point)
{
    QPoint globalPos = ui->uncertainParamTableView->mapToGlobal(point);
    QModelIndex idx = ui->uncertainParamTableView->indexAt(point);

    QMenu myMenu;
    myMenu.addAction(tr("Append"), this, SLOT(appendOneRow()));
    if(idx.row() != -1) {
        myMenu.addAction(tr("Insert Above"), this, SLOT(insertAbove()));
        myMenu.addAction(tr("Insert Below"), this, SLOT(insertBelow()));
        myMenu.addAction(tr("Remove"),  this, SLOT(removeOneRow()));
    }
    myMenu.addAction(tr("Load from file..."), this, SLOT(loadRawData()));
    myMenu.exec(globalPos);
}

void InputPropertiesDialog::appendOneRow()
{
    int rowCount = ui->uncertainParamTableView->model()->rowCount();
    ui->uncertainParamTableView->model()->insertRow(rowCount);
}

void InputPropertiesDialog::insertBelow()
{
    QModelIndex index = ui->uncertainParamTableView->currentIndex();
    ui->uncertainParamTableView->model()->insertRow(index.row()+1);
}

void InputPropertiesDialog::insertAbove()
{
    QModelIndex index = ui->uncertainParamTableView->currentIndex();
    ui->uncertainParamTableView->model()->insertRow(index.row());
}

void InputPropertiesDialog::removeOneRow()
{
    QModelIndex index = ui->uncertainParamTableView->currentIndex();
    ui->uncertainParamTableView->model()->removeRow(index.row());
    int nSamples = ui->uncertainParamTableView->model()->rowCount();

    // Check inputs
    Tigon::TVector<qreal> samples, weights;
    for(int i=0; i<nSamples; i++) {
        QModelIndex index1 = ui->uncertainParamTableView->model()->index(i,0);
        QModelIndex index2 = ui->uncertainParamTableView->model()->index(i,1);
        if(index1.data().isNull() && index2.data().isNull()) {
            return;
        }
        if(!index2.data().isNull()) {
            if(index2.data().toInt() < 1) {
                return;
            }
        }
        samples.push_back(index1.data().toReal());
        weights.push_back(index2.data().toReal());
    }
    m_d.value.dist()->defineSamples(samples, weights);
}

void InputPropertiesDialog::loadRawData()
{
    ///\todo Load samples and weights from txt or csv documents
}

void InputPropertiesDialog::on_parameterValueLineEdit_textChanged(const QString &arg1)
{
    IElementSPtr elept = IElementSPtr(new IElement(m_d.value.type(), arg1.toDouble()));
    m_d.value.defineValue(elept->value());
}

void InputPropertiesDialog::on_lboundLineEdit_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty()) {
        m_d.varLbound = arg1.toDouble();
    } else {
        m_d.varLbound = Lowest;
    }
}

void InputPropertiesDialog::on_uBoundlineEdit_textChanged(const QString &arg1)
{
    if(!arg1.isEmpty()) {
        m_d.varUbound = arg1.toDouble();
    } else {
        m_d.varUbound = Highest;
    }
}

void InputPropertiesDialog::on_distComboBox_currentIndexChanged(const QString &arg1)
{
    qDebug() << "Chosen distribution type " << arg1;
    ///[0] Check incoming
    if(arg1.isEmpty()) {
        return;
    }

    if(m_allDistMap.contains(arg1)) {
        Tigon::DistributionType type = m_allDistMap[arg1];
        if(ui->variableRadioButton->isChecked()) {
            ///[1] Variables
            qDebug() << "Check m_d.getUncertainMapping() :" << m_d.uncertainMapping();
            if(m_d.uncertainMapping() == Q_NULLPTR) {
                // create uncertain mapping
                m_d.setUncertainMapping(new UncertaintyMapping);
            }
            // Define type if the distribution type has changed
            if(m_d.uncertainMapping()->distributionType() != type) {
                m_d.uncertainMapping()->defineDistributionType(type);
            }

            buildVariableDistributionView();

        } else if(ui->paramRadioButton->isChecked()) {
            ///[1] Parameters
            // The current element has no distribution,
            // or we are changing the type of the distribution
            // create a new one
            if(m_d.value.dist() == Q_NULLPTR) {
                IDistribution* dist = DistributionFactory::instance()->createParametricDist(type);
                if(dist == Q_NULLPTR) {
                    // Non parametric distribution
                    dist = DistributionFactory::instance()->createNonParametricDist(type);
                }

                m_d.value.defineDist(dist);
            } else if(m_d.value.dist()->type() != type) {
                IDistribution* dist = DistributionFactory::instance()->createParametricDist(type);
                if(dist == Q_NULLPTR) {
                    // Non parametric distribution
                    dist = DistributionFactory::instance()->createNonParametricDist(type);
                }
                m_d.value.defineDist(dist);
            }

            buildParamterDistributionView();
        }

        ui->uncertainParamTableView->setEnabled(true);
        // Distribute the columns
        int nHeaders = ui->uncertainParamTableView->horizontalHeader()->count();
        for(int i=0; i<nHeaders; i++) {
            if(i != nHeaders-1) {
                ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
            } else {
                ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
            }
        }

    } else {
        // Distribution not supported
        ui->uncertainParamTableView->setDisabled(true);
    }
}

void InputPropertiesDialog::configureDistComboBox(QMap<QString, DistributionType> distMap)
{
    ui->distComboBox->clear();
    QStringList distributions = distMap.keys();
    distributions.insert(0,"");
    qDebug() <<  distributions;
    ui->distComboBox->addItems(distributions);
}

void InputPropertiesDialog::initialise()
{
    /// [0] Set read-only and appearance
    ui->descriptionTextEdit->setReadOnly(true);
    ui->nameLineEdit->setReadOnly(true);
    ui->funcsTextEdit->setReadOnly(true);
    ui->isExternalCheckBox->setDisabled(true);

    /// [1] Uncertainty Parameter view
    ui->uncertainParamTableView->setModel(&m_uncertainMappingModelData);
    ui->uncertainParamTableView->setDisabled(true);

    ui->uncertainParamTableView->horizontalHeader()->setStretchLastSection(true);
    m_uncertainMappingModelData.setHorizontalHeaderItem(0, new QStandardItem(tr("Term")));
    m_uncertainMappingModelData.setHorizontalHeaderItem(1, new QStandardItem(tr(Constants::PARAMETER)));
    m_uncertainMappingModelData.setItem(0, 0, new QStandardItem(tr("Constant Terms")));
    m_uncertainMappingModelData.setItem(1, 0, new QStandardItem(tr("Linear Terms")));
    m_uncertainMappingModelData.item(0,0)->setEditable(false);
    m_uncertainMappingModelData.item(1,0)->setEditable(false);
    ui->uncertainParamTableView->setContextMenuPolicy(Qt::CustomContextMenu);

    /// [2] Parameter distributions
    m_parameterDistModelData.setHorizontalHeaderItem(0, new QStandardItem(tr(Constants::PARAMETER)));

    /// [3] Default distribution maps
    if(m_allDistMap.isEmpty()) {
        m_allDistMap[DTUniformDistType] = UniformDistType;
        m_allDistMap[DTNormalDistType]  = NormalDistType;
        m_allDistMap[DTLinearDistType]  = LinearDistType;
        m_allDistMap[DTPeakDistType]    = PeakDistType;
        m_allDistMap[DTSampledDistType] = SampledDistType;
    }

    if(m_parametricDistMap.isEmpty()) {
        m_parametricDistMap[DTUniformDistType] = UniformDistType;
        m_parametricDistMap[DTNormalDistType]  = NormalDistType;
        m_parametricDistMap[DTLinearDistType]  = LinearDistType;
        m_parametricDistMap[DTPeakDistType]    = PeakDistType;
    }

    /// [4] Validators
    /// \todo
}

void InputPropertiesDialog::setupSignals()
{
    connect(&m_uncertainMappingModelData,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,
            SLOT(updateUncertaintyMappingParameters(QModelIndex,QModelIndex,QVector<int>)));
    connect(&m_parameterDistModelData,
            SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,
            SLOT(updateParameterDistData(QModelIndex,QModelIndex,QVector<int>)));
}

// Build proper tableview
void InputPropertiesDialog::buildParamterDistributionView()
{
    // Clear data
    m_parameterDistModelData.clear();

    if(m_d.value.dist()!=Q_NULLPTR) {
        TStringList names = m_d.value.dist()->parameterNames();
        if(names.size() > 0) {
            // Parametric distribution
            TVector<qreal> params = m_d.value.dist()->parameters();
            if(params.size() == names.size()) {
                for(int i=0; i<names.size(); i++) {
                    m_parameterDistModelData
                            .setHorizontalHeaderItem(i,
                                                     new QStandardItem(QString::fromStdString(names[i])));
                    m_parameterDistModelData
                            .setItem(0, i,
                                     new QStandardItem(QString::number(params[i])));
                }
            }
            return;
        } else {
            // Nonparametric distribution
            m_parameterDistModelData.setHorizontalHeaderItem(0, new QStandardItem(tr("Samples")));
            m_parameterDistModelData.setHorizontalHeaderItem(1, new QStandardItem(tr("Weights")));

            /// \todo generic way
            int nSamples = m_d.value.dist()->sampledVec().size();
            Tigon::TVector<qreal> samples = m_d.value.dist()->sampledVec();
            Tigon::TVector<qreal> weights = m_d.value.dist()->weights();
            if(nSamples == 0) {
                m_parameterDistModelData.setItem(0, 0, new QStandardItem());
                m_parameterDistModelData.setItem(0, 1, new QStandardItem());
            } else {
                for(int i=0; i<nSamples; i++) {
                    m_parameterDistModelData.setItem(i, 0,
                                                     new QStandardItem(QString::number(samples[i])));
                    m_parameterDistModelData.setItem(i, 1,
                                                     new QStandardItem(QString::number(weights[i])));
                }
            }
            return;
        }
    }

    // No distribution generated or there is some mistake
    m_parameterDistModelData.setHorizontalHeaderItem(0, new QStandardItem(tr(Constants::PARAMETER)));
    m_parameterDistModelData.setItem(0, 0, new QStandardItem());
}

void InputPropertiesDialog::buildVariableDistributionView()
{
    // Reset
    m_uncertainMappingModelData.clear();
    m_uncertainMappingModelData.setHorizontalHeaderItem(0, new QStandardItem(tr("Term")));
    m_uncertainMappingModelData.setItem(0, 0, new QStandardItem(tr("Constant Terms")));
    m_uncertainMappingModelData.setItem(1, 0, new QStandardItem(tr("Linear Terms")));

    if(m_d.uncertainMapping()!=Q_NULLPTR) {
        Tigon::TStringList names = DistributionFactory::instance()->parameaterNames(
                    m_d.uncertainMapping()->distributionType());
        Tigon::TVector<qreal> constTerm  = m_d.uncertainMapping()->constTerms();
        Tigon::TVector<qreal> linearTerm = m_d.uncertainMapping()->linearTerms();

        if(constTerm.size() != names.size()) {
            Tigon::fill(constTerm, 0.0, names.size());
        }
        if(linearTerm.size() != names.size()) {
            Tigon::fill(linearTerm, 0.0, names.size());
        }

        bool isDataReady = constTerm.size() == names.size();
        if(isDataReady) {
            for(int i=0; i<names.size(); i++) {
                m_uncertainMappingModelData
                        .setHorizontalHeaderItem(i+1, new QStandardItem(
                                                     QString::fromStdString(names[i])));

                m_uncertainMappingModelData
                        .setItem(0, i+1
                                 , new QStandardItem(QString::number(constTerm[i])));

                m_uncertainMappingModelData
                        .setItem(1, i+1
                                 , new QStandardItem(QString::number(linearTerm[i])));
            }
            return;
        }
    }

    // No distribution generated or there is some mistake
    m_uncertainMappingModelData.setHorizontalHeaderItem(1, new QStandardItem(tr(Constants::PARAMETER)));
    m_uncertainMappingModelData.setItem(0, 1, new QStandardItem());
    m_uncertainMappingModelData.setItem(1, 1, new QStandardItem());
}

void InputPropertiesDialog::showDetermisticVariablePrpt()
{
    ui->propertiesStackedWidget->setCurrentIndex(1);
    ui->uncertainGroupbox->hide();
    ui->boundsGroupBox->show();
}

void InputPropertiesDialog::showDetermisticParameterPrpt()
{
    ui->propertiesStackedWidget->setCurrentIndex(0);
}

void InputPropertiesDialog::showUncertainVariablePrpt()
{
    disconnect(ui->uncertainParamTableView, SIGNAL(customContextMenuRequested(QPoint)),
               this, SLOT(customContextMenuForNonParametricDist(QPoint)));

    ui->uncertainParamTableView->reset();
    ui->uncertainParamTableView->setModel(&m_uncertainMappingModelData);
    // Distribute the columns
    int nHeaders = ui->uncertainParamTableView->horizontalHeader()->count();
    for(int i=0; i<nHeaders; i++) {
        if(i != nHeaders-1) {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
        } else {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
        }
    }
    ui->uncertainParamTableView->horizontalHeader()->setStretchLastSection(true);
    ui->propertiesStackedWidget->setCurrentIndex(1);
    ui->boundsGroupBox->show();
    ui->uncertainGroupbox->show();
}

void InputPropertiesDialog::showUncertainParameterPrpt()
{
    connect(ui->uncertainParamTableView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(customContextMenuForNonParametricDist(QPoint)));

    ui->uncertainParamTableView->reset();
    ui->uncertainParamTableView->setModel(&m_parameterDistModelData);
    // Distribute the columns
    int nHeaders = ui->uncertainParamTableView->horizontalHeader()->count();
    for(int i=0; i<nHeaders; i++) {
        if(i != nHeaders -1) {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::Stretch);
        } else {
            ui->uncertainParamTableView->horizontalHeader()->setResizeMode(i, QHeaderView::ResizeToContents);
        }
    }
    ui->uncertainParamTableView->horizontalHeader()->setStretchLastSection(true);
    ui->propertiesStackedWidget->setCurrentIndex(1);
    ui->boundsGroupBox->hide();
    ui->uncertainGroupbox->show();
}

void InputPropertiesDialog::setParametricDistMap(const QMap<QString, Tigon::DistributionType> &parametricDistMap)
{
    m_parametricDistMap = parametricDistMap;
}

void InputPropertiesDialog::setReadOnly(bool readOnly)
{
    QList<QWidget *> widgets = this->findChildren<QWidget *>();
    foreach(QWidget* widget, widgets) {
        widget->setDisabled(readOnly);
    }
}

void InputPropertiesDialog::on_uncertaintyRadioButton_toggled(bool checked)
{
    if(checked) {
        qDebug() << "uncertain checked";
        if(ui->paramRadioButton->isChecked()) {
            configureDistComboBox(m_allDistMap);
            showUncertainParameterPrpt();
        } else {
            configureDistComboBox(m_parametricDistMap);
            showUncertainVariablePrpt();
        }
    }
}

void InputPropertiesDialog::on_deterministicRadioButton_toggled(bool checked)
{
    if(checked) {
        if(ui->paramRadioButton->isChecked()) {
            m_d.value.defineDist(Q_NULLPTR);
            showDetermisticParameterPrpt();
        } else {
            m_d.setUncertainMapping(Q_NULLPTR);
            showDetermisticVariablePrpt();
        }
    }
}

void InputPropertiesDialog::on_variableRadioButton_toggled(bool checked)
{
    if(checked) {
        m_d.scope = Constants::VARIABLE;
        if(ui->uncertaintyRadioButton->isChecked()) {
            configureDistComboBox(m_parametricDistMap);
            ui->distComboBox->setCurrentIndex(0);
            showUncertainVariablePrpt();
        } else {
            showDetermisticVariablePrpt();
        }
    }
}

void InputPropertiesDialog::on_paramRadioButton_toggled(bool checked)
{
    if(checked) {
        m_d.scope = Constants::PARAMETER;
        if(ui->uncertaintyRadioButton->isChecked()) {
            configureDistComboBox(m_allDistMap);
            ui->distComboBox->setCurrentIndex(0);
            showUncertainParameterPrpt();
        } else {
            // Deterministic
            showDetermisticParameterPrpt();
        }
    } else {
        ui->isExternalCheckBox->setChecked(checked);
    }
    ui->isExternalCheckBox->setEnabled(checked);
}

void QTigon::InputPropertiesDialog::on_isExternalCheckBox_toggled(bool checked)
{
    ///\note By default, if a parameter is marked as external, we expect the
    /// user to provide some prior information of this parameter. Later on during
    /// the optimization, this prior information will be replaced by the surrogate
    /// model trained from the data.
    if(checked) {
        ui->uncertaintyRadioButton->setChecked(true);
        ui->uncertaintyButtonsContainer->setDisabled(true);
    } else {
        ui->uncertaintyButtonsContainer->setDisabled(false);
        ui->deterministicRadioButton->setChecked(true);
        if(ui->uncertaintyRadioButton->isChecked()) {
            ui->propertiesStackedWidget->setCurrentIndex(1);
        } else {
            ui->propertiesStackedWidget->setCurrentIndex(0);
        }
    }
    m_d.setExternal(checked);
}
