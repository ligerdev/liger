CONFIG  += exceptions
QT      += core network concurrent
DEFINES += EMAILNOTIFIER_LIBRARY

DEFINES += SMTP_BUILD

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(../../ligerplugin.pri)

linux-* {
    # Linux specific commands here.
}

macx {
    # Mac specific commands here.
}

win32 {
    # Windows 32-bit specific commands here.
    CONFIG += dll
}

# Source files
SOURCES += emailnotifier.cpp \
    smtpemail/emailaddress.cpp \
    smtpemail/mimeattachment.cpp \
    smtpemail/mimecontentformatter.cpp \
    smtpemail/mimefile.cpp \
    smtpemail/mimehtml.cpp \
    smtpemail/mimeinlinefile.cpp \
    smtpemail/mimemessage.cpp \
    smtpemail/mimemultipart.cpp \
    smtpemail/mimepart.cpp \
    smtpemail/mimetext.cpp \
    smtpemail/quotedprintable.cpp \
    smtpemail/smtpclient.cpp \
    emailnotifierconfigdialog.cpp

HEADERS += emailnotifier.h \
    emailnotifier_global.h \
    emailnotifierconstants.h \
    smtpemail/emailaddress.h \
    smtpemail/mimeattachment.h \
    smtpemail/mimecontentformatter.h \
    smtpemail/mimefile.h \
    smtpemail/mimehtml.h \
    smtpemail/mimeinlinefile.h \
    smtpemail/mimemessage.h \
    smtpemail/mimemultipart.h \
    smtpemail/mimepart.h \
    smtpemail/mimetext.h \
    smtpemail/quotedprintable.h \
    smtpemail/smtpclient.h \
    smtpemail/smtpexports.h \
    smtpemail/SmtpMime \
    emailnotifierconfigdialog.h \
    emailnotifierconstants.h

OTHER_FILES += \
    EmailNotifier.pluginspec.in \
    smtpemail/LICENSE \
    smtpemail/README.md

RESOURCES +=

FORMS += \
    emailnotifierconfigdialog.ui
