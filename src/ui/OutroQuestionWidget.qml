import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: outroQuestionRoot
    width: 1000
    height: 300

    property int questionIndex: 0
    property string questionTitle: "為什麽CJY這麽on9？"
    property var options: ["因為他on", "因為他有點on", "找ljq問去", "不L關我事"]
    property string description: "俺覺得CJY和ljq撮合撮合非常合適。"
    property string correctText: "因為他on"
    property string selectedText: "不L關我事"

    function getBGColor(text){
        if (text === correctText){
            return {
                bg_color: "#4CAF50",
                border_color: "#2e7032"
            };
        }
        if (text === selectedText){
            return {
                bg_color: "#f44336",
                border_color: "#b71c1c"
            };
        }
        return {bg_color: "#f5f5f5", border_color: "#a1a1a1"};
    }

    Rectangle{
        border.color: "#131313"
        border.width: 2
        color: "#e3e3e3"

        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            spacing: 0

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: outroQuestionRoot.width * 0.6
                Layout.leftMargin: 20
                spacing: 15

                Text {
                    id: outroQuestionTitle
                    Layout.preferredWidth: parent.width
                    text: `${questionIndex+1}. ${questionTitle}`
                    font.pointSize: 25
                    wrapMode: Text.WordWrap
                    maximumLineCount: 5
                    elide: Text.ElideRight
                    lineHeight: 1.3
                }

                Text {
                    id: outroDescription
                    width: outroQuestionTitle.width
                    text: `解析：${description}`
                    visible: description.length > 0
                    font.pointSize: 13
                    wrapMode: Text.WordWrap
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: outroQuestionRoot.width * 0.4 - 20
                Layout.rightMargin: 20
                Layout.topMargin: 10
                Layout.bottomMargin: 10
                spacing: 15

                Repeater {
                    model: options

                    Label {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60

                        property string optionText: modelData
                        readonly property var colorInfo: getBGColor(optionText)

                        text: optionText
                        font.pointSize: 20
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: (optionText === selectedText) ? "#0000ff" : "#000000"
                        padding: 10

                        background: Rectangle {
                            radius: 5
                            border.width: 2
                            border.color: colorInfo.border_color
                            color: colorInfo.bg_color
                        }
                    }
                }
            }
        }
    }
}
