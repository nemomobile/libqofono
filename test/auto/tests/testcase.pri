CONFIG += testcase
QT = core testlib dbus
QOFONO_DIR = ../../../src/
INCLUDEPATH += ../lib ../ $$QOFONO_DIR

equals(QT_MAJOR_VERSION, 4): {
    LIBS += -L$$QOFONO_DIR -lqofono
    target.path = $$[QT_INSTALL_LIBS]/libqofono/tests
}

equals(QT_MAJOR_VERSION, 5): {
    LIBS += -L$$QOFONO_DIR -lqofono-qt5
    target.path = $$[QT_INSTALL_LIBS]/libqofono-qt5/tests

}
INSTALLS += target
