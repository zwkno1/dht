TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    consistenthashing.h

QMAKE_CXXFLAGS = -std=c++11

