// SPDX-FileCopyrightText: 2021 - 2022 Petros Koutsolampros
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.settings
import QtQuick.Dialogs

import acanthis 1.0

import "." as Ui

ApplicationWindow {
    id: window

    width: 800
    height: 600

    visible: true

    property GraphModel graphModel: documentManager.createEmptyDocument()

    Settings {
        id: settings
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height

        property color glViewForegroundColour: Qt.rgba(0, 0, 0, 255)
        property color glViewBackgroundColour: Qt.rgba(255, 255, 255, 255)
        property int glViewAntialiasingSamples: 0
        property bool glViewHighlightOnHover: true
    }

    ListModel {
        id: graphDisplayModel

        ListElement {
            graphModelFile: "Untitled"
            current: true
        }
        ListElement {
            graphModelFile: "/qwdqwdqwd/qwdqwdqwd/sdaslrgelrg/aergaerghetyhjtyj/tyjytjtyjuyuj/Untitled 2"
            current: false
        }
    }

    FileDialog {
        id: openDialog
        onAccepted: window.graphModel = documentManager.openDocument(
                        openDialog.fileUrl)
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&New...")
            }
            Action {
                text: qsTr("&Open...")
                onTriggered: {
                    openDialog.open()
                }
            }
            Action {
                text: qsTr("&Save")
            }
            Action {
                text: qsTr("Save &As...")
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Quit")
            }
        }
        Menu {
            title: qsTr("&Edit")
            Action {
                text: qsTr("Cu&t")
            }
            Action {
                text: qsTr("&Copy")
            }
            Action {
                text: qsTr("&Paste")
            }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
            }
        }
    }

    header: ToolBar {
        id: toolbar
        background: Rectangle {
            color: Theme.panelColour
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                id: leftButton
                text: qsTr("‹")
                onClicked: stack.pop()
            }

            ButtonGroup {
                id: btnGrp
            }

            ListView {
                id: graphListNameView
                model: graphDisplayModel

                delegate: Button {
                    id: tabButton
                    anchors.verticalCenter: parent.verticalCenter
                    ButtonGroup.group: btnGrp
                    checkable: true
                    width: graphListNameView.width / graphDisplayModel.count
                    onClicked: {
                        console.log("Activate file " + graphModelFile)
                    }

                    background: Rectangle {
                        color: {
                            if (hovered) {
                                checked ? Theme.activeTabHoverColour : Theme.inactiveTabHoverColour
                            } else {
                                checked ? Theme.activeTabColour : "transparent"
                            }
                        }
                    }
                    contentItem: RowLayout {
                        spacing: 0
                        Text {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            text: graphModelFile
                            elide: Text.ElideLeft
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                        }
                        Button {
                            text: "✕"
                            Layout.alignment: Qt.AlignCenter
                            background: Rectangle {
                                width: 21
                                height: 21
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                radius: parent.width * 0.5
                                color: parent.hovered ? Theme.tabCloseButtonHoverColour : parent.parent.parent.background.color
                            }
                            onClicked: {
                                console.log("Close file " + graphModelFile)
                            }
                        }
                    }
                    // this extends the buttons to the top and bottom. Remove to show their padding
                    anchors.top: parent.top
                }
                Layout.fillWidth: true
                Layout.fillHeight: true
                orientation: ListView.Horizontal
            }

            ToolButton {
                id: rightButton
                text: qsTr("⋮")
                onClicked: drawer.open()
            }
        }
    }

    SplitView {
        id: mainSplitView
        objectName: "mainSplitView"
        anchors.fill: parent
        handle: Item {
            implicitWidth: 4
        }

        Layout.fillWidth: true

        SplitView {
            id: graphViews
            SplitView.fillWidth: true
            focus: true

            GLMapView {
                id: gl_map_view
                graphModel: window.graphModel
                foregroundColour: Qt.rgba(0, 0, 0, 255)
                backgroundColour: Qt.rgba(255, 255, 255, 255)
                antialiasingSamples: 0
                highlightOnHover: true
                //anchors.fill: parent

                // it is necessary to "flip" the FBO here because the default assumes
                // that y is already flipped. Instead this will be handled internally
                // to be in line with depthmapX
                mirrorVertically: true

                SplitView.preferredWidth: window.width / 3
                SplitView.fillWidth: true
                focus: true
            }
        }

        SplitView {
            id: panelSplitView
            objectName: "panelSplitView"
            orientation: Qt.Vertical
            handle: Item {
                implicitHeight: 4
            }

            readonly property int defaultPreferredWidth: 240

            SplitView.minimumWidth: 200
            SplitView.preferredWidth: defaultPreferredWidth
            SplitView.maximumWidth: window.width / 3

            //                Loader {
            //                    objectName: "tilesetSwatchLoader"
            //                    //active: window.projectType === Project.TilesetType && window.canvas
            //                    active: true
            //                    visible: active
            ////                    sourceComponent: Ui.TilesetSwatchPanel {
            ////                        id: tilesetSwatch
            ////                        tileCanvas: window.canvas
            ////                        project: window.graphModel
            ////                        // Don't let e.g. the pencil icon go under us.
            ////                        z: canvasContainer.z - 1
            ////                    }

            //                    SplitView.minimumHeight: active && item.expanded ? item.header.implicitHeight : undefined
            //                    SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
            //                    SplitView.fillHeight: active && item.expanded
            //                }
            Loader {
                objectName: "mapsLoader"
                //active: window.isLayeredImageProjectType && window.canvas
                active: true
                visible: active
                sourceComponent: Ui.MapPanel {
                    //                        layeredImageCanvas: window.canvas
                    graphModel: window.graphModel
                    //z: canvasContainer.z - 1
                }

                SplitView.minimumHeight: active
                                         && item.expanded ? item.minimumUsefulHeight : undefined
                SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
                SplitView.fillHeight: active && item.expanded
            }
        }
    }

    Drawer {
        id: drawer
        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height

        ListView {
            id: listView
            currentIndex: -1
            anchors.fill: parent

            delegate: ItemDelegate {
                width: parent.width
                text: model.graphModelFile
                highlighted: ListView.isCurrentItem
                onClicked: {
                    if (listView.currentIndex !== index) {
                        listView.currentIndex = index
                        titleLabel.text = model.title
                        stackView.replace(model.source)
                    }
                    drawer.close()
                }
            }

            model: graphDisplayModel

            ScrollIndicator.vertical: ScrollIndicator {}
        }
    }
}
