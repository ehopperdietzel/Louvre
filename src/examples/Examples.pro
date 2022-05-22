TEMPLATE = subdirs
CONFIG += ordered

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

SUBDIRS = $$PWD/../src/Wayland++.pro \
          $$PWD/MyCompositor/MyCompositor.pro




