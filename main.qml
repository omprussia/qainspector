// Copyright (c) 2020 Open Mobile Platform LLС.
import QtQuick 2.12
import QtQuick.Controls 1.4 as ControlsOld
import QtQuick.Controls.Styles 1.4 as StylesOld
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQml.Models 2.12
import Qt.labs.settings 1.0

import ru.omprussia.qainspector 1.0

ApplicationWindow {
    id: myWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("QA Inspector")

    Settings {
        id: settings
        property string host: hostField.placeholderText
        property string port: portField.placeholderText
        property string application: applicationField.placeholderText
    }

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
                                font.bold: nameArea.pressed
                                MouseArea {
                                    id: nameArea
                                    anchors.fill: parent
                                    onClicked: {
                                        myModel.copyText(modelData.name)
                                    }
                                }
                            }
                            Label {
                                text: ":"
                            }
                            Label {
                                Layout.fillWidth: true
                                text: modelData.value
                                font.bold: valueArea.pressed
                                MouseArea {
                                    id: valueArea
                                    anchors.fill: parent
                                    onClicked: {
                                        myModel.copyText(modelData.value)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    SocketConnection {
        id: socketconnection
        hostname: hostField.text
        port: portField.text
        applicationName: applicationField.text
        onConnectedChanged: {
            if (!socketconnection.connected) {
                if (applicationField.changed) {
                    reconnectTimer.start()
                }
            }
        }
    }

    Timer {
        id: reconnectTimer
        interval: 2000
        repeat: false
        onTriggered: {
            socketconnection.connected = true
            applicationField.changed = false
        }
    }

    Row {
        id: controlsRow
        spacing: 4

        TextField {
            id: hostField
            placeholderText: "192.168.2.15"
            text: settings.host
            onTextChanged: settings.host = text
            selectByMouse: true
            width: 100
        }

        TextField {
            id: portField
            placeholderText: "8888"
            text: settings.port
            onTextChanged: settings.port = text
            selectByMouse: true
            width: 50
        }

        TextField {
            id: applicationField
            placeholderText: "jolla-settings"
            text: settings.application
            selectByMouse: true
            property bool changed: false
            onTextChanged: {
                settings.application = text
                if (socketconnection.connected) {
                    changed = true
                }
            }
            Keys.onReturnPressed: {
                if (!socketconnection.connected || applicationField.changed) {
                    connectButton.clicked()
                }
            }
        }

        Button {
            id: connectButton
            enabled: !reconnectTimer.running
            text: socketconnection.connected ? applicationField.changed ? "Reconnect" : "Disconnect" : "Connect"
            onClicked: socketconnection.connected = !socketconnection.connected
        }

        Button {
            text: "Dump dumb"
            visible: !socketconnection.connected
            onClicked: {
                myModel.loadDumb()
            }
        }

        Button {
            text: "Dump tree"
            visible: socketconnection.connected
            onClicked: {
                socketconnection.getDumpTree(function(dump) {
                    myModel.loadDump(dump)
                })
                socketconnection.getGrabWindow(function(ok) {
                    screenImage.source = ""
                    screenImage.source = myImage
                    screenImage.sourceChanged()
                })
            }
        }

        Button {
            text: "Dump page"
            visible: socketconnection.connected
            onClicked: {
                socketconnection.getDumpPage(function(dump) {
                    myModel.loadDump(dump)
                })
                socketconnection.getGrabWindow(function(ok) {
                    screenImage.source = ""
                    screenImage.source = myImage
                })
            }
        }

        Button {
            text: "Dump cover"
            visible: socketconnection.connected
            onClicked: {
                socketconnection.getDumpCover(function(dump) {
                    myModel.loadDump(dump)
                })
                socketconnection.getGrabCover(function(ok) {
                    screenImage.source = ""
                    screenImage.source = myImage
                })
            }
        }

        Button {
            text: "Expand all"
            visible: myTreeView.__listView.count > 0
            onClicked: {
                myTreeView.expandAll()
            }
        }
    }

    Item {
        id: searchRow
        anchors.bottom: parent.bottom
        width: parent.width
        height: socketconnection.connected ? searchButton.height : 0

        TextField {
            id: searchField
            placeholderText: ""
            text: placeholderText
            selectByMouse: true
            anchors.left: parent.left
            anchors.right: searchSwitchRow.left

            Keys.onReturnPressed: {
                searchButton.clicked()
            }
        }

        Row {
            id: searchSwitchRow
            anchors.verticalCenter: searchButton.verticalCenter
            anchors.right: searchButton.left

            Switch {
                id: partialSwitch
                text: "Partial"
                checked: true
                enabled: !xpathRadio.checked
            }

            RadioButton {
                id: classNameRadio
                text: "ClassName"
                onCheckedChanged: {
                    if (checked) {
                        searchButton.searchProperty = "classname"
                    }
                }
            }

            RadioButton {
                id: textRadio
                text: "Text"
                checked: true
                onCheckedChanged: {
                    if (checked) {
                        searchButton.searchProperty = "mainTextProperty"
                    }
                }
            }

            RadioButton {
                id: valueRadio
                text: "ObjectName"
                onCheckedChanged: {
                    if (checked) {
                        searchButton.searchProperty = "objectName"
                    }
                }
            }

            RadioButton {
                id: xpathRadio
                text: "XPath"
                onCheckedChanged: {
                    if (checked) {
                        searchButton.searchProperty = "xpath"
                    }
                }
            }
        }

        Button {
            id: searchButton
            text: "Search"
            visible: socketconnection.connected
            anchors.right: parent.right
            property string searchProperty: "mainTextProperty"
            onClicked: {
                if (searchField.text == "") {
                    searchField.forceActiveFocus()
                    return
                }

                var idx = myModel.searchIndex(searchProperty, searchField.text, partialSwitch.checked, myTreeView.selection.currentIndex)
                myTreeView.selectIndex(idx)
            }
        }
    }

    Row {
        anchors.top: screenBackground.bottom

        Button {
            text: "Refresh"
            visible: socketconnection.connected
            onClicked: {
                socketconnection.getGrabWindow(function(ok) {
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

        MouseArea {
            anchors.fill: parent
            onClicked: {
                pointRect.posx = mouse.x
                pointRect.posy = mouse.y
                pointRect.visible = true

//                socketconnection.findObject(mouse.x / screenBackground.factor, mouse.y / screenBackground.factor)
                var idx = myModel.searchByCoordinates(mouse.x / screenBackground.factor, mouse.y / screenBackground.factor)
                myTreeView.selectIndex(idx)
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
        anchors.bottomMargin: searchRow.height
        model: myModel

        property bool searchIndex: false
        property int selectedIndex: -1
        onSelectedIndexChanged: {
            if (searchIndex >= 0) {
                searchIndex = false
                searchAnimation.stop()
                __listView.positionViewAtIndex(selectedIndex, ListView.Contain)
            }
        }

        function selectIndex(idx) {
            myTreeView.forceActiveFocus()
            myTreeView.expandAll()
            myTreeView.selectedIndex = -1
            myTreeView.searchIndex = true
            myTreeView.selection.clearCurrentIndex()
            myTreeView.__listView.positionViewAtBeginning()
            myTreeView.selection.setCurrentIndex(idx, ItemSelectionModel.ClearAndSelect)
            if (myTreeView.searchIndex) {
                searchAnimation.start()
            }
            screenBackground.selection = myModel.getRect(idx)
        }

        NumberAnimation {
            id: searchAnimation
            target: myTreeView.__listView
            property: "contentY"
            from: 0
            to: Math.max(0, myTreeView.__listView.contentHeight - myTreeView.__listView.height - 24)
            duration: myTreeView.__listView.contentHeight / 5
            onStopped: {
                if (myTreeView.selectedIndex == -1) {
                    myTreeView.__listView.positionViewAtBeginning()
                }
            }
        }

        function expandAll() {
            var someIndexes = myModel.getChildrenIndexes();
            for (var i = 0; i < someIndexes.length; i++) {
                myTreeView.expand(someIndexes[i]);
            }
        }

        selection: ItemSelectionModel {
            model: myModel
        }

        style: StylesOld.TreeViewStyle {
            activateItemOnSingleClick: true
            backgroundColor: "#20aaaaaa"
            alternateBackgroundColor: "#40aaaaaa"
            textColor: "#000000"
        }


        __itemDelegateLoader: DelegateLoader {
            __style: myTreeView.__style
            __itemDelegate: myTreeView.itemDelegate
            __mouseArea: myTreeView.__mouseArea
            __treeModel: myTreeView.__model
        }

        itemDelegate: Component {
            Item {
                id: itemDelegate
                property bool itemSelected: styleData.selected
                onItemSelectedChanged: {
                    if (myTreeView.searchIndex && itemSelected && styleData.column == 0 && styleData.row >= 0) {
                        myTreeView.selectedIndex = styleData.row
                    }
                }

                property int rowIndex: styleData.row
                onRowIndexChanged: {
                    if (myTreeView.searchIndex && styleData.selected && styleData.column == 0) {
                        myTreeView.selectedIndex = styleData.row
                    }
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    color: styleData.textColor
                    elide: styleData.elideMode
                    text: styleData.value
                }
            }
        }

        headerDelegate: Component {
            Rectangle {
                id: headerDelegate
                height: 24
                border.width: 1
                border.color: "lightgray"
                color: "white"
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: styleData.value
                    x: 4
                }
            }
        }

        onCurrentIndexChanged: {
            screenBackground.selection = myModel.getRect(currentIndex)
        }

        onPressAndHold: {
//            myTreeView.expand(index)
            var dataList = myModel.getDataList(index)
            popupComponent.createObject(myWindow.contentItem,
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
            role: "objectName"
            title: "ObjectName"
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

    Rectangle {
        id: pointRect
        color: "transparent"
        border.width: 2
        border.color: "white"
        visible: false
        property int posx: 0
        property int posy: 0
        width: pointText.paintedWidth + 8
        height: pointText.paintedHeight + 8
        x: screenBackground.x + posx
        y: screenBackground.y + posy

        Rectangle {
            anchors.verticalCenter: parent.top
            anchors.horizontalCenter: parent.left
            color: "red"
            width: 4
            height: 4
            radius: 2
        }

        Rectangle {
            anchors.fill: pointText
            anchors.margins: -4
            color: "#80000000"
        }

        Text {
            id: pointText
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 4
            text: "%1:%2".arg(pointRect.posx / screenBackground.factor).arg(pointRect.posy / screenBackground.factor)
            color: "white"
            font.pixelSize: 22 * screenBackground.factor
        }
    }
}
