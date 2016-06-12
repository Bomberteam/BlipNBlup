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
    bubble.cpp \
    wind.cpp \
    flyer.cpp \
    wasp.cpp \
    resourcemaster.cpp \
    effectmaster.cpp \
    catchable.cpp \
    container.cpp \
    rage.cpp

HEADERS += \
    luckey.h \
    mastercontrol.h \
    inputmaster.h \
    bnbcam.h \
    fish.h \
    controllable.h \
    walker.h \
    castmaster.h \
    bubble.h \
    wind.h \
    flyer.h \
    wasp.h \
    resourcemaster.h \
    effectmaster.h \
    catchable.h \
    gameevents.h \
    container.h \
    rage.h
