import QtQuick 1.1
import MeeGo.QOfono 0.2


Rectangle {
    width: 480
    height: 560
    Text {
        id: textLine
        anchors.centerIn: parent
    }
    Text {
        id: textLine2
        anchors.top: textLine.bottom
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
          context1.active = !context1.active
        }
    }

    Manager {
        id: manager
    }

    OfonoConnMan {
       id: ofono1
       Component.onCompleted: {
//           active: true //set radio packet active
       //    console.log(manager.modems)
       }
       modemPath: manager.modems[0]
    }

    Modem {
        id: modem1
       modemPath: manager.modems[0]
    }

    ContextConnection {
        id: context1
         contextPath : ofono1.contexts[0]
        Component.onCompleted: {
            textLine.text = context1.active ? "online" : "offline"
      }
        onActiveChanged: {
            textLine.text = context1.active ? "online" : "offline"
        }
    }
    NetworkRegistration {
        modemPath: manager.modems[0]
        id: netreg
        Component.onCompleted: {
            textLine2.text = (modem1.powered ? registration.name : "not powered")
        }
    }
    NetworkOperator {
        id: netop
    }
}
