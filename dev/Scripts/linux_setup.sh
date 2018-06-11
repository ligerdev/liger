MAJOR_VERSION=5.6
MINOR_VERSION=2
 
VERSION=${MAJOR_VERSION}.${MINOR_VERSION}
INSTALLATION_FILE=qt-opensource-linux-x64-${VERSION}.run
wget http://download.qt.io/official_releases/qt/${MAJOR_VERSION}/${VERSION}/${INSTALLATION_FILE}
chmod +x ${INSTALLATION_FILE}
./${INSTALLATION_FILE}

sudo apt-get install -y git
sudo apt-get install -y python3
sudo apt-get install -y python3-pip
sudo apt-get install -y build-essential
sudo apt-get install -y libfontconfig1
sudo apt-get install -y mesa-common-dev
sudo apt-get install -y libglu1-mesa-dev 
