TEMPLATE = subdirs

CONFIG -= qt
CONFIG -= app_bundle
CONFIG += ordered
CONFIG += c++17

SUBDIRS = lib/Louvre.pro \
          backends/LBackends.pro\
          examples/Examples.pro

