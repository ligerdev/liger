/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <QString>
#include <iostream>

#include <botan/botan.h>

using namespace std;
using namespace Botan;

void test_BotanExperiments()
{
    //Botan::LibraryInitializer init;

    AutoSeeded_RNG rng;
    SymmetricKey key(rng, 32);
    //    get_pbkdf()
    //OctetString key(PBKDF::derive_key(string("This is my password")),32);

    InitializationVector iv(rng, 16);

    string datastring = string("This is some data - the password is: 554235.");

    Pipe encryptor(get_cipher("AES-256/CBC", key, iv, ENCRYPTION),
                   new Base64_Encoder);

    encryptor.start_msg();
    encryptor.write(datastring);
    encryptor.end_msg(); // flush buffers, complete computations
    string encryptedstring;
    encryptedstring = encryptor.read_all_as_string();
    std::cout << encryptedstring << endl;

    Pipe decryptor(new Base64_Decoder,
                   get_cipher("AES-256/CBC", key, iv, DECRYPTION));
    decryptor.start_msg();
    decryptor.write(encryptedstring);
    decryptor.end_msg();
    string decryptedstring;
    decryptedstring = decryptor.read_all_as_string();
    cout << decryptedstring << endl;
}

void test_passHashing()
{
    QStringList passwords;
    QStringList hashes;

    passwords << "yiming";
    passwords << "shaul";
    passwords << "peter";
    passwords << "robin";
    passwords << "joao";

    passwords << "nvh";
    passwords << "ped";
    passwords << "tase";
    passwords << "ved";
    passwords << "ce";

    AutoSeeded_RNG rng;
    for(int i = 0; i < passwords.size(); i++) {
        hashes << QString::fromStdString(generate_bcrypt(passwords[i].toStdString(), rng, 12));
        bool OK = check_bcrypt(passwords[i].toStdString(), hashes[i].toStdString());
        cout << passwords[i].toStdString()
             << " |----> " << hashes[i].toStdString()
             << " : " << (OK ? "yes" : "no")
             << endl;
    }

}

int main()
{
    cout << "Running Benchmarks..." << endl;
    test_BotanExperiments();
    test_passHashing();
    return 0;
}
