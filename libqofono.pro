TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    plugin \
    test/auto/tst_qofono

QT_VERSION=$$[QT_VERSION]
contains(QT_VERSION, "^4.*" ) {
    SUBDIRS += ofonotest
} else {
}

