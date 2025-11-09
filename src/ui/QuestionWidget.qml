import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout{
    id: questionRoot

    //  Examples
    property string title: "為什麽CJY這麽on9？"
    property var optionText: ["因為他on", "因為他有點on", "找ljq問去", "不L關我事"]
    property string descriptionText: "俺覺得CJY和ljq撮合撮合非常合適。"

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

    GridLayout{
        id: optionButtons
        columns: 2
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: 30
        columnSpacing: 20
        rowSpacing: 20

        Repeater{
            model: optionText
            delegate: Button{
                text: modelData
                enabled: !answerRevealed
                Layout.preferredWidth: 300
                Layout.preferredHeight: 120
                font.pointSize: 16

                onClicked:{
                    selectedOption = text
                    backend.currentQuestion.sessionSelectedAnswer = text
                    backend.handleAnswer(text);
                }

                background: Rectangle {
                    radius: 8
                    border.width: 2
                    border.color: {
                        if (!answerRevealed) return "#9E9E9E";
                        return text === correctOption ? "#2E7D32" : "#B71C1C";
                    }

                    // 核心：动态背景色
                    color: {
                        if (!answerRevealed) {
                            return parent.pressed ? "#E0E0E0" : "#FFFFFF"
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

                implicitHeight: 60
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
