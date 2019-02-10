#DISABLE_PYTHON=yes
!contains(DISABLE_PYTHON, yes) {
    QT_CONFIG -= no-pkg-config
    HAVE_PYTHON = no
    unix {
        system(python-config --prefix) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API
            PYTHON_INCLUES = $$system(python-config --includes)
            PYTHON_LIB = $$system(python-config --ldflags)
            QMAKE_CXXFLAGS += $${PYTHON_INCLUES}
            QMAKE_LFLAGS += $${PYTHON_LIB}

            message("Include Python.h:  $${PYTHON_INCLUES}")
            message("Link to python lib:  $${PYTHON_LIB}")
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

    contains(HAVE_PYTHON, no) {
         error("Cannot find python library")
    }
}
