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
        width: 720
        height: 1280
        source: myImage

        Rectangle {
            id: selectionRect
            color: "transparent"
            border.width: 2
            border.color: "white"
        }
    }

    TreeView {
        anchors.fill: parent
        anchors.leftMargin: 720
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
            console.log(myModel.getRect(currentIndex))
            var rect = myModel.getRect(currentIndex)
            selectionRect.x = rect.x
            selectionRect.y = rect.y
            selectionRect.width = rect.width
            selectionRect.height = rect.height
        }

        TableViewColumn {
            role: "id"
            title: "ObjectId"
            width: 800
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
            role: "enabled"
            title: "Enabled"
            width: 40
        }

        TableViewColumn {
            role: "visible"
            title: "Visible"
            width: 40
        }

        TableViewColumn {
            role: "x"
            title: "X"
            width: 40
        }

        TableViewColumn {
            role: "y"
            title: "Y"
            width: 40
        }

        TableViewColumn {
            role: "z"
            title: "Z"
            width: 40
        }

        TableViewColumn {
            role: "abs_x"
            title: "AbsX"
            width: 40
        }

        TableViewColumn {
            role: "abs_y"
            title: "AbsY"
            width: 40
        }

        TableViewColumn {
            role: "width"
            title: "Width"
            width: 50
        }

        TableViewColumn {
            role: "height"
            title: "Height"
            width: 50
        }
    }
}
