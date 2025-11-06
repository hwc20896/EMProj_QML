import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{
    id: outroRoot

    StackLayout{
        id: outroLayout
        anchors.fill: parent

        Rectangle{
            id: outroMain
            Image{
                anchors.fill: parent
                source: "qrc:images/drawables/outrobg.png"
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
                    Layout.preferredWidth: 100

                    font.pixelSize: 20
                    onClicked: Qt.quit()
                }



                Button{
                    id: replay
                    text: "重新開始"
                    Layout.preferredHeight: 100
                    Layout.preferredWidth: 300
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.leftMargin: 400
                    Layout.topMargin: 300
                    font.pointSize: 20
                    onClicked: stackView.replace(getGame())
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

            ColumnLayout{
                id: infoLayout
                anchors.fill: parent

                Text{
                    id: placeholderText
                    text: "此頁面仍在建設中。"
                    font.pointSize: 30
                }

                Button{
                    id: placeholderReturnButton
                    text: "返回"

                    font.pointSize: 30

                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 70

                    onClicked: outroLayout.currentIndex = 0
                }
            }
        }
        currentIndex: 0
    }
}
