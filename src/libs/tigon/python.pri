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
        packagesExist(python-3.10) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_3_10
            CONFIG += link_pkgconfig
            PKGCONFIG += python-3.10
            LIBS += `python-config --libs --embed`
        } else:packagesExist(python-3.9) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_3_9
            CONFIG += link_pkgconfig
            PKGCONFIG += python-3.9
            LIBS += `python-config --libs --embed`
        } else:packagesExist(python-3.8) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_3_8
            CONFIG += link_pkgconfig
            PKGCONFIG += python-3.8
            LIBS += `python-config --libs --embed`
        } else:packagesExist(python-3.7) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_3_7 REQUIRES_ATOMIC
            CONFIG += link_pkgconfig
            PKGCONFIG += python-3.7
        } else:packagesExist(python-3.6) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_3_6
            CONFIG += link_pkgconfig
            PKGCONFIG += python-3.6
        } else:packagesExist(python-2.7) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_2_7
            CONFIG += link_pkgconfig
            PKGCONFIG += python-2.7
        } else:system(python-config --prefix) {
            # currently here for reference, tested only up-to 3.10
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API PYTHON_LINUX PYTHON_2_7
            QMAKE_CXXFLAGS += `python-config --cflags`
            # The following line doesn't work because the libraries are placed
            # before the objects when the linker is called
            # QMAKE_LFLAGS += `python-config --ldflags`
            #
            # Load the libraries instead as follows
            LIBS += `python-config --libs` # for python <= 3.7
            # LIBS += `python-config --libs --embed` # uncomment for python > 3.7
        }
    }

    win32 {
        exists(C:/Python310/include/Python.h) {
            HAVE_PYTHON = yes
            DEFINES += PYTHON_API
            QMAKE_CXXFLAGS += -IC:/Python310/include/
            QMAKE_LIBS += -LC:/Python310/libs -lpython3
        }
    }
}
