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
        text: manager.available ? netreg.name : "Ofono not available"
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
          context1.active = !context1.active
        }
    }

    OfonoManager {
        id: manager
        onAvailableChanged: {
            console.log("Ofono is " + available)
           textLine2.text = manager.available ? netreg.name :"Ofono not available"
        }
        onModemAdded: {
            console.log("modem added "+modem)
        }
        onModemRemoved: console.log("modem removed")
    }

    OfonoConnMan {
       id: ofono1
       Component.onCompleted: {
       //    console.log(manager.modems)
       }
       modemPath: manager.modems[0]
    }

    OfonoModem {
        id: modem1
       modemPath: manager.modems[0]

    }

    OfonoContextConnection {
        id: context1
        contextPath : ofono1.contexts[0]
        Component.onCompleted: {
            textLine.text = context1.active ? "online" : "offline"
      }
        onActiveChanged: {
            textLine.text = context1.active ? "online" : "offline"
        }
    }
    OfonoNetworkRegistration {
        modemPath: manager.modems[0]
        id: netreg
        Component.onCompleted: {
            netreg.scan()
        }
      onNetworkOperatorsChanged : {
          console.log("operators :"+netreg.currentOperator["Name"].toString())

        }
    }
    OfonoNetworkOperator {
        id: netop
    }
}
