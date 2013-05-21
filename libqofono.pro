TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    plugin \
    test

equals(QT_MAJOR_VERSION, 4):{
    SUBDIRS += ofonotest
}


equals(QT_MAJOR_VERSION, 4):  {
    OTHER_FILES += rpm/libqofono.spec \
                   rpm/libqofono.yaml
}

equals(QT_MAJOR_VERSION, 5):  {
    OTHER_FILES += rpm/libqofono-qt5.spec \
                   rpm/libqofono-qt5.yaml \
                    TODO \
                    README
}
