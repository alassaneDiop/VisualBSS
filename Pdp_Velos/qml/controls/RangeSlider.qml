import QtQuick 2.5
import QtQuick.Controls 2.1

RangeSlider {
    signal firstValueChanged(real firstValue)
    signal secondValueChanged(real secondValue)
    signal firstPositionChanged(real firstPosition)
    signal secondPositionChanged(real secondPosition)

    id: control
    snapMode: "SnapAlways"
    from: 0
    to: 100
    stepSize: 1
    first.value: from
    second.value: to
    first.onValueChanged: firstValueChanged(first.value)
    second.onValueChanged: secondValueChanged(second.value)
    first.onPositionChanged: firstPositionChanged(to * first.position)
    second.onPositionChanged: secondPositionChanged(to * second.position)

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + (control.availableHeight / 2) - (height / 2)
        implicitWidth: 200
        implicitHeight: 4
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: "#bdbebf"

        Rectangle {
            x: (control.first.visualPosition * parent.width)
            width: (control.second.visualPosition * parent.width) - x
            height: parent.height
            color: "#21be2b"
            radius: 2
        }
    }

    first.handle: Rectangle {
        x: control.leftPadding + first.visualPosition * (control.availableWidth - width)
        y: control.topPadding + (control.availableHeight / 2) - (height / 2)
        implicitWidth: 10
        implicitHeight: 16
        radius: 2
        color: first.pressed ? "#a5a5a5" : "#f6f6f6"
        border.color: "#bdbebf"
    }

    second.handle: Rectangle {
        x: control.leftPadding + second.visualPosition * (control.availableWidth - width)
        y: control.topPadding + (control.availableHeight / 2) - (height / 2)
        implicitWidth: 10
        implicitHeight: 16
        radius: 2
        color: second.pressed ? "#a5a5a5" : "#f6f6f6"
        border.color: "#bdbebf"
    }
}
