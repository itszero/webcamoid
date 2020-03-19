/* Webcamoid, webcam capture application.
 * Copyright (C) 2016  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import AkQmlControls 1.0

GridLayout {
    columns: 3

    Connections {
        target: AutoBlur

        onRadiusChanged: {
            sldRadius.value = radius
            spbRadius.rvalue = radius
        }
    }

    // Configure blur radius.
    Label {
        id: lblRadius
        text: qsTr("Radius")
    }
    Slider {
        id: sldRadius
        value: AutoBlur.radius
        stepSize: 1
        to: 128
        Layout.fillWidth: true

        onValueChanged: AutoBlur.radius = value
    }
    AkSpinBox {
        id: spbRadius
        rvalue: AutoBlur.radius
        maximumValue: sldRadius.to
        step: sldRadius.stepSize

        onRvalueChanged: AutoBlur.radius = rvalue
    }
    AkButton {
        id: btnCaptureBaseline
        label: qsTr("Capture Baseline")
        onClicked: AutoBlur.handleCaptureBaseline()
    }
}
