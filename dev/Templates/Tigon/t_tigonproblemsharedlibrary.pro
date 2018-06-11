TEMPLATE = lib

QTC_LIB_DEPENDS += tigon

QT      -= gui
CONFIG  -= app_bundle
TARGET  = {% filter lower %}{{ ProblemName }}{% endfilter %}

INCLUDEPATH += {{ path_to_tigon }}

SOURCES += {% filter lower %}{{ ProblemName }}{% endfilter %}.cpp
HEADERS += {% filter lower %}{{ ProblemName }}{% endfilter %}.h
