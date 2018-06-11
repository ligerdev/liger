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
#include "ui_visualisationwidget.h"
#include <visualisation/core/visualisationwidget.h>
#include <visualisation/core/visualisationdatamodel.h>
#include <visualisation/core/visualisationviewmodel.h>

#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QSvgGenerator>
#include <QDebug>

using namespace Visualisation;

enum ImageFormat {
    IMG_SVG,
    IMG_PDF,
    IMG_EPS,
    IMG_JPEG,
    IMG_JPG,
    IMG_PNG,
};

VisualisationWidget::VisualisationWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VisualisationWidget)
    , m_data(0)
    , m_view(0)
{
    ui->setupUi(this);
    ui->centralwidget->setContentsMargins(0,0,0,0);
}

VisualisationWidget::~VisualisationWidget()
{
    delete ui;
    delete m_data;
    delete m_view;
}

QMenuBar *VisualisationWidget::menuBar() const
{
    return ui->menubar;
}

QMenu *VisualisationWidget::menuFile() const
{
    return ui->menuFile;
}

QMenu *VisualisationWidget::menuEdit() const
{
    return ui->menuEdit;
}

QAction *VisualisationWidget::addActionToMenuFile(const QString &actionName)
{
    return ui->menuFile->addAction(actionName);
}

QAction *VisualisationWidget::addActionToMenuEdit(const QString &actionName)
{
    return ui->menuEdit->addAction(actionName);
}

void VisualisationWidget::addItemToToolBar(QWidget* item)
{
    ui->toolBar->addWidget(item);
}

void VisualisationWidget::addSpacerToToolBar()
{
    ui->toolBar->addSeparator();
}

VisualisationViewModel *VisualisationWidget::view() const
{
    return m_view;
}

VisualisationDataModel *VisualisationWidget::data() const
{
    return m_data;
}

void VisualisationWidget::showVariableSelectionForm()
{
    // Default do nothing
}

void VisualisationWidget::setData(const QVector<QVector<qreal> > &dataVec,
                                      const QStringList &names)
{
    m_data->setData(dataVec, names);
}

void VisualisationWidget::setData(const QVector<QVector<qreal> > &dataVec)
{
    m_data->setData(dataVec);
}

void VisualisationWidget::setNames(const QStringList &names)
{
    m_data->setNames(names);
}

void VisualisationWidget::setCategories(const QVariantList &colCategories)
{
    m_data->setColCategories(colCategories);
}

void VisualisationWidget::setSelectedIndices(const QVariantList &selected)
{
    m_data->setSelectedIndices(selected);
}

void VisualisationWidget::setBoxPlotData(const QVector<QVector<QVector<qreal> > > &boxplotData)
{
    m_data->setBoxplotData(boxplotData);
}

void VisualisationWidget::setGoals(const QVariantList &goals)
{
    m_data->setGoals(goals);
}

void VisualisationWidget::setThresholds(const QVariantList &thresholds)
{
    m_data->setThresholds(thresholds);
}

void VisualisationWidget::setPreferenceDirerctions(const QVariantList &preferenceDirections)
{
    m_data->setPreferenceDirerctions(preferenceDirections);
}

QVector<QVector<qreal> > VisualisationWidget::rawData() const
{
    return m_data->rawData();
}

QStringList VisualisationWidget::allNames() const
{
    return m_data->allNames();
}

QVariantList VisualisationWidget::categories() const
{
    return m_data->colCategories();
}

QVariantList VisualisationWidget::selectedIndices() const
{
    return m_data->selectedIndices();
}

QList<int> VisualisationWidget::brushedIndices() const
{
    return m_data->brushedIndices();
}

QMap<int, QVector<qreal> > VisualisationWidget::brushedBounds() const
{
    return m_data->brushedBounds();
}

void VisualisationWidget::resetBrushedIndices()
{
    m_data->resetBrushedIndices();
}

void VisualisationWidget::reloadView()
{
    m_view->reload();
}

void VisualisationWidget::on_actionSave_triggered()
{
    QString fname = QFileDialog::getSaveFileName(this,"Save...",
                                                 QDir::homePath(),
                                                 "Image Type (*.png)");
    if(fname.isEmpty()) {
        return;
    }

    QImage image(m_view->geometry().width(), m_view->geometry().height(),
                 QImage::Format_ARGB32_Premultiplied);

    //image.fill(Qt::transparent);
    image.fill(Qt::white);
    QPainter painter(&image);
    m_view->render(&painter);
    painter.end();
    image.save(fname);
}

void VisualisationWidget::on_actionSave_As_triggered()
{
    QString selectedFilter;
    QString nfilters("PNG (*.png);;SVG (*.svg);;PDF (*.pdf);;EPS (*.eps);;TIFF (*.tiff);;JPG (*.jpg);;JPEG (*.jpeg)");
    QString fname = QFileDialog::getSaveFileName(this, "Save As...",
                                                 QDir::homePath(),
                                                 nfilters,
                                                 &selectedFilter);
    if(fname.isEmpty()) {
        return;
    }


    QFileInfo finfo = QFileInfo(fname);

    QString suffix;
    QString bpath  = finfo.absolutePath();
    QString bfname = finfo.baseName();

    ImageFormat format;

    if(selectedFilter=="PNG (*.png)") {
        format = IMG_PNG;
        suffix = "png";
    }
    if(selectedFilter=="SVG (*.svg)") {
        format = IMG_SVG;
        suffix = "svg";
    }
    if(selectedFilter=="PDF (*.pdf)") {
        format = IMG_PDF;
        suffix = "pdf";
    }
    if(selectedFilter=="EPS (*.eps)") {
        format = IMG_EPS;
        suffix = "eps";
    }
    if(selectedFilter=="JPG (*.jpg)") {
        format = IMG_JPG;
        suffix = "jpg";
    }
    if(selectedFilter=="JPEG (*.jpeg)") {
        format = IMG_JPEG;
        suffix = "jpeg";
    }

    fname = bpath+"/"+bfname+"."+suffix;
    fname = QDir::toNativeSeparators(fname);

    QPainter painter;
    int width = m_view->geometry().width();
    int height= m_view->geometry().height();

    switch(format) {
    case IMG_SVG: {
        QSvgGenerator svg;
        svg.setFileName(fname);
        QSize size(width,height);
        svg.setSize(size);
        painter.begin(&svg);
        m_view->render(&painter);
        painter.end();
    }
        break;

        /// \todo Yiming PDF and EPS printing not working
    case IMG_EPS:
    case IMG_PDF: {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        //printer.setPageSize(QPrinter::A4);
        printer.setResolution(QPrinter::HighResolution);
        printer.setOutputFileName(fname);
        if (!painter.begin(&printer)) { // failed to open file
             QMessageBox::warning(this,
                                  "Cannot Print",
                                   "failed to open file, is it writable?");
             return;
        }
        m_view->render(&painter);
        painter.end();
    }
        break;

    case IMG_JPEG:
    case IMG_JPG:
    case IMG_PNG: {
        QSize size(width,height);
        QImage image(size, QImage::Format_ARGB32);
        image.fill(Qt::white);
        painter.begin(&image);
        m_view->render(&painter);
        painter.end();
        // TODO -Yiming Add quality control
        image.save(fname, suffix.toLocal8Bit(), 100);
    }
        break;

    default:
        QMessageBox::warning(this,"Unknown file extension.","Unknown file extension. File not saved.");
    }
}

void VisualisationWidget::on_actionSelect_Vars_triggered()
{
    showVariableSelectionForm();
}

void VisualisationWidget::setView(VisualisationViewModel *view)
{
    m_view = view;
    m_data = view->dataModel();
    centralWidget()->layout()->addWidget(view);
    connect(m_data, &VisualisationDataModel::brushedIndicesChanged,
            this, &VisualisationWidget::brushedIndicesUpdated);
    connect(this, &VisualisationWidget::setDisplayPreferences,
            m_data, &VisualisationDataModel::setDisplayPreferences);
}
