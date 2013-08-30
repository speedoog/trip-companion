#-------------------------------------------------
#
# Project created by QtCreator 2013-08-21T11:18:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TripCompanion
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    exiv/xmpsidecar.cpp \
    exiv/xmp.cpp \
    exiv/version.cpp \
    exiv/value.cpp \
    exiv/utils.cpp \
    exiv/types.cpp \
    exiv/tiffvisitor.cpp \
    exiv/tiffimage.cpp \
    exiv/tiffcomposite.cpp \
    exiv/tgaimage.cpp \
    exiv/tags.cpp \
    exiv/sonymn.cpp \
    exiv/sigmamn.cpp \
    exiv/samsungmn.cpp \
    exiv/rw2image.cpp \
    exiv/rafimage.cpp \
    exiv/psdimage.cpp \
    exiv/properties.cpp \
    exiv/preview.cpp \
    exiv/pngimage.cpp \
    exiv/pngchunk.cpp \
    exiv/pgfimage.cpp \
    exiv/pentaxmn.cpp \
    exiv/panasonicmn.cpp \
    exiv/orfimage.cpp \
    exiv/olympusmn.cpp \
    exiv/nikonmn.cpp \
    exiv/mrwimage.cpp \
    exiv/minoltamn.cpp \
    exiv/metadatum.cpp \
    exiv/makernote.cpp \
    exiv/localtime.c \
    exiv/jpgimage.cpp \
    exiv/jp2image.cpp \
    exiv/iptc.cpp \
    exiv/image.cpp \
    exiv/gifimage.cpp \
    exiv/futils.cpp \
    exiv/fujimn.cpp \
    exiv/exif.cpp \
    exiv/error.cpp \
    exiv/epsimage.cpp \
    exiv/easyaccess.cpp \
    exiv/datasets.cpp \
    exiv/crwimage.cpp \
    exiv/cr2image.cpp \
    exiv/convert.cpp \
    exiv/canonmn.cpp \
    exiv/bmpimage.cpp \
    exiv/basicio.cpp \
    TcPhoto.cpp \
    GpsFrame.cpp \
    GpsLog.cpp \
    TcLib.cpp \
    TcSettings.cpp \
    GpsDateTime.cpp

HEADERS  += \
    exiv/xmpsidecar.hpp \
    exiv/xmp.hpp \
    exiv/version.hpp \
    exiv/value.hpp \
    exiv/utils.hpp \
    exiv/tzfile.h \
    exiv/types.hpp \
    exiv/timegm.h \
    exiv/tiffvisitor_int.hpp \
    exiv/tiffimage_int.hpp \
    exiv/tiffimage.hpp \
    exiv/tifffwd_int.hpp \
    exiv/tiffcomposite_int.hpp \
    exiv/tgaimage.hpp \
    exiv/tags_int.hpp \
    exiv/tags.hpp \
    exiv/sonymn_int.hpp \
    exiv/sigmamn_int.hpp \
    exiv/samsungmn_int.hpp \
    exiv/rw2image_int.hpp \
    exiv/rw2image.hpp \
    exiv/rcsid_int.hpp \
    exiv/rafimage.hpp \
    exiv/psdimage.hpp \
    exiv/properties.hpp \
    exiv/private.h \
    exiv/preview.hpp \
    exiv/pngimage.hpp \
    exiv/pngchunk_int.hpp \
    exiv/pgfimage.hpp \
    exiv/pentaxmn_int.hpp \
    exiv/panasonicmn_int.hpp \
    exiv/orfimage_int.hpp \
    exiv/orfimage.hpp \
    exiv/olympusmn_int.hpp \
    exiv/nikonmn_int.hpp \
    exiv/mrwimage.hpp \
    exiv/minoltamn_int.hpp \
    exiv/metadatum.hpp \
    exiv/metacopy.hpp \
    exiv/makernote_int.hpp \
    exiv/jpgimage.hpp \
    exiv/jp2image.hpp \
    exiv/iptc.hpp \
    exiv/image.hpp \
    exiv/i18n.h \
    exiv/gifimage.hpp \
    exiv/getopt_win32.h \
    exiv/futils.hpp \
    exiv/fujimn_int.hpp \
    exiv/exv_conf.h \
    exiv/exiv2app.hpp \
    exiv/exiv2.hpp \
    exiv/exif.hpp \
    exiv/error.hpp \
    exiv/epsimage.hpp \
    exiv/easyaccess.hpp \
    exiv/datasets.hpp \
    exiv/crwimage_int.hpp \
    exiv/crwimage.hpp \
    exiv/cr2image_int.hpp \
    exiv/cr2image.hpp \
    exiv/convert.hpp \
    exiv/canonmn_int.hpp \
    exiv/bmpimage.hpp \
    exiv/basicio.hpp \
    exiv/actions.hpp \
    TcPhoto.h \
    GpsFrame.h \
    GpsLog.h \
    mainwindow.h \
    TcLib.h \
    TcSettings.h \
    GpsDateTime.h

FORMS    += mainwindow.ui

CONFIG += mobility
MOBILITY = 

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/version.xml \
    TripCompanion_icon.png

