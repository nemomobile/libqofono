CONFIG += testcase
QT = core testlib dbus
INCLUDEPATH += ../lib ../

equals(QT_MAJOR_VERSION, 4): {
    LIBS += -L../../../src -lqofono
    target.path = $$[QT_INSTALL_PREFIX]/opt/tests/libqofono/
}

equals(QT_MAJOR_VERSION, 5): {
    LIBS += -L../../../src -lqofono-qt5
    target.path = $$[QT_INSTALL_PREFIX]/opt/tests/libqofono-qt5/

}
INSTALLS += target
