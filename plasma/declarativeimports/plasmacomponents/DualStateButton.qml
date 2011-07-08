/*
*   Copyright (C) 2011 by Daker Fernandes Pinheiro <dakerfp@gmail.com>
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU Library General Public License as
*   published by the Free Software Foundation; either version 2, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details
*
*   You should have received a copy of the GNU Library General Public
*   License along with this program; if not, write to the
*   Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

import QtQuick 1.0
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: dualButton

    // Common API
    property bool checked
    property alias pressed: mouseArea.pressed

    signal clicked()

    // Plasma API
    property alias text: label.text // TODO: Not yet part of the common API
    property QtObject theme: PlasmaCore.Theme { }
    property alias view: surface.sourceComponent

    width: surface.width + label.paintedWidth
    height: surface.height

    function entered() {
        shadow.opacity = 0;
        hover.opacity = 1;
    }

    function released() {
        dualButton.checked = !dualButton.checked;
        dualButton.clicked();
    }

    Keys.onSpacePressed: entered();
    Keys.onReturnPressed: entered();
    Keys.onReleased: {
        if(event.key == Qt.Key_Space ||
           event.key == Qt.Key_Return)
            released();
    }

    PlasmaCore.FrameSvgItem {
        id: hover

        anchors {
            fill: surface
            leftMargin: -margins.left
            topMargin: -margins.top
            rightMargin: -margins.right
            bottomMargin: -margins.bottom
        }
        opacity: 0
        imagePath: "widgets/button"
        prefix: "hover"

        Behavior on opacity {
            PropertyAnimation { duration: 250 }
        }
    }

    PlasmaCore.FrameSvgItem {
        id: shadow

        anchors {
            fill: surface
            leftMargin: -margins.left
            topMargin: -margins.top
            rightMargin: -margins.right
            bottomMargin: -margins.bottom
        }
        imagePath: "widgets/button"
        prefix: "shadow"

        Behavior on opacity {
            PropertyAnimation { duration: 250 }
        }
    }

    Loader {
        id: surface

        anchors {
            verticalCenter: parent.verticalCenter
            left: text ? parent.left : undefined
            horizontalCenter: text ? undefined : parent.horizontalCenter
        }
    }

    Text {
        id: label

        text: dualButton.text
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: surface.right
            right: parent.right
            // XXX: see how this margin will be set
            leftMargin: hover.margins.right
        }
        color: theme.textColor
        verticalAlignment: Text.AlignVCenter
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true

        onReleased: dualButton.released();
        onEntered: dualButton.entered();
        onPressed: dualButton.forceActiveFocus();
        onExited: {
            shadow.opacity = 1;
            hover.opacity = 0;
        }
    }
}
