# Support python-2.7 (64-bit on windows) for now.
#DISABLE_PYTHON=yes
!contains(DISABLE_PYTHON, yes) {
    QT_CONFIG -= no-pkg-config
    TMP=$$(BOOST_PYTHON_LIB)
    isEmpty(TMP) {
        error("Boost.Python not found.")
    }
    unix {
        QMAKE_RPATHDIR += $$(BOOST_PYTHON_LIB)
        packagesExist(python-2.7) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API
            CONFIG += link_pkgconfig
            PKGCONFIG += python-2.7
        } else:system(python-config --prefix) {
            # currently here for reference, only 2.7 is tested
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API
            QMAKE_CXXFLAGS += `python-config --cflags`
            QMAKE_LFLAGS += `python-config --ldflags`
        }
    }

    win32 {
        exists(C:/Python27/include/Python.h) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API
            QMAKE_CXXFLAGS += -IC:/Python27/include/
            QMAKE_LIBS += -LC:/Python27/libs -lpython27
        }
    }
}
