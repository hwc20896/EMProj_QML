import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Page{
    id: introRoot
    width: 1000
    height: 700
    anchors.fill: parent

    StackLayout{
        id: rootLayout
        anchors.fill: parent

        Rectangle{
            id: introPage

            Image{
                id: introBackground
                anchors.fill: parent
                source: "qrc:/images/drawables/firstpagebg.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                id: introMainLayout
                anchors.fill: parent

                RowLayout{
                    id: upperLayout
                    Layout.alignment: Qt.AlignRight | Qt.AlignTop
                    Layout.topMargin: 20
                    Layout.rightMargin: 50
                    spacing: 130

                    MuteSwitch{
                        id: introMuteSwitch
                        width: 70
                        height: 70
                    }

                    Button{
                        id: toRule
                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 70


                        text: "規則頁"
                        font.pointSize: 18

                        onClicked: {
                            rootLayout.currentIndex = 1;
                        }
                    }
                }

                RowLayout{
                    id: lowerLayout
                    Layout.alignment: Qt.AlignRight
                    Layout.rightMargin: 70
                    Layout.bottomMargin: root.height * 0.1
                    spacing: 50

                    Button{
                        id: startGame
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 100
                        text: "開始游戲"
                        font.pointSize: 20
                        onClicked: {
                            backend.initialize();
                            stackView.push(getGame());
                        }
                    }
                }
            }
        }

        Rectangle{
            id: rulePage

            Image{
                id: ruleBackground
                anchors.fill: rulePage
                source: "qrc:images/drawables/qnabg.png"
            }

            ColumnLayout{
                id: ruleLayout
                anchors.fill: parent

                Text{
                    id: ruleTitle
                    text: "游戲規則"
                    Layout.topMargin: 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    font.pointSize: 30
                }

                RowLayout {
                    id: ruleMiddleLayout
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 30

                    Rectangle {
                        color: Qt.rgba(255,255,255,0.45)  // 半透明深色背景，更易讀
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.topMargin: rulePage.height*0.09
                        Layout.leftMargin: parent.width * 0.05
                        Layout.rightMargin: parent.width * 0.05


                        Text {
                            anchors.fill: parent
                            anchors.margins: 35

                            id: ruleContext
                            text: `
這是一個問答遊戲（選擇題 / 是非題）。
題庫共有22題，系統每輪會抽取10條題目進行遊戲。
玩家需依次作答，答對會播放恭喜音效，答錯則播放鼓勵音效。
正確與否，在玩家回答後都出現正確答案。

額外：
- 全對的玩家可登上排行榜（根據時間決定名次）。玩家信息會以UUID v4存儲。

更多功能敬請期待！

等級劃分：（正確率）
    81% ~ 100%  -> S
    61% ~ 80%   -> A
    41% ~ 60%   -> B
    0%  ~ 40%   -> C`

                            color: "black"
                            font.pointSize: 14
                            wrapMode: Text.WordWrap
                            verticalAlignment: Text.AlignTop
                        }
                    }
                }

                Button{
                    id: returnToIntro
                    text: "返回"
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 60
                    Layout.topMargin: 10
                    Layout.bottomMargin: 20
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    onClicked: {
                        rootLayout.currentIndex = 0;
                    }
                }
            }
        }

        currentIndex: 0
    }
}
