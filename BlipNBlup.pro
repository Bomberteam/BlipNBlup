LIBS += ../BlipNBlup/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH += \
    ../BlipNBlup/Urho3D/include \
    ../BlipNBlup/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    luckey.cpp \
    mastercontrol.cpp \
    inputmaster.cpp \
    bnbcam.cpp \
    fish.cpp \
    controllable.cpp \
    walker.cpp \
    castmaster.cpp \
    bubble.cpp

HEADERS += \
    luckey.h \
    mastercontrol.h \
    inputmaster.h \
    bnbcam.h \
    fish.h \
    controllable.h \
    walker.h \
    castmaster.h \
    bubble.h
