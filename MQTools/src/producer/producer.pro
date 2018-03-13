TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    simpleproducer.cpp \
    simpleconsumer.cpp \
    config.cpp \
    mysqlhelper.cpp

HEADERS += \
    simpleproducer.h \
    simpleconsumer.h \
    config.h \
    mysqlhelper.h
INCLUDEPATH += /usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.3
INCLUDEPATH += /usr/local/apr/include/apr-1
LIBS +=  -lunivalue -lactivemq-cpp
LIBS += -L/usr/lib64/ -lmysqlclient
