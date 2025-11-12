import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{
    id: outroRoot

    property int correctCount: backend.correctCount
    property int totalCount: backend.correctCount + backend.incorrectCount

    StackLayout{
        id: outroLayout
        anchors.fill: parent

        Rectangle{
            id: outroMain
            Image{
                anchors.fill: parent
                source: "qrc:/images/drawables/outrobg.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                id: outroMainLayout
                anchors.fill: parent

                Button{
                    id: quitGame
                    text: "退出游戲"
                    Layout.alignment: Qt.AlignTop | Qt.AlignRight
                    Layout.topMargin: outroRoot.height * 0.03
                    Layout.rightMargin: outroRoot.width * 0.2

                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 130

                    font.pixelSize: 20
                    onClicked: Qt.quit()
                }


                RowLayout{
                    Layout.leftMargin: 200
                    Layout.topMargin: 300
                    spacing: 250

                    Button{
                        id: toResult
                        text: "顯示答題情況"
                        Layout.preferredHeight: 100
                        Layout.preferredWidth: 300
                        font.pointSize: 20
                        onClicked: outroLayout.currentIndex = 2;
                    }

                    Button{
                        id: replay
                        text: "重新開始"
                        Layout.preferredHeight: 100
                        Layout.preferredWidth: 300
                        font.pointSize: 20
                        onClicked: stackView.replace(getGame())
                    }
                }

                Button{
                    id: toInfo
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 50
                    Layout.bottomMargin: 20
                    Layout.rightMargin: outroRoot.width * 0.1
                    text: "© 2025."
                    font.pixelSize: 20
                    Layout.alignment: Qt.AlignBottom | Qt.AlignRight
                    onClicked: outroLayout.currentIndex = 1
                }
            }
        }

        Rectangle{
            id: infoPage

            Image{
                anchors.fill: parent
                source: "qrc:/images/drawables/status_info.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                anchors.fill: parent
                Text{
                    text: "專案信息："
                    Layout.alignment: Qt.AlignHCenter
                    font.pointSize: 30
                }

                Text{
                    text: "所屬隊伍：葉洪古范·雙法F4"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter
                }

                Text{
                    text: "分工：（隊名先後順序）
        葉鈺文：配音
        洪偉晴：程序製作，背景音樂
        古藝軒：繪圖
        范嘉盈：問題收集，玩法擴展"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter
                }

                Text{
                    text: "程式語言：C++, Qt
使用音樂：OMFG - Pizza （手動降調至0.8）"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter
                }

                Text{
                    text: "專案儲存庫網址：
https://github.com/hwc20896/EMProj_QML"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter
                }

                Button{
                    text: "返回"
                    Layout.alignment: Qt.AlignHCenter
                    font.pointSize: 20
                    Layout.preferredHeight: 70
                    Layout.preferredWidth: 120
                    onClicked: outroLayout.currentIndex = 0
                }
            }
        }


        Rectangle{
            id: resultPage

            Image{
                anchors.fill: parent
                source: "qrc:/images/drawables/result.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                anchors.fill: parent

                Text{
                    id: resultTitle
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    Layout.topMargin: 20

                    text: "結果"
                    font.pointSize: 30
                }

                Text{
                    id: resultCorrectStatus

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignTop

                    text: {
                        const percentage = Math.round(correctCount/totalCount * 10000) / 100
                        return `總體成績：${correctCount} / ${totalCount} → ${percentage}%`
                    }

                    font.pointSize: 20
                }

                Text{
                    id: resultTimeElapsed

                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                    text: `總作答時間：${backend.getElapsedTime()}`
                    font.pointSize: 20
                }

                Image{
                    id: resultRank

                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    source: {
                        const percentage = Math.round(correctCount/totalCount * 10000) / 100;
                        const formatStr = "qrc:/rank/assets/Dungeon_Ranking_{}.png";

                        if (percentage > 80) return formatStr.replace("{}","S");
                        if (percentage > 60) return formatStr.replace("{}","A");
                        if (percentage > 40) return formatStr.replace("{}","B");
                        return formatStr.replace("{}","C");
                    }
                }

                RowLayout{
                    id: resultBottomLayout
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 200
                    Layout.bottomMargin: 20

                    Button{
                        id: returnToOutro
                        Layout.preferredHeight: 60
                        Layout.preferredWidth: 200

                        text: "返回"
                        font.pointSize: 20

                        onClicked: outroLayout.currentIndex = 0;
                    }

                    Button{
                        id: toStatus
                        Layout.preferredHeight: 60
                        Layout.preferredWidth: 200

                        text: "顯示答題情況"
                        font.pointSize: 20

                        onClicked: outroLayout.currentIndex = 3;
                    }
                }
            }
        }

        Rectangle{
            id: statusPage

            Image{
                anchors.fill: parent
                source: "qrc:/images/drawables/status_info.png"
                fillMode: Image.Stretch
            }

            ColumnLayout{
                anchors.fill: parent
                spacing: 20
                Text{
                    id: statusTitle
                    Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                    text: "題目回顧"
                    font.pointSize: 30
                    Layout.topMargin: 30
                }

                ScrollView{
                    id: statusMain
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.leftMargin: 20
                    Layout.rightMargin: 20

                    ColumnLayout{
                        spacing: 20
                        Layout.fillWidth: true

                        Repeater{
                            model: backend.getSessionQuestionData()

                            OutroQuestionWidget{
                                Layout.fillWidth: true
                                questionIndex: index
                                questionTitle: modelData.questionTitle
                                options: modelData.options
                                description: modelData.description
                                correctText: modelData.correctText
                                selectedText: modelData.sessionSelectedAnswer
                            }
                        }
                    }
                }

                Button{
                    id: returnToResult
                    Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                    text: "返回"
                    font.pointSize: 20
                    Layout.bottomMargin: 20

                    onClicked: outroLayout.currentIndex = 2;
                }
            }
        }

        currentIndex: 0
    }
}
