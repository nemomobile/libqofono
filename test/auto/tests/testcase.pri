CONFIG += testcase
QT = core testlib dbus
INCLUDEPATH += ../lib ../

equals(QT_MAJOR_VERSION, 4): {
    LIBS += -L../../../src -lqofono
    target.path = $$INSTALL_ROOT$$PREFIX/lib/libqofono/tests
}

equals(QT_MAJOR_VERSION, 5): {
    LIBS += -L../../../src -lqofono-qt5
    target.path = $$INSTALL_ROOT$$PREFIX/lib/libqofono-qt5/tests

}
INSTALLS += target
