import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{
    id: gamePageRoot
    width: 1000
    height: 700
    anchors.fill: parent

    property int correctCount: backend.correctCount
    property int incorrectCount: backend.incorrectCount
    property int currentProgress: backend.progress
    property int totalCount: 4

    property var currentQuestion: ({})
    property bool answerRevealed: false
    property string selectedOption: ""
    property string correctOption: ""

    Connections{
        target: backend

        function onCurrentQuestionChanged(){
            currentQuestion = backend.currentQuestion;
            selectedOption = backend.currentQuestion.sessionSelectedAnswer || "";
            correctOption = backend.currentQuestion.correctText || "";
            answerRevealed = backend.currentQuestion.sessionAnswered;
        }

        function onCurrentAnsweredChanged(){
            answerRevealed = backend.currentQuestion.sessionAnswered;
            console.debug(answerRevealed)
            console.debug("Selected option: " + selectedOption)
            console.debug("Correct option: " + correctOption)
        }
    }

    Image{
        source: "qrc:/images/drawables/qnabg.png"
        fillMode: Image.Stretch
        anchors.fill: parent
    }

    Component.onCompleted: {
        backend.loadQuestions(currentGamemode, totalCount);
        backend.startTimer();
    }

    ColumnLayout{
        id: gameMainLayout
        anchors.fill: parent
        spacing: gamePageRoot.width * 0.07

        RowLayout{
            id: gameUpperLayout
            Layout.fillWidth: true
            spacing: 70
            Layout.topMargin: 10
            Layout.alignment: Qt.AlignTop

            Item{Layout.fillWidth: true}

            Text{
                id: scoreDisplayer
                text: `<font color="red">錯誤數：${incorrectCount}</font> | <font color="#7cfc00">正確數：${correctCount}</font>`
                font.pointSize: 20
                Layout.leftMargin: 180
            }

            MuteSwitch{
                id: ingameMuteSwitch
            }

            Text{
                id: progressDisplayer
                text: `進度：${currentProgress} / ${totalCount} - ${Math.round(currentProgress/totalCount * 10000) / 100}%`
                font.pointSize: 20
                Layout.rightMargin: 70
            }
        }

        QuestionWidget{
            id: questionWidget
            title: currentQuestion.questionTitle? currentQuestion.questionTitle : "載入中...\n如果一直停留在這裏，請上報GitHub Issues。"
            optionText: currentQuestion.options || []
        }

        RowLayout{
            id: gameLowerLayout
            Layout.bottomMargin: 20
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true

            Button{
                id: prevButton
                Layout.leftMargin: 30
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                text: "← 上一題"
                font.pointSize: 15
                enabled: backend.currentQuestionIndex > 0
                visible: backend.currentQuestionIndex > 0
                onClicked: backend.currentQuestionIndex -= 1;
            }

            Item{ Layout.fillWidth: true }

            Button{
                id: nextButton
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                Layout.rightMargin: 50
                text: backend.currentQuestionIndex < totalCount - 1? "下一題 →": "完成"
                font.pointSize: 15
                enabled: answerRevealed
                onClicked: {
                    if (backend.currentQuestionIndex < totalCount - 1) {
                        backend.startTimer();
                        backend.currentQuestionIndex += 1;
                    } else {
                        console.log("Last page reached.")
                        backend.finalize();
                    }
                }
            }
        }
    }
}
