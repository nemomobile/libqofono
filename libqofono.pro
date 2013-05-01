TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    plugin \
    test/auto/tst_qofono

equals(QT_MAJOR_VERSION, 4):{
    SUBDIRS += ofonotest
}

