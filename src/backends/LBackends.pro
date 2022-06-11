TEMPLATE = subdirs

CONFIG -= qt
CONFIG -= app_bundle
CONFIG += ordered
CONFIG += c++17

SUBDIRS = DRM/LBackendDRM.pro \
          X11/LBackendX11.pro \


