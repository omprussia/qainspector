import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Image {
        id: screenImage
        source: myImage
        width: sourceSize.width * factor
        height: sourceSize.height * factor
        property real factor: 0.5
        property rect selection

        Rectangle {
            id: selectionRect
            color: "transparent"
            border.width: 2
            border.color: "white"
            width: screenImage.selection.width * screenImage.factor
            height: screenImage.selection.height * screenImage.factor
            x: screenImage.selection.x * screenImage.factor
            y: screenImage.selection.y * screenImage.factor

            Rectangle {
                anchors.fill: sizeLabel
                anchors.margins: -4
                color: "#80000000"
            }

            Text {
                id: sizeLabel
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 8 * screenImage.factor
                text: "%1:%2 %3:%4".arg(selectionRect.x).arg(selectionRect.y)
                                   .arg(selectionRect.width).arg(selectionRect.height)
                color: "white"
                font.pixelSize: 16 * screenImage.factor
            }
        }
    }

    TreeView {
        id: myTreeView
        anchors.fill: parent
        anchors.leftMargin: screenImage.width
        model: myModel
        itemDelegate: Item {
            Text {
                anchors.verticalCenter: parent.verticalCenter
                color: styleData.textColor
                elide: styleData.elideMode
                text: styleData.value
            }
        }

        onCurrentIndexChanged: {
            screenImage.selection = myModel.getRect(currentIndex)
        }

        onClicked: {
            myTreeView.expand(index)
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
