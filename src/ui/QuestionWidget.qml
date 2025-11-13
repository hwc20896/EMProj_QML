import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes

ColumnLayout{
    id: questionRoot

    //  Examples
    property string title: ""
    property var optionText: []
    property string descriptionText: ""

    signal popOption(string text)

    spacing: 30

    Text{
        id: questionTitle
        Layout.topMargin: gamePageRoot.height * 0.01
        Layout.leftMargin: gamePageRoot.width * 0.05
        Layout.rightMargin: gamePageRoot.width * 0.05

        text: `${backend.currentQuestionIndex + 1}. ${title}`
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Layout.fillWidth: true

        font.pointSize: 25
        wrapMode: Text.WordWrap
    }

    GridLayout {
        id: grid
        columns: 2
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 30
        columnSpacing: 20
        rowSpacing: 20

        Repeater {
            id: optionButtons
            model: optionText

            delegate: Button {
                id: btn
                property string optionText: modelData
                property bool isWrong: false

                text: modelData
                enabled: !isWrong && !answerRevealed
                Layout.preferredWidth: 300
                Layout.preferredHeight: 120
                font.pointSize: 16

                background: Rectangle {
                    radius: 8
                    border.width: 2
                    border.color: {
                        if (!answerRevealed) return "#9E9E9E";
                        if (btn.text === correctOption) return "#2E7D32";
                        if (btn.text === selectedOption) return "#B71C1C";
                        return "#9E9E9E";
                    }

                    // 核心：动态背景色
                    color: {
                        if (!answerRevealed) {
                            return parent.pressed ? "#E0E0E0" : "#FFFFFF";
                        }

                        // 答案揭示状态
                        if (text === correctOption) {
                            return "#4CAF50"  // 正确答案绿色
                        }
                        if (text === selectedOption) {
                            return "#F44336"  // 用户选择的错误答案红色
                        }
                        return "#F5F5F5" // 未选中的选项
                    }
                }

                onClicked: {
                    if (isInReviveMode) {
                        handleReviveAnswer(text);
                    } else {
                        selectedOption = text;
                        backend.handleAnswer(text);
                    }
                }

                // 劃線效果

                Shape {
                    id: strike
                    anchors.fill: parent
                    visible: btn.isWrong
                    opacity: 0.0
                    property real margin: 6


                    ShapePath {
                        strokeColor: "red"
                        strokeWidth: 2
                        capStyle: ShapePath.RoundCap
                        startX: strike.margin
                        startY: strike.margin
                        PathLine { x: strike.width - strike.margin; y: strike.height - strike.margin }
                    }


                    // 線條動畫
                    NumberAnimation on opacity {
                        id: strikeFade
                        from: 0.0
                        to: 1.0
                        duration: 200
                        easing.type: Easing.InOutQuad
                        running: false

                        onStopped: {
                            if (btn.isWrong) fadeOut.start();
                        }
                    }
                }

                PropertyAnimation {
                    id: fadeOut
                    target: btn
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 400
                    easing.type: Easing.InOutQuad
                    onStopped: btn.enabled = false; // ✅ 安全關閉點擊
                }

                function markWrong() {
                    if (isWrong) return;
                    isWrong = true;
                    backend.playSwoon();
                    strikeFade.start();
                }
            }
        }
    }

    // 🔔 從 C++ 傳入錯誤選項名稱
    onPopOption: function (text) {
        for (let i = 0; i < optionButtons.count; ++i) {
            const btn = optionButtons.itemAt(i);
            if (btn.optionText === text) {
                btn.markWrong();
                break;
            }
        }
    }

    Text{
        id: description
        Layout.alignment: Qt.AlignCenter

        visible: descriptionText.length !== 0
        text: {
            if (answerRevealed && descriptionText.length !== 0) return `解析：${descriptionText}`;
            return "";
        }
        font.pointSize: 20
        font.bold: true
        color: "#ffffff"
    }
}
