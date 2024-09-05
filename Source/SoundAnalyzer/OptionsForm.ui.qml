import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

Dialog {

    id: optionsDialog

    property var window: null
    property alias scaleCombo: scaleComboBox

    width:800
    height:600

    property var scaleModel:[
         "0.0000001"
        ,"0.0000002"
        ,"0.0000005"
        ,"0.000001"
        ,"0.000002"
        ,"0.000005"
        ,"0.00001"
        ,"0.00002"
        ,"0.00005"
        ,"0.0001"
        ,"0.0002"
        ,"0.0005"
        ,"0.001"
        ,"0.002"
        ,"0.005"
        ,"0.1"
        ,"0.2"
        ,"0.5"
        ,"1"
        ,"2"
        ,"5"
        ,"10"
        ,"20"
        ,"50"
        ,"100"
        ,"200"
        ,"500"
        ,"1000"
        ,"2000"
        ,"5000"
        ,"10000"
        ,"20000"
        ,"50000"
        ,"100000"
        ,"200000"
        ,"500000"
        ,"1000000"
        ,"2000000"
        ,"5000000"
        ,"10000000"]


    Timer {

        id: update
        interval: 500
        onTriggered: application.updateDeviceLists()
        running: true
    }

    ListView {
        id: pageView
        x: 0
        y: 0
        width: 159
        height: 480
        model: ListModel {

            ListElement {
                name: "General"
            }
            ListElement {
                name: "Recorder"
            }
            ListElement {
                name: "Goertzel"
            }
        }
        delegate: Rectangle {
            width: 159
            height: 40
            color:"transparent"
            Text{
                anchors.fill: parent
                text: name
                font.pixelSize: 15
                font.bold: index == pageView.currentIndex ? true : false
                color: index == pageView.currentIndex ? "black" : "blue"
                anchors.verticalCenter: parent.verticalCenter
            }
            MouseArea {
                anchors.fill: parent
                onClicked: pageView.currentIndex = index
            }
        }
    }
    Rectangle {

        x:159
        y:0
        width: 1
        height:480
        color:"gray"
    }

    Label {
        id: headline
        x: 160
        y: 0
        width: 600
        height: 55
        text: pageView.model.get(pageView.currentIndex).name
        font.bold: true
        font.pointSize: 17
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }



    Item {
        visible:pageView.currentIndex === 0
        x: 160+15
        y: 55
        width: 600
        height: 425

        CheckBox {
                id : row1b
                width: parent.width
                text: "Automatic hide"
                checked: window.autoHiding
                onCheckedChanged: window.autoHiding = checked
        }
        /*
        CheckBox {
                id:row1c
                width: parent.width
                anchors.top:row1b.bottom
                text: "Stays on top"
                checked: window.staysOnTop
                onCheckedChanged: window.staysOnTop = checked
        }*/
        RowLayout {

            id:row1d
            anchors.top : row1b.bottom
            width:parent.width

            Text{
                text: "Scale"
            }

            ComboBox {
                id: scaleComboBox
                model: optionsDialog.scaleModel

                currentIndex:  {

                    var value=sharedSettings.getValueForKey("general.scale")
                    var i=0;
                    for(var scl in optionsDialog.scaleModel){
                        if( value===parseFloat(optionsDialog.scaleModel[scl])){
                            scl
                            break;
                        }
                        i++;
                    }
                }
                onCurrentIndexChanged: {

                    var val = parseFloat(model[currentIndex])
                    sharedSettings.setValueForKey("general.scale", val)
                    window.setVertGauges(model[currentIndex])
                }
            }
        }
    }


    Item {
        visible:pageView.currentIndex === 1
        x: 160+15
        y: 55
        width: 600
        height: 425

        Item  {
            id: leftGroupItem
            width:parent.width /2
            height:110
            GroupBox {
                title: "Bits per sample"

                RowLayout {
                    id: bytesPerSampleColumn
                    ExclusiveGroup { id: bytesSampleGroup }
                    RadioButton {
                        text: "8 bit"
                        checked: sharedSettings.getValueForKey("recorder.nBytesSample") === 1
                        onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("recorder.nBytesSample", 1)
                        exclusiveGroup: bytesSampleGroup
                    }
                    RadioButton {
                        text: "16 bit"
                        checked: sharedSettings.getValueForKey("recorder.nBytesSample") === 2
                        onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("recorder.nBytesSample", 2)
                        exclusiveGroup: bytesSampleGroup
                    }
                }
            }
        }
        Item  {
            width:parent.width /2
            x:parent.width /2
            GroupBox {
                title: "Channels"

                RowLayout {
                    id: channelsColumn
                    ExclusiveGroup { id: channelsGroup }
                    RadioButton {
                        text: "Mono"
                        checked: sharedSettings.getValueForKey("recorder.nChannels") === 1
                        onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("recorder.nChannels", 1)
                        exclusiveGroup: channelsGroup
                    }
                    RadioButton {
                        text: "Stereo"
                        checked: sharedSettings.getValueForKey("recorder.nChannels") === 2
                        onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("recorder.nChannels", 2)
                        exclusiveGroup: channelsGroup
                    }
                }
            }
        }
        Item {
            width: parent.width
            anchors.top:leftGroupItem.bottom
            RowLayout{
                id: row1
                width: parent.width
                anchors.top:parent.top
                Text {
                    width:200
                    text: "Device"
                }
                ComboBox {
                    width: 200
                    model: sharedData.getValueForKey("recorder.devicesList")
                    currentIndex: {

                        for (var i in model) {

                            if ( model[i] === sharedSettings.getValueForKey("recorder.defaultDevice") ) {

                                i
                                return
                            }
                        }
                        0
                    }
                    onCurrentIndexChanged: if (model) sharedSettings.setValueForKey("recorder.defaultDevice", model[currentIndex] )
                }
            }
            ColumnLayout{
                id: row2
                width: parent.width
                anchors.top:row1.bottom
                Text {
                    width:200
                    text: "Capture device"
                }
                ComboBox {
                    model: sharedData.getValueForKey("recorder.captureDevicesList")
                    currentIndex: {

                        for (var i in model) {

                            if ( model[i] === sharedSettings.getValueForKey("recorder.defaultCaptureDevice") ) {

                                i
                                return
                            }
                        }
                        0
                    }
                    onCurrentIndexChanged: if (model) sharedSettings.setValueForKey("recorder.defaultCaptureDevice", model[currentIndex] )
                }
            }

            RowLayout{
                id: row3
                width: parent.width
                anchors.top:row2.bottom
                Text {
                    text: "Sampling frequency [Hz] (1-200000)"
                }
                TextField {
                    placeholderText: "frequency"
                    validator: IntValidator {bottom: 1; top: 200000;}
                    text: sharedSettings.getValueForKey("recorder.samplingFrequency") ? sharedSettings.getValueForKey("recorder.samplingFrequency") : "44100"
                    onTextChanged: {
                        if ( acceptableInput ) {

                            sharedSettings.setValueForKey("recorder.samplingFrequency", parseInt(text))
                        }
                    }
                }
                Item{}
            }
            RowLayout{
                id: row4
                width: parent.width
                anchors.top:row3.bottom
                Text {
                    text: "Segment length (1-16384)"
                }
                TextField {
                    id: lengthTextFiled
                    placeholderText: "length"
                    validator: IntValidator {bottom: 1; top: 16384;}
                    text: sharedSettings.getValueForKey("recorder.segmentLength") ? sharedSettings.getValueForKey("recorder.segmentLength") : "512"
                    onTextChanged: {
                        if ( acceptableInput ) {

                            sharedSettings.setValueForKey("recorder.segmentLength", parseInt(text))
                            window.setGauges()
                        }
                    }
                }
                Item {}
            }
            RowLayout{
                id: row5
                width: parent.width
                anchors.top:row4.bottom
                Text {
                    text: "Setting from this page will be applied with next recorder run"
                }
                Item {}
            }
        }
    }

    Item {
        visible:pageView.currentIndex === 2
        x: 160+15
        y: 55
        width: 600
        height: 425

        RowLayout{
            id: row21
            width: parent.width

            Text {
                id:overlapText
                text: "Segment overlap (0-16384)"
            }
            TextField {
                id : overlapTextField
                width: parent.width - overlapText.width
                placeholderText: "overlap"
                validator: IntValidator {bottom: 0; top: 16384;}
                text: sharedSettings.getValueForKey("goertzel.overlap") ? sharedSettings.getValueForKey("goertzel.overlap") : "0"
                onTextChanged: {
                    if ( acceptableInput ) {

                        sharedSettings.setValueForKey("goertzel.overlap", parseInt(text))
                        window.setGauges()
                    }
                }
            }
            Item {}
        }
        RowLayout{
            id: row22
            width: parent.width
            anchors.top:row21.bottom
            ColumnLayout{
                Text {
                    text: "Frequency indexes(separated by ';')"
                }
                TextArea {
                    id: frequenciesTextArea
                    width: parent.width
                    height: 120
                    text: sharedSettings.getValueForKey("goertzel.frequencies") ? sharedSettings.getValueForKey("goertzel.frequencies") : "1"
                    onTextChanged: {

                        sharedSettings.setValueForKey("goertzel.frequencies", text)
                    }
                }
                Button{
                    text: "set all"

                    onClicked: {

                        var str="";
                        var overlap = parseInt(overlapTextField.text);
                        var length = parseInt(lengthTextFiled.text)
                        for(var i=0;i<(length+overlap);i++)
                        {
                            str+= i.toString()
                            str+= ";"
                            if (i%10==0)
                            {
                                str+= "\n"
                            }
                        }
                        frequenciesTextArea.text = str
                    }
                }
            }
            Item {}
        }
        GroupBox {
            title: "Device"
            anchors.top:row22.bottom
            RowLayout {
                id: gerDevice
                ExclusiveGroup { id: gerDeviceGroup }
                RadioButton {
                    text: "GPU"
                    checked: sharedSettings.getValueForKey("goertzel.cpu") === false
                    onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("goertzel.cpu", false)
                    exclusiveGroup: gerDeviceGroup
                }
                RadioButton {
                    text: "CPU"
                    checked: sharedSettings.getValueForKey("goertzel.cpu") === true
                    onCheckedChanged: if ( checked ) sharedSettings.setValueForKey("goertzel.cpu", true)
                    exclusiveGroup: gerDeviceGroup
                }
            }
        }
    }

//    ListView {
//        id: recorderOView
//        x: 160
//        y: 55
//        width: 480
//        height: 425
//        model: ListModel {
//            ListElement {
//                name: "Grey"
//                colorCode: "grey"
//            }

//            ListElement {
//                name: "Red"
//                colorCode: "red"
//            }

//            ListElement {
//                name: "Blue"
//                colorCode: "blue"
//            }

//            ListElement {
//                name: "Green"
//                colorCode: "green"
//            }
//        }
//        delegate: Item {
//            x: 5
//            width: 80
//            height: 40
//            Row {
//                id: row2
//                Rectangle {
//                    width: 40
//                    height: 40
//                    color: colorCode
//                }

//                Text {
//                    text: name
//                    font.bold: true
//                    anchors.verticalCenter: parent.verticalCenter
//                }
//                spacing: 10
//            }
//        }
//    }


}
