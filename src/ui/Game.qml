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
    property int totalCount: 10

    property var currentQuestion: ({})
    property bool answerRevealed: false
    property string selectedOption: ""
    property string correctOption: ""

    signal gameFinished()

    property bool isReviveChecked: false
    property bool reviveAnswerRevealed: false

    Connections{
        target: backend

        function onCurrentQuestionChanged(){
            // 如果在复活模式，忽略后端的题目更新
            if (isInReviveMode) return;

            currentQuestion = backend.currentQuestion;
            selectedOption = backend.currentQuestion.sessionSelectedAnswer || "";
            correctOption = backend.currentQuestion.correctText || "";
            answerRevealed = backend.currentQuestion.sessionAnswered;
        }

        function onCurrentAnsweredChanged(){
            // 如果在复活模式，忽略后端的答案更新
            if (isInReviveMode) return;

            answerRevealed = backend.currentQuestion.sessionAnswered;
            console.debug(answerRevealed)
            console.debug("Selected option: " + selectedOption)
            console.debug("Correct option: " + correctOption)
        }
    }

    // 修改 onRevive 函数
    function onRevive(){
        if (isInReviveMode) {
            backend.finalize();
            stackView.push(getOutro());
            return;
        }
        reviveImg.opacity = 1.0
        questionWidget.opacity = 0.0
        progressDisplayer.opacity = 0.0
        prevButton.opacity = 0.0
        scoreDisplayer.opacity = 0.0
        isInReviveMode = true
        reviveFade.start()
    }

    Image{
        source: "qrc:/images/drawables/qnabg.png"
        fillMode: Image.Stretch
        anchors.fill: parent
    }

    Component.onCompleted: {
        backend.loadQuestions(totalCount);
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
            descriptionText: currentQuestion.description
            opacity: 1.0
        }

        RowLayout{
            id: gameLowerLayout
            Layout.bottomMargin: 20
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            spacing: 30

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
                id: reviveEnabledButton
                Layout.preferredHeight: 60
                Layout.preferredWidth: 60

                icon{
                    readonly property string disabledReviveURL: "qrc:/images/assets/Revive_Stone_Blackout.png"
                    readonly property string enabledReviveURL: "qrc:/images/assets/Revive_Stone.png"

                    source: backend.isReviveEnabled? enabledReviveURL : disabledReviveURL
                    width: 50
                    height: 50
                    color: "transparent"
                }

                background: Rectangle{
                    color: {
                        if (!backend.isReviveEnabled) return "#e0e0e0";
                        if (!isReviveChecked) return "#efbf04";
                        return "#00ccdd";
                    }
                    radius: 5
                }

                onClicked: {
                    if (incorrectCount === 0) return;
                    isReviveChecked = !isReviveChecked;
                }
            }

            Button{
                id: popWrongButton
                Layout.preferredHeight: 60
                Layout.preferredWidth: 60

                icon{
                    readonly property string disabledPopWrong: "qrc:/images/assets/Pop_Wrong_Blackout.png"
                    readonly property string enabledPopWrong: "qrc:/images/assets/Pop_Wrong.png"

                    source: backend.isPopWrongEnabled? enabledPopWrong : disabledPopWrong
                    width: 50
                    height: 50
                    color: "transparent"
                }

                background: Rectangle{
                    color: {
                        if (!backend.isPopWrongEnabled) return "#e0e0e0";
                        return "#efbf04";
                    }
                    radius: 5
                }

                onClicked: function(){
                    const option = backend.revokeMatch();
                    if (option.length === 0) return;
                    questionWidget.popOption(option);
                }
            }

            Button{
                id: nextButton
                Layout.preferredWidth: 160
                Layout.preferredHeight: 60
                Layout.rightMargin: 50
                text: backend.currentQuestionIndex < totalCount - 1? "下一題 →": "完成"
                font.pointSize: 15
                enabled: answerRevealed
                onClicked: {
                    if (isInReviveMode) {
                        // 复活模式下的特殊处理
                        console.log("Revive question completed.")
                        backend.finalize();
                        stackView.replace(getOutro())
                        return;
                    }

                    if (backend.currentQuestionIndex < totalCount - 1) {
                        backend.startTimer();
                        backend.currentQuestionIndex += 1;
                    } else {
                        console.log("Last page reached.")
                        if (isReviveChecked) onRevive()
                        else {
                            backend.finalize();
                            stackView.replace(getOutro())
                        }
                    }
                }
            }
        }
    }

    Image{
        id: reviveImg
        source: "qrc:/images/assets/Revive_Stone.png"
        opacity: 0.0
        anchors.centerIn: parent
        width: 200
        height: 200
    }

    // 在属性定义部分添加
    property bool isInReviveMode: false
    property var reviveQuestionData: ({})

    // 修改 reviveFade 动画
    SequentialAnimation {
        id: reviveFade
        PauseAnimation{
            duration: 500
        }
        PropertyAnimation {
            target: reviveImg
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 800
            easing.type: Easing.InOutQuad
        }
        ScriptAction {
            script: {
                console.debug("I'm here!")
                // 进入复活模式
                isInReviveMode = true;
                // 获取复活题目并保存
                reviveQuestionData = backend.getRevivalQuestion();
                // 直接设置题目内容
                questionWidget.opacity = 1.0;
                questionWidget.title = reviveQuestionData.questionTitle || "載入中...\n如果一直停留在這裏，請上報GitHub Issues。";
                questionWidget.optionText = reviveQuestionData.options || [];
                questionWidget.descriptionText = reviveQuestionData.description;

                // 重置答案状态
                answerRevealed = false;
                selectedOption = "";
                correctOption = reviveQuestionData.correctText || "";
            }
        }
    }

    function handleReviveAnswer(option) {
        if (reviveAnswerRevealed) return;

        reviveAnswerRevealed = true;
        selectedOption = option;
        correctOption = reviveQuestionData.correctText || "";

        // 检查答案是否正确
        const isCorrect = (option === correctOption);

        if (isCorrect) {
            // 答对了，播放正确声音
            backend.playCorrect();
            console.log("复活题目答对了！");
        } else {
            // 答错了，播放错误声音
            backend.playIncorrect();
            console.log("复活题目答错了");
        }

        // 显示答案
        answerRevealed = true;
    }
}
