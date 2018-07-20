import QtQuick 2.7
import QtQuick.Controls 1.4 as ControlsOld
import QtQuick.Controls.Styles 1.4 as StylesOld
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import ru.omprussia.qainspector 1.0

ApplicationWindow {
    id: myWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Component {
        id: popupComponent
        Popup {
            id: popup
            modal: true
            focus: true
            width: popupList.width + 25
            height: popupList.height + 25
            clip: true

            property alias model: popupRepeater.model

            Component.onCompleted: open()
            onClosed: popup.destroy()

            Flickable {
                id: popupList
                width: popupColumn.width
                height: 600
                contentHeight: popupColumn.height
                Column {
                    id: popupColumn
                    Repeater {
                        id: popupRepeater
                        delegate: Row {
                            height: 20
                            spacing: 4
                            onWidthChanged: if (popupColumn.width < width) popupColumn.width = width
                            Label {
                                text: modelData.name
                            }
                            Label {
                                text: ":"
                            }
                            Label {
                                Layout.fillWidth: true
                                text: modelData.value
                            }
                        }
                    }
                }
            }
        }
    }

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
            selectByMouse: true
        }

        TextField {
            id: portField
            placeholderText: "55556"
            text: placeholderText
            selectByMouse: true
        }

        TextField {
            id: applicationField
            placeholderText: "jolla-settings"
            text: placeholderText
            selectByMouse: true
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

    ControlsOld.TreeView {
        id: myTreeView
        anchors.fill: parent
        anchors.topMargin: controlsRow.height
        anchors.leftMargin: screenBackground.width
        model: myModel

        style: StylesOld.TreeViewStyle {
            activateItemOnSingleClick: true
            backgroundColor: "#20aaaaaa"
            alternateBackgroundColor: "#40aaaaaa"
            textColor: "#000000"
        }

        itemDelegate: Item {
            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
            }
        }

        onCurrentIndexChanged: {
            screenBackground.selection = myModel.getRect(currentIndex)
        }

        onPressAndHold: {
//            myTreeView.expand(index)
            var dataList = myModel.getDataList(index)
            popupComponent.createObject(myWindow,
                                        {
                                            model: dataList,
                                            x: myTreeView.x + 16,
                                            y: myTreeView.y + 16
                                        })
        }

        onDoubleClicked: {
            myModel.copyToClipboard(index)
        }

        ControlsOld.TableViewColumn {
            role: "classname"
            title: "Classname"
            width: 400
        }

        ControlsOld.TableViewColumn {
            role: "abs_x"
            title: "abx"
            width: 40
        }

        ControlsOld.TableViewColumn {
            role: "abs_y"
            title: "aby"
            width: 40
        }

        ControlsOld.TableViewColumn {
            role: "mainTextProperty"
            title: "Text"
            width: 100
        }

        ControlsOld.TableViewColumn {
            role: "enabled"
            title: "ena"
            width: 30
        }

        ControlsOld.TableViewColumn {
            role: "visible"
            title: "vis"
            width: 30
        }

        ControlsOld.TableViewColumn {
            role: "width"
            title: "wid"
            width: 50
        }

        ControlsOld.TableViewColumn {
            role: "height"
            title: "hei"
            width: 50
        }
    }
}
