import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import main 1.0
import QtQuick.Window 2.0

ApplicationWindow {
    visible: true
    width: 1054
    height: 830
    title: qsTr("Sound analyzer")
    id: appWindow

    property bool autoHiding: sharedSettings.getValueForKey("recorder.autoHiding") ? sharedSettings.getValueForKey("recorder.autoHiding") : false
    property bool staysOnTop: sharedSettings.getValueForKey("recorder.staysOnTop") ? sharedSettings.getValueForKey("recorder.staysOnTop") : false




    OptionsForm {
        id: optionsForm
        window: appWindow
        onClickedButtonChanged: {

            hideTimer.start()
        }
    }

    FileDialog {
        id: fileDialog
        title: selectExisting ? "File for reading" : "File for writing"
        //folder: shortcuts.home
       // nameFilters: [ "Sound WAV files (*.wav)"]
        selectMultiple: false
        property bool matfile: false
        onAccepted: {
            hideTimer.start()

            var path = fileDialog.fileUrl.toString();
            // remove prefixed "file:///"
            path= path.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})|(https:\/{2})/,"");
            // unescape html codes like '%23' for '#'
            var cleanPath = decodeURIComponent(path);
            //console.log(cleanPath)

            if (matfile){

                application.processMatFile(cleanPath);
                return;
            }

            sharedSettings.setValueForKey("recorder.inFile", false )
            sharedSettings.setValueForKey("recorder.outFile", false )
            sharedSettings.setValueForKey("recorder.fileName", cleanPath )
            if ( fileDialog.selectExisting )
                sharedSettings.setValueForKey("recorder.inFile", true )
            else
                sharedSettings.setValueForKey("recorder.outFile", true )
        }
        onRejected: {

            hideTimer.start()
        }
    }

    MouseArea {

        anchors.fill: parent
        onClicked: appWindow.show()
    }


    Item {
        anchors.fill: parent
        focus:true
        Keys.onPressed: {

            if ( event.key == Qt.Key_Space) {

                application.pause()
                event.accepted = true
            }
            else if ( event.key == Qt.Key_Plus ) {

                var ndx = optionsForm.scaleCombo.currentIndex
                if ( ndx<(optionsForm.scaleCombo.count - 1) ) {

                    optionsForm.scaleCombo.currentIndex = ndx + 1
                }
                event.accepted = true
            }
            else if ( event.key == Qt.Key_Minus) {

                var ndx = optionsForm.scaleCombo.currentIndex
                if ( ndx>0 ) {

                    optionsForm.scaleCombo.currentIndex = ndx - 1
                }
                event.accepted = true
            }
            else if ( event.key == Qt.Key_S) {

                application.isRunning? application.stop() : application.start()
                appWindow.setGauges()
                event.accepted = true
            }
        }
    }

    Timer {

        id: hideTimer
        interval: 3000
        onTriggered: appWindow.hide()
        running: true

    }

    function hide() {

       if ( !autoHiding ) {

           return
       }

       if ( visibility == Window.Windowed ) appWindow.flags |= Qt.FramelessWindowHint
       toolbar.y = -toolbar.height
       tabbar.y = appWindow.height
    }

    function show() {

        if ( visibility == Window.Windowed ) appWindow.flags &= ~Qt.FramelessWindowHint
        toolbar.y = 0
        tabbar.y = appWindow.height - tabbar.height
        hideTimer.restart()
    }

    function setGauges()
    {
        if (!application.isRunning) {

            barGraphGauge.maximumText = sharedSettings.getValueForKey("recorder.segmentLength").toFixed(0)

        }
    }

    function setVertGauges(maximum)
    {
        barGraphAmpGauge.maximumText = maximum
        barGraphAmpGaugeOS.maximumText = maximum
        barGraphLeftGauge.minimumText = "-"+maximum
        barGraphLeftGauge.maximumText = maximum
        barGraphRightGauge.minimumText = "-"+maximum
        barGraphRightGauge.maximumText = maximum
        var val = parseFloat(maximum)
        topBarGraphG.maximum = val
        topBarGraphGOS.maximum = val
        topBarGraph.minimum = -val
        topBarGraph.maximum = val
        bottomBarGraph.minimum = -val
        bottomBarGraph.maximum = val
    }

    onAutoHidingChanged: {

        sharedSettings.setValueForKey("recorder.autoHiding",  autoHiding )
        if ( autoHiding ) {

            hideTimer.start()
        }
        else {

            hideTimer.stop();
        }
    }
    onStaysOnTopChanged: {

        sharedSettings.setValueForKey("recorder.staysOnTop",  staysOnTop )
        if ( staysOnTop ) {

            appWindow.flags |= Qt.WindowStaysOnTopHint
        }
        else {

            appWindow.flags &= ~Qt.WindowStaysOnTopHint
        }
    }

    onVisibilityChanged: {

        if ( visibility == Window.Maximized ) {

            appWindow.flags &= ~Qt.FramelessWindowHint
        }
        else if ( visibility == Window.Windowed ) {

            if ( toolbar.y == 0 ) {

                appWindow.flags &= ~Qt.FramelessWindowHint
            }
            else{

                appWindow.flags |= Qt.FramelessWindowHint
            }
        }
    }

    Item {
        id: rychly
        visible: tabbar.currentIndex == 0
        anchors.top: toolbar.bottom
        anchors.bottom: tabbar.top
        width:parent.width


        Gauge {
            id : barGraphLeftGauge
            width:  30
            height: application.isStereo ? ( parent.height - 30 ) / 2 : parent.height - 30
            x:0
            color: "yellow"
            minimumText: "-1"
            maximumText: "1"
            horizontal:false
        }
        Gauge {
            id : barGraphRightGauge
            width:  30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            visible: rychly.visible && application.isStereo
            x:0
            color: "yellow"
            minimumText: "-1"
            maximumText: "1"
            horizontal:false
        }
        CBarGraph {
            id : topBarGraph
            objectName: "topBarGraph"
            anchors.top  : parent.top
            x:30
            width:parent.width -30
            height:  application.isStereo ? ( parent.height - 30 ) / 2 : parent.height - 30
            visible:rychly.visible
            color:"blue"
        }
        CBarGraph {
            id : bottomBarGraph
            objectName: "bottomBarGraph"
            x:30
            width:parent.width -30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            visible: rychly.visible && application.isStereo
            color:"red"
        }
        Gauge {
            id: barGraphGauge
            x:30
            width:parent.width-30
            height: 30
            y: ( parent.height - 30 )
            color: "yellow"
            minimumText: "0"
            maximumText: "1"
            minStep: 1
            horizontal:true
        }
    }

    Item {
        id: geortzelView
        visible: tabbar.currentIndex == 1
        anchors.top: toolbar.bottom
        anchors.bottom: tabbar.top
        width:parent.width


        Gauge {
            id : barGraphAmpGauge
            width:  30
            height: ( parent.height - 30 ) / 2
            x:0
            color: "yellow"
            minimumText: "0"
            maximumText: "1"
            horizontal:false
        }
        Gauge {
            id : barGraphPhaseGauge
            width:  30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            x:0
            color: "yellow"
            minimumText:"-Pi"
            maximumText:"+Pi"
            minimum:-Math.PI
            maximum: Math.PI
            horizontal:false
        }
        CBarGraph {
            id : topBarGraphG
            objectName: "topBarGraphG"
            anchors.top  : parent.top
            x:30
            width:parent.width -30
            height:  ( parent.height - 30 ) / 2
            visible:geortzelView.visible
            minimum: 0.0
            maximum: 1.0
            color:"blue"
            alternateColor: application.isStereo ? "red" : "blue"
        }
        CBarGraph {
            id : bottomBarGraphG
            objectName: "bottomBarGraphG"
            x:30
            width:parent.width -30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            visible: geortzelView.visible
            minimum: -Math.PI
            maximum: Math.PI
            color:"green"
        }
        Gauge {
            id: barGraphGaugeG
            x:30
            width:parent.width-30
            height: 30
            y: ( parent.height - 30 )
            color: "yellow"
            minimumText:"0"
            maximumText: application.frequencies
            minStep: application.isStereo ? 0.5 : 1.0
            horizontal:true
        }
    }
    Item {
        id: oneShotView
        visible: tabbar.currentIndex == 2
        anchors.top: toolbar.bottom
        anchors.bottom: tabbar.top
        width:parent.width


        Gauge {
            id : barGraphAmpGaugeOS
            width:  30
            height: ( parent.height - 30 ) / 2
            x:0
            color: "yellow"
            minimumText: "0"
            maximumText: "1"
            horizontal:false
        }
        Gauge {
            id : barGraphPhaseGaugeOS
            width:  30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            x:0
            color: "yellow"
            minimumText:"-Pi"
            maximumText:"+Pi"
            minimum:-Math.PI
            maximum: Math.PI
            horizontal:false
        }
        CBarGraph {
            id : topBarGraphGOS
            objectName: "topBarGraphGOS"
            anchors.top  : parent.top
            x:30
            width:parent.width -30
            height:  ( parent.height - 30 ) / 2
            visible:barGraphPhaseGaugeOS.visible
            minimum: 0.0
            maximum: 1.0
            color:"blue"
        }
        CBarGraph {
            id : bottomBarGraphGOS
            objectName: "bottomBarGraphGOS"
            x:30
            width:parent.width -30
            height: ( parent.height - 30 ) / 2
            y: ( parent.height - 30 ) / 2
            visible: barGraphPhaseGaugeOS.visible
            minimum: -Math.PI
            maximum: Math.PI
            color:"green"
        }
        Gauge {
            id: barGraphGaugeGOS
            objectName: "horizontalGaugeGOS"
            x:30
            width:parent.width-30
            height: 30
            y: ( parent.height - 30 )
            color: "yellow"
            minimumText:"0"
            maximumText:"1"
            minStep: 1.0
            horizontal:true
        }
    }






    MessageDialog {
        id: messageDialog
        objectName: "messageDialog"

        title: ""
        text: ""

        property bool quitAfterAccepted: false

        onAccepted: {

            if ( quitAfterAccepted ) {

                Qt.quit()
            }
        }
    }


    ToolBar{

        id: toolbar
        y:0
        width:parent.width

        RowLayout {
            Button {
                text: "File"

                onClicked: {

                    hideTimer.restart()
                    fileMenu.popup()
                }
                Menu {
                    id: fileMenu

                    MenuItem {
                        text : "Open for reading"
                        onTriggered: {
                            hideTimer.stop()
                            fileDialog.selectExisting = true
                            fileDialog.matfile = false
                            fileDialog.open()
                        }
                    }
                    MenuItem {
                        text: "Open For writing"
                        onTriggered: {
                            hideTimer.stop()
                            fileDialog.selectExisting = false
                            fileDialog.matfile = false
                            fileDialog.open()
                        }
                    }
                    MenuItem {
                        text: "Close file"
                        onTriggered: {
                            sharedSettings.setValueForKey("recorder.inFile", false )
                            sharedSettings.setValueForKey("recorder.outFile", false )
                            sharedSettings.setValueForKey("recorder.fileName", "" )

                        }
                    }
                    MenuSeparator {

                    }
                    MenuItem {
                        text : "Process mat file"
                        onTriggered: {
                            hideTimer.stop()
                            fileDialog.selectExisting = true
                            fileDialog.matfile = true
                            fileDialog.open()
                        }
                    }
                    MenuSeparator {

                    }
                    MenuItem {
                        text: "Quit"
                        onTriggered: appWindow.close()
                    }
                }
            }
            Button {
                text: "Options"

                onClicked: {hideTimer.stop();optionsForm.open();}
            }
            Text{

                text : ""
                function updateText() {

                    text = sharedSettings.getValueForKey("recorder.inFile") ? "Read from\n"+sharedSettings.getValueForKey("recorder.fileName") :
                                                                              ( sharedSettings.getValueForKey("recorder.outFile") ? "Write to\n"+sharedSettings.getValueForKey("recorder.fileName") : "No file\n set")
                }
                Component.onCompleted: {sharedSettings.updated.connect( updateText );updateText()}
            }
            Button {
                text: application.isRunning? "Stop" : "Start"
                onClicked: {

                    application.isRunning? application.stop() : application.start()
                    appWindow.setGauges()
                }
            }
            Button {
                text: application.isPaused? "Continue" : "Pause"
                onClicked: {

                    application.pause()
                }
            }
            Text{
                objectName: "coresText"
                text: ""
            }
            Text{

                objectName: "timeText"
                text: ""
            }
            Text{

                objectName: "computeFrequencyText"
                text : ""
            }
            Text{
                objectName: "samplingFrequencyText"
                text:""
            }

        }
        Behavior on y {

            NumberAnimation {
                target: toolbar
                property: "y"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }

    Rectangle{

        id: tabbar
        y:parent.height - height
        width:parent.width
        color: "lightGray"
        height:40

        property int currentIndex: 0

        Button {
            id:btnRaw
            text: "Timeline"
            onClicked: {tabbar.currentIndex = 0;application.setMode(0);hideTimer.restart()}
        }
        Button {
            id: btnRaw2
            anchors.left: btnRaw.right
            text:"Goertzel"
            onClicked: {tabbar.currentIndex = 1;application.setMode(1);hideTimer.restart()}
        }
        Button {
            anchors.left: btnRaw2.right
            text:"OneShot"
            onClicked: {tabbar.currentIndex = 2;application.setMode(2);hideTimer.restart()}
        }
        Behavior on y {


            NumberAnimation {
                target: tabbar
                property: "y"
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
    }

}
