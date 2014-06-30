CONFIG += testcase
QT = core testlib dbus
INCLUDEPATH += ../lib ../

equals(QT_MAJOR_VERSION, 4): {
    LIBS += -L../../../src -lqofono
    target.path = $$[QT_INSTALL_LIBS]/libqofono/tests
}

equals(QT_MAJOR_VERSION, 5): {
    LIBS += -L../../../src -lqofono-qt5
    target.path = $$[QT_INSTALL_LIBS]/libqofono-qt5/tests

}
INSTALLS += target
