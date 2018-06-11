/****************************************************************************
**
** Copyright (C) 2012-2013 The University of Sheffield (www.sheffield.ac.uk)
**
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
#include "{% filter lower %}{{ StringName }}{% endfilter %}form.h"
#include "ui_{% filter lower %}{{ StringName }}{% endfilter %}form.h"

#include <visualization/visualizationconstants.h>
#include <QWebElement>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QSvgGenerator>
#include <QWebFrame>
#include <QDebug>

#include <QFileDialog>
#include <QMessageBox>
#include <QGridLayout>

using namespace {{Namespace}};

{{StringName}}Form::{{StringName}}Form(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::{{StringName}}Form)
{
    ui->setupUi(this);
    ui->gridLayout->setContentsMargins(0,0,0,0);
}

{{StringName}}Form::~{{StringName}}Form()
{
    delete ui;
}

QWebView* {{StringName}}Form::webView()
{
    return ui->webView;
}

void {{Namespace}}::{{StringName}}Form::on_actionSave_triggered()
{
    QString fname = QFileDialog::getSaveFileName(this,"Save...",
                                                 QDir::currentPath(),
                                                 "Image Type (*.png)");

    if(fname.isEmpty())
        return;

    // CHANGE ME!!!!!!!!!!!!
    QWebElement el = ui->webView->page()->mainFrame()->findFirstElement("#parcoords_plot");
    qDebug() << el.toPlainText();

    QImage image(ui->webView->geometry().width(),
                 ui->webView->geometry().height(),
                 QImage::Format_ARGB32_Premultiplied);
//    image.fill(Qt::transparent);
    image.fill(Qt::white);
    QPainter painter(&image);
    el.render(&painter);
    painter.end();
    image.save(fname);
}

void {{Namespace}}::{{StringName}}Form::on_actionSave_As_triggered()
{
    QString selectedFilter;
    QString nfilters("PNG (*.png);;SVG (*.svg);;BMP (*.bmp);;PDF (*.pdf);;PS (*.ps);;EPS (*.eps);;TIFF (*.tiff);;GIF (*.gif);;JPG (*.jpg);;JPEG (*.jpeg)");
    QString fname = QFileDialog::getSaveFileName(this, "Save As...",
                                             QDir::currentPath(),
                                             nfilters,
                                             &selectedFilter);

    qDebug() << "selectedFilter: " << selectedFilter;

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
    if(selectedFilter=="BMP (*.bmp)") {
        format = IMG_BMP;
        suffix = "bmp";
    }
    if(selectedFilter=="PDF (*.pdf)") {
        format = IMG_PDF;
        suffix = "pdf";
    }
    if(selectedFilter=="PS (*.ps)") {
        format = IMG_PS;
        suffix = "ps";
    }
    if(selectedFilter=="EPS (*.eps)") {
        format = IMG_EPS;
        suffix = "eps";
    }
    if(selectedFilter=="TIFF (*.tiff)") {
        format = IMG_TIFF;
        suffix = "tiff";
    }
    if(selectedFilter=="GIF (*.gif)") {
        format = IMG_GIF;
        suffix = "gif";
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
    qDebug() << "fname: " << fname;

    QPainter painter;
    // CHANGE ME!!!!!!!
    QWebElement el = ui->webView->page()->mainFrame()->findFirstElement("#parcoords_plot");
    int width = ui->webView->geometry().width();
    int height= ui->webView->geometry().height();

    switch(format) {
    case IMG_SVG: {
        QSvgGenerator svg;
        svg.setFileName(fname);
        QSize size(width,height);
        svg.setSize(size);
        painter.begin(&svg);
        el.render(&painter);
        painter.end();
    }
        break;

    case IMG_EPS:
    case IMG_PS:
    case IMG_PDF: {
        QPrinter printer;
        printer.setPageSize(QPrinter::A4);
        printer.setOutputFileName(fname);
        // TODO: change quality here?
        QWebFrame* frame = ui->webView->page()->mainFrame();
        frame->print(&printer);
    }
        break;
        
    case IMG_TIFF:
    case IMG_GIF:
    case IMG_BMP:
    case IMG_JPEG:       
    case IMG_JPG:
    case IMG_PNG: {
        QSize size(width,height);
        QImage image(size, QImage::Format_ARGB32);
        image.fill(Qt::white);
        painter.begin(&image);
        el.render(&painter);
        painter.end();
        // TODO: add quality
        image.save(fname, suffix.toLocal8Bit(), 100);
    }
        break;
        
    default:
        int ret = QMessageBox::warning(this,"Unknown file extension.","Unknown file extension. File not saved.");
    }

}
