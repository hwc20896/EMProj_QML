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

    property var currentQuestion: {}
    property bool answerRevealed: false
    property string selectedOption: ""
    property string correctOption: ""

    Connections{
        target: backend

        function onCurrentQuestionChanged(){
            currentQuestion = backend.currentQuestion;
            answerRevealed = false;
            selectedOption = "";
            correctOption = "";
        }

        function onAnswerResult(isCorrect, correctAnswer, questionId) {
            if (questionId !== currentQuestion.id) return

            answerRevealed = true
            correctOption = correctAnswer
        }
    }

    Image{
        source: "qrc:/images/drawables/qnabg.png"
        fillMode: Image.Stretch
        anchors.fill: parent
    }

    Component.onCompleted: backend.loadQuestions(currentGamemode, 4)

    ColumnLayout{
        id: gameMainLayout
        anchors.fill: parent
        spacing: gamePageRoot.width * 0.07

        RowLayout{
            id: gameUpperLayout
            spacing: 70
            width: gamePageRoot.width
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
            width: gameMainLayout.width
            Layout.alignment: Qt.AlignBottom

            Button{
                id: prevButton
                Layout.leftMargin: 30
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                text: "← 上一題"
                font.pointSize: 15
                enabled: backend.currentQuestionIndex > 0
                onClicked: backend.currentQuestionIndex -= 1;
            }

            Item{ Layout.fillWidth: true }

            Button{
                id: nextButton
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                Layout.rightMargin: 50
                text: "下一題 →"
                font.pointSize: 15
                onClicked: {
                    if (backend.currentQuestionIndex < 3) {
                        backend.currentQuestionIndex += 1;
                    } else {
                        console.log("Last page reached.")
                    }
                }
            }
        }
    }
}
