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
#include <network/inetwork.h>
#include <network/networkprotocolsyncthing.h>
#include <botan/botan.h>

#include <iostream>
#include <QtTest>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QFileInfo>

/* Before you start:
 * !! Please put syncthing executable in folder your-home-directory/syncthing !!
 */

using namespace std;
using namespace Network;
using namespace Network::Syncthing;

//#define GENERATE_USER_DB

class tst_network : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_Find_Syncthing();
    void test_Syncthing_EmptyConstructor();
    void test_Syncthing_Initialisation();
    void test_Syncthing_ResetNetwork();
    void test_Syncthing_StartShutdownService();
    void test_Syncthing_ConnectToNode();
#ifdef GENERATE_USER_DB
    void test_GenerateUserDatabase();
#endif
};

void tst_network::test_Find_Syncthing()
{
    QString syncthingPath = QDir(QDir::homePath()).filePath("syncthing/syncthing");
#ifdef _WIN32
    syncthingPath = syncthingPath + ".exe";
#endif
    QFileInfo file(syncthingPath);
    bool res = file.exists();
    if(!res) {
        std::cout << syncthingPath.toStdString() << "  does not exist" << std::endl;
        std::cout << "Please put " << file.fileName().toStdString() << " in folder "
                  << QDir(QDir::homePath()).filePath("syncthing").toStdString()
                  << "/"
                  << std::endl;
    }
    QCOMPARE(res, true);
    res = file.isExecutable();
    if(!res) {
        std::cout << syncthingPath.toStdString() << " is not executable" << std::endl;
    }
    QCOMPARE(res, true);
}

void tst_network::test_Syncthing_EmptyConstructor()
{
    INetwork* net = new NetworkProtocolSyncthing();
    delete net;
}

void tst_network::test_Syncthing_Initialisation()
{
    // TODO - remove location dependices
    QString syncthingPath = QDir(QDir::homePath()).filePath("syncthing/syncthing");
    std::cout << syncthingPath.toStdString() << std::endl;
    QString configPath = QDir(QDir::homePath()).filePath("liger-test-syncthing");
    std::cout << configPath.toStdString() << std::endl;
    // remove the folder if exists
    if(QDir(configPath).exists()) {
        QDir(configPath).removeRecursively();
    }

    QString sharedFolder = QDir(QDir::homePath()).filePath("liger-test-syncthing/LigerSharedFolder");
    QStringList threeIDs;
    threeIDs << "YXMK23I-VZBDZVP-SIPJ6RX-XUOJGEN-FE6VZLK-JFJ7S2K-PP3QQSH-5MOSEQO";
    threeIDs << "BTJDEXT-BDAE4MV-JQNIJUL-G3C5O62-2RQG7CE-EIPM6VU-HBLC7X6-WMEFHAI";
    threeIDs << "KEBVUY3-POF7LQH-3ULRD2J-GF5MY2K-6AYP27Q-5LQO5HM-4EH5EZT-JPTQMQQ";

    QStringList peers;
    peers << threeIDs[1];
    peers << threeIDs[2];

    NetworkProtocolSyncthing* net = new NetworkProtocolSyncthing(configPath, sharedFolder);
    net->setExecutable(syncthingPath);
    //net->defineNodeID(threeIDs[0]);
    net->setPeers(peers);
    for(int i = 0; i < peers.size(); i++) {
        QCOMPARE(net->peers()[i], peers[i]);
    }

    bool success = net->initialiseService();
    std::cout << "node ID:" << net->nodeID().toStdString() << std::endl;
    QCOMPARE(success, true);
    QCOMPARE(net->isInitialised(),true);

    NetworkProtocolSyncthing* net2 = new NetworkProtocolSyncthing(configPath, sharedFolder);
    net2->setExecutable(syncthingPath);
    success = net2->initialiseService();
    QCOMPARE(success, false);
    delete net2;
}

void tst_network::test_Syncthing_ResetNetwork()
{
    // TODO - remove location dependices
    QString syncthingPath = QDir(QDir::homePath()).filePath("syncthing/syncthing");
    std::cout << syncthingPath.toStdString() << std::endl;
    QString configPath = QDir(QDir::homePath()).filePath("liger-test-syncthing");
    std::cout << configPath.toStdString() << std::endl;
    // remove the folder if exists
    if(QDir(configPath).exists()) {
        QDir(configPath).removeRecursively();
    }
    QString sharedFolder = QDir(QDir::homePath()).filePath("liger-test-syncthing/LigerSharedFolder");
    NetworkProtocolSyncthing* net = new NetworkProtocolSyncthing(configPath, sharedFolder);
    net->setExecutable(syncthingPath);
    net->initialiseService();
    QString id0 = net->nodeID();
    std::cout << "0 - node ID: " << id0.toStdString() << std::endl;
    bool success = net->resetNetwork();
    QCOMPARE(success, true);
    QString id1 = net->nodeID();
    QCOMPARE(id1.size(), 63);
    bool res = (id0 != id1);
    std::cout << "1 - node ID: " << id1.toStdString() << std::endl;
    QCOMPARE(res, true);
}

void tst_network::test_Syncthing_StartShutdownService()
{
    // TODO - remove location dependices
    QString syncthingPath = QDir(QDir::homePath()).filePath("syncthing/syncthing");
    std::cout << syncthingPath.toStdString() << std::endl;
    QString configPath = QDir(QDir::homePath()).filePath("liger-test-syncthing");
    QString sharedFolder = QDir(QDir::homePath()).filePath("liger-test-syncthing/LigerSharedFolder");
    NetworkProtocolSyncthing* net = new NetworkProtocolSyncthing(configPath, sharedFolder);
    net->setExecutable(syncthingPath);
    bool success = net->startService();
    QCOMPARE(success, true);
    QCOMPARE(net->isConnected(), true);
    success = net->shutdownService();
    QCOMPARE(success, true);
    QCOMPARE(net->isConnected(), false);
}

void tst_network::test_Syncthing_ConnectToNode()
{
    // TODO - remove location dependices
    QString syncthingPath = QDir(QDir::homePath()).filePath("syncthing/syncthing");
    QString configPath = QDir(QDir::homePath()).filePath("liger-test-syncthing");
    // remove the folder if exists
    if(QDir(configPath).exists()) {
        QDir(configPath).removeRecursively();
    }
    QString sharedFolder = QDir(QDir::homePath()).filePath("liger-test-syncthing/LigerSharedFolder");
    NetworkProtocolSyncthing* net = new NetworkProtocolSyncthing(configPath, sharedFolder);
    net->setExecutable(syncthingPath);
    net->setSharedFolder(sharedFolder);
    net->initialiseService();
    net->startService();
    QStringList threeIDs;
    threeIDs.append("YXMK23I-VZBDZVP-SIPJ6RX-XUOJGEN-FE6VZLK-JFJ7S2K-PP3QQSH-5MOSEQO");
    threeIDs.append("BTJDEXT-BDAE4MV-JQNIJUL-G3C5O62-2RQG7CE-EIPM6VU-HBLC7X6-WMEFHAI");
    threeIDs.append("KEBVUY3-POF7LQH-3ULRD2J-GF5MY2K-6AYP27Q-5LQO5HM-4EH5EZT-JPTQMQQ");
    for(int i = 0; i < threeIDs.size(); i++) {
        net->connectToNode(threeIDs[i]);
    }
    QCOMPARE(net->isConnected(), false);
    QCOMPARE(net->peers(), threeIDs);
}

#ifdef GENERATE_USER_DB
void tst_network::test_GenerateUserDatabase()
{
    using namespace Botan;
    QStringList passwords;
    QStringList hashes;

    passwords << "yiming";
    passwords << "shaul";
    passwords << "peter";
    passwords << "robin";
    passwords << "stephen";
    for(int i=0; i<2; i++) {
        passwords << "test_" + QString::number(i);
    }

    AutoSeeded_RNG rng;
    for(int i = 0; i < passwords.size(); i++) {
        hashes << QString::fromStdString(generate_bcrypt(passwords[i].toStdString(), rng, 12));
        bool OK = check_bcrypt(passwords[i].toStdString(), hashes[i].toStdString());
        cout << passwords[i].toStdString()
             << " |----> " << hashes[i].toStdString()
             << " : " << (OK ? "yes" : "no")
             << endl;
    }

    QString database = QDir::home().filePath("test.sqlite");
    qDebug() << database;
    QFile* dataFile = new QFile(database);
    if (!dataFile->open(QIODevice::WriteOnly)) {
        QCOMPARE(false, true);
    }
    dataFile->close();
    QSqlDatabase db = QSqlDatabase::addDatabase(
                "QSQLITE", QLatin1String(QSqlDatabase::defaultConnection));
    db.setDatabaseName(database);
    if(db.open()) {
        QString create("CREATE TABLE \"authentication\" (\"username\" CHAR UNIQUE , \"hash\" CHAR UNIQUE )");
        QSqlQuery* query = new QSqlQuery(db);
        bool success = query->exec(create);
        if(success) {
            query->clear();
            for(int i = 0; i < passwords.size(); i++) {
                QString queryString("INSERT INTO authentication (\"username\", \"hash\") "
                                    + QString("VALUES ('")
                                    + passwords[i]
                                    + "', '"
                                    + hashes[i]
                                    + "')");
                success = query->exec(queryString);
                if(!success) {
                    qDebug() << query->lastError().text();
                }
            }
        } else {
            qDebug() << query->lastError().text();
        }
        query->clear();
    }
    db.close();
}
#endif

QTEST_APPLESS_MAIN(tst_network)

#include "tst_network.moc"


