LIBS += ../BlipNBlup/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += \
    ../BlipNBlup/Urho3D/include \
    ../BlipNBlup/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    inputmaster.cpp \
    bnbcam.cpp

HEADERS += \
    mastercontrol.h \
    inputmaster.h \
    bnbcam.h
