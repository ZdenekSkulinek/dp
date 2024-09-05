import QtQuick 2.0




Rectangle {

    id: root
    property alias maximumText: maximumTextid.text
    property alias minimumText: minimumTextid.text
    property bool horizontal: true
    property real minStep: 0.0

    //do not set
    property real maximum: parseFloat(maximumText)
    property real minimum: parseFloat(minimumText)

    property real mainStep: getMainStep(maximum,minimum,minStep)
    property real mainBegin: getBegin(minimum,mainStep)
    property real secStep: getSecStep(minimum,maximum,mainStep, horizontal ? width : height, minStep)
    property real secBegin: getBegin(minimum,secStep)



    clip:true

    function getMainStep(maximum, minimum, minStep)
    {
        if (maximum===Math.PI){

            return Math.PI / 4
        }

        var len =( maximum - minimum ) / 10.0
        if (maximum === -minimum)
        {
            len =( maximum - minimum ) / 20.0
        }

        var p = 0
        while(len > 1.0) {

            p++
            len /= 10.0
        }
        while(len <= 0.1 ) {

            p--;
            len *= 10.0
        }

        if ( len >= 0.3 ){

                len = 1.0
        }
        else {

            len = 0.1

        }
        var newStep = len * Math.pow(10,p)
        if (newStep < minStep && minStep!==0) {

            //console.log("zzz sms "+minStep+"/"+maximum)
            return minStep
        }
        //console.log("zzz smss "+newStep+"/"+maximum)
        return newStep
    }

    function getBegin(minimum, mainStepSize)
    {
        var n = ( minimum) / mainStepSize
        var fn = Math.floor(n)
        if ( fn === n ) {

            return 0
        }
        return ( fn + 1 ) * mainStepSize - minimum
    }

    function getSecStep(minimum,maximum,mainStep, width, minStep)
    {
        if (maximum===Math.PI){

            return Math.PI/20
        }
        var len = width / ( maximum - minimum )
        var msLen = mainStep * len
        var newStep = 0.0
        if ( msLen < 8) {

            newStep =  mainStep
        }
        else if (msLen< 20){

            newStep =  mainStep / 2.0
        }
        else if ( msLen < 40) {

            newStep =  mainStep / 5.0
        }
        else {

            newStep =  mainStep / 10.0
        }
        if (newStep <  minStep && minStep!==0) {

            newStep = minStep
        }
        if (newStep === mainStep) {

            newStep=0
        }
        //console.log("zzz sssms"+newStep+"/"+maximum)
        return newStep
    }

    Repeater{

        model: (Math.floor((root.maximum-root.minimum) / root.secStep) + 1) < (root.horizontal ? root.width : root.height) ? (Math.floor((root.maximum-root.minimum) / root.secStep) + 1) : 0


        Rectangle{
            color: "black"
            width: root.horizontal ? 1 : root.width * 0.25
            height: root.horizontal ? root.height * 0.25 : 1
            x: root.horizontal ? root.secBegin + index / (root.maximum - root.minimum) * root.width * root.secStep : root.width * 0.75
            y: root.horizontal ? 0 : root.height - (root.secBegin + index / (root.maximum - root.minimum) * root.height * root.secStep)

        }
    }

    Repeater{

        model: (Math.floor((root.maximum-root.minimum) / root.mainStep) + 1) < (root.horizontal ? root.width : root.height) ? (Math.floor((root.maximum-root.minimum) / root.mainStep) + 1) : 0


        Rectangle{
            color: "black"
            width: root.horizontal ? 1 : root.width * 0.5
            height: root.horizontal ? root.height * 0.5 : 1
            x: root.horizontal ? root.mainBegin + index / (root.maximum - root.minimum) * root.width * root.mainStep : root.width * 0.5
            y: root.horizontal ? 0 : root.height - (root.secBegin + index / (root.maximum - root.minimum) * root.height * root.mainStep)

        }
    }

    Text {
        id:minimumTextid
        text:"0"
        color: minimum===0.0 ? "red" : "black"
        visible: root.horizontal
        font.pixelSize: height * 0.8
        x:0
        height: root.height * 0.5
        y: root.height * 0.5
        horizontalAlignment: Text.AlignLeft
    }

    Text {
        id:maximumTextid
        text:"1"
        color: maximum===0.0 ? "red" : "black"
        visible: root.horizontal
        font.pixelSize: height * 0.8
        anchors.right: parent.right
        height: root.height * 0.5
        y: root.height * 0.5
        horizontalAlignment: Text.AlignRight
    }
    Text {

        text: minimumTextid.text
        color: minimum===0.0 ? "red" : "black"
        visible: !root.horizontal
        font.pixelSize: width * 0.8
        x:0
        width: root.width * 0.5
        horizontalAlignment: Text.AlignLeft
        rotation: 270
        anchors.bottom: parent.bottom
    }

    Text {

        text: maximumTextid.text
        color: maximum===0.0 ? "red" : "black"
        visible: !root.horizontal
        font.pixelSize: width * 0.8
        anchors.top: parent.top
        width: root.width * 0.5
        x: 0
        horizontalAlignment: Text.AlignRight
        rotation: 270
    }

    Rectangle{
        id: horizZero
        color: "red"
        visible: root.horizontal && root.maximum >= 0 && root.minimum <= 0
        width: 1
        height: root.height * 0.5
        x:  - root.minimum / (root.maximum - root.minimum) * root.width

    }

    Rectangle{
        id: vertZero
        color: "red"
        visible: !root.horizontal && root.maximum >= 0 && root.minimum <= 0
        width: root.width * 0.5
        height: 1
        x: root.width * 0.5
        y: root.height + (root.minimum / (root.maximum - root.minimum) * root.height )

    }

    Text {

        text:"0"
        visible: root.horizontal && root.maximum > 0 && root.minimum < 0
        color: "red"
        font.pixelSize: height * 0.8
        x:0
        height: root.height * 0.5
        y: root.height * 0.5
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: horizZero.horizontalCenter
    }

    Text {

        text:"0"
        visible: !root.horizontal && root.maximum > 0 && root.minimum < 0
        color: "red"
        font.pixelSize: width * 0.8
        width: root.width * 0.5
        x: 0
        horizontalAlignment: Text.AlignHCenter
        rotation: 270
        anchors.verticalCenter: vertZero.verticalCenter
    }

}
