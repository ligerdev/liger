/****************************************************************************
**
** Copyright (C) 2012-2017 The University of Sheffield (www.sheffield.ac.uk)
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
#include "populationviewer.h"
#include "ui_populationviewer.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

PopulationViewer::PopulationViewer(QWidget *parent) :
    QOperatorDiagTabItem(parent),
    ui(new Ui::PopulationViewer)
{
    ui->setupUi(this);
}

PopulationViewer::~PopulationViewer()
{
    delete ui;
}

void PopulationViewer::reset()
{
    ui->filePathLineEdit->clear();
    ui->dataTableWidget->clear();
    ui->dataTableWidget->setColumnCount(0);
    ui->dataTableWidget->setRowCount(0);

    emit updateFilePath(ui->filePathLineEdit->text());
}

void PopulationViewer::save()
{
    if(!m_data.isEmpty() && m_json.isEmpty()) {
        emit updateFromPoupluationViewer(m_data);
    } else {
        emit updateJsonFromPopViewr(m_json);
    }
    emit updateFilePath(ui->filePathLineEdit->text());
}

void PopulationViewer::on_loadPushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(0,
                                                tr("Open Data File"),
                                                QDir::homePath(),
                                                QString("Json (*.json);;"
                                                        "Text (*.txt);;"));
    loadDataFromFile(file);
    emit updateFilePath(ui->filePathLineEdit->text());
}

QVector<QVector<qreal> > PopulationViewer::data() const
{
    return m_data;
}

QJsonObject PopulationViewer::jsonData() const
{
    return m_json;
}

void PopulationViewer::setFilePath(QString file)
{
    loadDataFromFile(file);
}

void PopulationViewer::on_clearPushButton_clicked()
{
    reset();
}

void PopulationViewer::selectDefaultPopulation()
{

}

void PopulationViewer::loadDataFromFile(const QString& file)
{
    if(file.isEmpty()) {
        return;
    }

    ui->filePathLineEdit->setText(file);

    QFile ifile(file);
    if (!ifile.open(QFile::ReadOnly)) {
        return;
    }

    QFileInfo fileInfo(file);
    QString suffix= fileInfo.suffix();

    m_data.clear();

    if(suffix == "txt") {
        /*!
         * \note Format:
         * First line: #Var
         * Second line: #Obj
         * Thirdline: var and obj names
         * Fourth onward: data
         */

        int nVar = 0;
        int nObj = 0;
        QStringList names;

        QTextStream itextStream(&ifile);
        QString line;
        int counter = 0;
        while (itextStream.readLineInto(&line)) {
            qDebug() << line;
            if(line.contains("#Var")) {
                QStringList list = line.split(",");
                if(list.size() == 2) {
                    nVar = list[1].toInt();
                    ++counter;
                    qDebug() << nVar;
                    continue;
                } else {
                    return;
                }
            }

            if(line.contains("#Obj")) {
                QStringList list = line.split(",");
                if(list.size() == 2) {
                    nObj = list[1].toInt();
                    ++counter;
                    qDebug() << nObj;
                    continue;
                } else {
                    return;
                }
            }

            if(counter == 2) {
                names = line.split(",");
                if(names.size() < nVar) {
                    return;
                }
                qDebug() << names;
                ++counter;
                continue;
            }

            if(counter > 2) {
                // read data
                QVector<qreal> d;
                QStringList dataStr = line.split(",");
                // Only keep dicision variables
                dataStr.erase(dataStr.begin()+nVar, dataStr.end());
                qDebug() << dataStr;
                foreach(const QString& dataItem, dataStr) {
                    d.append(dataItem.toDouble());
                }
                m_data.append(d);
                ++counter;
            }
        }
        ifile.close();

        /// Populate the GUI
        ui->dataTableWidget->setColumnCount(nVar);
        ui->dataTableWidget->setRowCount(m_data.size());

        // headers
        for(int i=0; i<nVar; i++) {
            ui->dataTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(names[i]));
        }
        // data
        for(int i=0; i<m_data.size(); i++) {
            for(int j=0; j<m_data[i].size(); j++) {
                ui->dataTableWidget->
                        setItem(i, j, new QTableWidgetItem(QString::number(m_data[i][j])));
            }
        }

    } else if(suffix == "xml") {
        /// \todo
        return;

    } else if(suffix == "json") {
        qDebug() << "suffix is json";
        QString str = ifile.readAll();
        ifile.close();
        //qDebug() << "JSON loaded: " << str;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
        if(jsonDoc.isNull()) {
            return;
        }
        if(jsonDoc.isArray()) {
            qDebug() << "ERROR - JSON is not an object but an array: " << jsonDoc.array();
        } else if(jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            if(jsonObj.contains("Populations")) {
                m_json = jsonObj;
                //qDebug() << "JSON is a Populations log: " << m_json;
            } else if(jsonObj.contains("Evaluations")) {
                m_json = jsonObj;
                //qDebug() << "JSON is an Evaluations log: " << m_json;
            } else if(jsonObj.contains("Single Population")) {
                m_json = jsonObj;
                //qDebug() << "JSON is a single population: " << m_json;
            } else if(jsonObj.contains("Decision Vec")){
                m_json = jsonObj;
                //qDebug() << "JSON is a single evaluation: " << m_json;
            } else {
                //qDebug() << "ERROR - Unidentified JSON object: " << jsonObj;
            }
        } else {
            return;
        }

        /// Populate the GUI
        ui->dataTableWidget->setColumnCount(1);
        ui->dataTableWidget->setRowCount(1);

        // data
        ui->dataTableWidget->setItem(0, 0, new QTableWidgetItem("Json loaded"));

    } else {
        qDebug() << "suffix not recognised";
        return;
    }

}
