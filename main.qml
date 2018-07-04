import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

import ru.omprussia.qainspector 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    RemoteConnection {
        id: connection
        hostname: hostField.text
        port: portField.text
        applicationName: applicationField.text
    }

    Row {
        id: controlsRow
        spacing: 4

        TextField {
            id: hostField
            placeholderText: "192.168.2.15"
            text: placeholderText
        }

        TextField {
            id: portField
            placeholderText: "55556"
            text: placeholderText
        }

        TextField {
            id: applicationField
            placeholderText: "jolla-settings"
            text: placeholderText
        }

        Button {
            text: connection.connected ? "Disconnect" : "Connect"
            onClicked: connection.connected = !connection.connected
        }

        Button {
            text: "Dump tree"
            visible: connection.connected
            onClicked: {
                connection.getDumpTree(function(dump) {
                    myModel.loadDump(dump)
                })
                connection.getGrabWindow(function(ok) {
                    screenImage.source = ""
                    screenImage.source = myImage
                })
            }
        }

        Button {
            text: "Dump page"
            visible: connection.connected
            onClicked: {
                connection.getDumpPage(function(dump) {
                    myModel.loadDump(dump)
                })
                connection.getGrabWindow(function(ok) {
                    screenImage.source = ""
                    screenImage.source = myImage
                })
            }
        }
    }

    Rectangle {
        id: screenBackground
        y: controlsRow.height
        width: screenImage.sourceSize.width * factor
        height: screenImage.sourceSize.height * factor
        property real factor: 0.5
        property rect selection
        color: "black"

        Image {
            id: screenImage
            anchors.fill: parent
            source: myImage
        }

        Rectangle {
            id: selectionRect
            color: "transparent"
            border.width: 2
            border.color: "white"
            width: screenBackground.selection.width * screenBackground.factor
            height: screenBackground.selection.height * screenBackground.factor
            x: screenBackground.selection.x * screenBackground.factor
            y: screenBackground.selection.y * screenBackground.factor

            Rectangle {
                anchors.fill: sizeLabel
                anchors.margins: -4
                color: "#80000000"
            }

            Text {
                id: sizeLabel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 8 * screenBackground.factor
                text: "%1:%2 %3:%4".arg(selectionRect.x).arg(selectionRect.y)
                                   .arg(selectionRect.width).arg(selectionRect.height)
                color: "white"
                font.pixelSize: 16 * screenBackground.factor
            }
        }
    }

    ItemsTreeModes {
        id: myModel
    }

    TreeView {
        id: myTreeView
        anchors.fill: parent
        anchors.topMargin: controlsRow.height
        anchors.leftMargin: screenBackground.width
        model: myModel

        alternatingRowColors: false
        itemDelegate: Item {
            Text {
                anchors.verticalCenter: parent.verticalCenter
//                color: styleData.textColor
//                elide: styleData.elideMode
                text: styleData.value
            }
        }
        headerDelegate: Rectangle {
            height: 20
            color: "gray"
            border.width: 1
            border.color: "#40ffffff"
            Text {
                color: "white"
                text: styleData.value
            }
        }

        onCurrentIndexChanged: {
            screenBackground.selection = myModel.getRect(currentIndex)
        }

        onClicked: {
            myTreeView.expand(index)
        }

        onDoubleClicked: {
            myModel.copyToClipboard(index)
        }

        TableViewColumn {
            role: "classname"
            title: "Classname"
            width: 400
        }

        TableViewColumn {
            role: "name"
            title: "Name"
            width: 100
        }

        TableViewColumn {
            role: "text"
            title: "Text"
            width: 100
        }

        TableViewColumn {
            role: "title"
            title: "Title"
            width: 100
        }

        TableViewColumn {
            role: "label"
            title: "Label"
            width: 100
        }

        TableViewColumn {
            role: "placeholderText"
            title: "Placeholder"
            width: 100
        }

        TableViewColumn {
            role: "enabled"
            title: "ena"
            width: 30
        }

        TableViewColumn {
            role: "visible"
            title: "vis"
            width: 30
        }

        TableViewColumn {
            role: "abs_x"
            title: "abx"
            width: 40
        }

        TableViewColumn {
            role: "abs_y"
            title: "aby"
            width: 40
        }

        TableViewColumn {
            role: "width"
            title: "wid"
            width: 50
        }

        TableViewColumn {
            role: "height"
            title: "hei"
            width: 50
        }

        TableViewColumn {
            role: "x"
            title: "x"
            width: 40
        }

        TableViewColumn {
            role: "y"
            title: "y"
            width: 40
        }

        TableViewColumn {
            role: "z"
            title: "z"
            width: 40
        }
    }
}
