import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Page{
    id: introRoot
    width: 1000
    height: 700
    anchors.fill: parent

    signal startMain(int gamemode)

    StackLayout{
        id: rootLayout
        anchors.fill: parent

        Rectangle{
            id: introPage

            Image{
                id: introBackground
                anchors.fill: parent
                source: "qrc:images/drawables/firstpagebg.png"
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

                    ComboBox{
                        id: gamemodeChooser
                        Layout.preferredWidth: 230
                        Layout.preferredHeight: 100

                        editable: false
                        model: ["所有題目","只有基本法","只有憲法"]
                        font.pointSize: 20
                    }

                    Button{
                        id: startGame
                        Layout.preferredWidth: 300
                        Layout.preferredHeight: 100
                        text: "開始游戲"
                        font.pointSize: 20
                        onClicked: {
                            backend.initialize();
                            introRoot.startMain(gamemodeChooser.currentIndex);
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
                            text: `（遊戲以程序檔 (.exe) 呈現）

玩法：{0}
每輪遊戲可選擇不同模式（全部題目、僅基本法、僅憲法），系統會隨機抽取{1}題。
玩家需依次作答，答對可進入下一題並播放恭喜音效，答錯則顯示正確答案並播放鼓勵音效。
基本法題庫共{2}題，憲法題庫共{3}題，每輪將隨機抽出{1}題進行遊戲。

等級劃分：（正確率）
    80% ~ 100%  -> S
    55% ~ 80%   -> A
    30% ~ 55%   -> B
    0% ~  30%   -> C`

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
