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
                        text: "開始遊戲"
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
                anchors.fill: parent
                source: "qrc:/images/drawables/qnabg.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                id: ruleLayout
                anchors.fill: parent
                anchors.margins: 20

                Text{
                    id: ruleTitle
                    text: "遊戲規則"
                    Layout.topMargin: 40
                    Layout.alignment: Qt.AlignHCenter
                    font.pointSize: 30
                }

                ScrollView {
                    id: scrollView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 20
                    Layout.bottomMargin: 20

                    ScrollBar.vertical.policy: ScrollBar.AsNeeded
                    ScrollBar.horizontal.policy: ScrollBar.AsNeeded

                    // 内容区域 - 使用ColumnLayout替代Rectangle
                    ColumnLayout {
                        width: scrollView.width
                        spacing: 0

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.minimumHeight: Math.max(scrollView.availableHeight, ruleContext.implicitHeight + 70)
                            color: Qt.rgba(255, 255, 255, 0.45)

                            Text {
                                id: ruleContext
                                width: parent.width - 70
                                anchors.centerIn: parent

                                text: `這是一個問答遊戲（選擇題 / 是非題）。
題庫共有22題，系統每輪會抽取10條題目進行遊戲。
玩家需依次作答，答對會播放恭喜音效，答錯則播放鼓勵音效。
正確與否，在玩家回答後都出現正確答案。

額外：
- 排除卡：
    > 連續答對三題且回答時間在7秒内即可獲得。
    > 可以在「選擇題」中使用此卡，將會播放「雷聲」並自動排除一個錯誤選項。
- 復活卡：
    > 連續答對五題即可獲得。
    > 在回答所有10條問題後額外顯示「復活題目」供回顧學習，不改變本來答題情況。
    > 只有至少一題回答錯誤，此復活卡才可以使用（使用時變成藍色，可隨時取消），否則按下按鈕什麽也不會發生。
以上技能將在遊戲頁面的右下方顯示。（左邊是復活卡，右邊是排除卡，切記）

- 全對的玩家可登上排行榜（根據時間決定名次）。玩家信息將會以UUID v4存儲。（此功能暫時不可用）

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
                }

                Button{
                    id: returnToIntro
                    text: "返回"
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 60
                    Layout.topMargin: 10
                    Layout.bottomMargin: 20
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        rootLayout.currentIndex = 0;
                    }
                }
            }
        }

        currentIndex: 0
    }
}
