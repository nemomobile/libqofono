TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src plugin ofonotest \
    test/auto/tst_qofono

# tools tests

#tools.depends = src

#QMAKE_CLEAN += \
#    build-stamp \
#    configure-stamp \
#    artifacts/*.deb \
#    *.log.xml \
#    *.log

#QMAKE_DISTCLEAN += \
#    build-stamp \
#    configure-stamp \
#    *.log.xml \
#    *.log
