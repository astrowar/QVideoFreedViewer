import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import QtMultimedia  2

import VideoClientModule 1.0
 

Window {
    id:root 
    width: 720
    height: 480
    visible: true
    title: qsTr("Thunter Viewer")

    Material.theme: Material.Dark
    Material.accent: Material.DeepOrange
    Material.background:  Material.Black

    VideoClient{  id: videoClientA  }
    VideoClient{  id: videoClientB  }
    VideoClient{  id: videoClientC  }
    VideoClient{  id: videoClientD  }
    SensorClient{ id: sensorClientTemp  }

    function nameServerURL( currentIndex ){
        if (currentIndex === 0) {  return ("") }
        if (currentIndex === 1) {  return ("http://127.0.0.1:5000/video_feed") }
        if (currentIndex === 2) {  return("http://127.0.0.1:5000/video_bw") }
        if (currentIndex === 3) {  return ("http://127.0.0.1:5000/video_yolo") }
        if (currentIndex === 4) {  return ("http://127.0.0.1:5000/video_web") }

        return "";
       }

    Rectangle {
        color:"#27272a"
        anchors.fill: parent
    }

    RowLayout{
        anchors.fill: parent
        RowLayout{
             id : rowA
              Layout.fillHeight: true
              Layout.fillWidth: true

               ColumnLayout {
                   Layout.fillHeight: true
                   Layout.fillWidth: true

                   Rectangle {
                       color:"transparent"
                       Layout.fillHeight: true
                       Layout.fillWidth: true
                      ComboBox {
                           id: comboA
                           height:  40
                            model: ["None","CAM", "BW", "YOLO"]
                            onCurrentIndexChanged: {
                                   videoClientA.connectHttp(nameServerURL(currentIndex) )
                                   fpsMeter.clear()
                              }
                         }
                       ImageDisplay{
                           id: imageDisplayA
                           anchors.horizontalCenter : parent.horizontalCenter
                           anchors.bottom : parent.bottom  
                           width: parent.width
                           height: parent.height - comboC.height
                       }
 
                   }
                    Rectangle {
                        color:"transparent"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        ComboBox {
                           id: comboB
                           height:  40
                             model: ["None","CAM", "BW", "YOLO"] 
                            onCurrentIndexChanged: {
                                videoClientB.connectHttp(nameServerURL(currentIndex) )
                            }
                         }                        
                        ImageDisplay{
                            id: imageDisplayB
                            anchors.horizontalCenter : parent.horizontalCenter
                            anchors.bottom : parent.bottom  
                            width: parent.width
                            height: parent.height - comboC.height
                        }

                    }
                 }
                 ColumnLayout{
                     Layout.fillHeight: true
                     Layout.fillWidth: true

                    Rectangle {              
                        color:"transparent"
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        ComboBox {
                           id: comboC
                           height:  40
                            model: ["None","CAM", "BW", "YOLO"]                         
                            onCurrentIndexChanged: {
                               videoClientC.connectHttp(nameServerURL(currentIndex) )
                            }
                         }

                        ImageDisplay  {
                            id: imageDisplayC
                            anchors.horizontalCenter : parent.horizontalCenter
                            anchors.bottom : parent.bottom                           
                            width: parent.width
                            height: parent.height - comboC.height
                        }
                    

                    }
                     Rectangle {
                         color:"transparent"
                         Layout.fillHeight: true
                         Layout.fillWidth: true 
                      ComboBox {
                           id: comboD
                           height:  40
                            model: ["None","CAM", "BW", "YOLO", "WEB"	]   
                            onCurrentIndexChanged: {
                                videoClientD.connectHttp(nameServerURL(currentIndex) )
                            }
                         }
                         ImageDisplay{
                             id: imageDisplayD
                             anchors.horizontalCenter : parent.horizontalCenter
                             anchors.bottom : parent.bottom  
                             width: parent.width
                             height: parent.height - comboC.height
                         } 
                     }
                 }
        }


        ColumnLayout{
            Layout.preferredWidth:  200
            Layout.minimumWidth: 200
            Layout.maximumWidth: 200
            Layout.fillHeight: true
             
             Text{
                color: "white"
               text: "SENSOR " + sensorClientTemp.value
             }

             Rectangle {   
                    width: 150
                    height: 150
                    color: "transparent"
                    border.color : Material.accent
                    border.width: 2

                    ColumnLayout{
                             anchors.fill: parent
                             spacing: 4
                            GraphicDisplay {
                                id:fpsMeter                                 
                                //height: 120                                
                                lineColor : "white"

                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.margins : 4      
                            }
                            Text{
                                Layout.alignment: Qt.AlignLeft
                                // only show if videoClientA.frametime is not zero
                                text : "FPS:" +  (videoClientA.frametime > 0 ? (1000.0/(videoClientA.frametime)).toFixed(0) : "0.0" )
                                color: Material.accent
                                font.pixelSize: 20
                                Layout.margins : 4
                                 
                            }
                    }
             }

                Rectangle {
                    id : connectedInfo ;
                    //red or green , defined by  videoClient.connected property
                    color:  videoClientA.connected ? "green" : "red"
                    radius: 7
                    width: 50
                    height: 50
                }
                Button {
                    text: "Connect"
                    Layout.fillWidth: true
                    onClicked: {
                         comboA.currentIndex = 3
                         comboB.currentIndex = 2
                         comboC.currentIndex = 1
                         comboD.currentIndex = 4 
                    }
                }
                Button {
                    text: "Disconnect"
                    Layout.fillWidth: true
                    onClicked: {
                         comboA.currentIndex = 0
                         comboB.currentIndex = 0
                         comboC.currentIndex = 0
                         comboD.currentIndex = 0 
                    }
                }

            }
        }

    //on load windows
    Component.onCompleted: {
        sensorClientTemp.connectHttp("http://127.0.0.1:5000/sensor")	
       // videoClientA.setImageDisplay(imageDisplayA)
       // videoClientB.setImageDisplay(imageDisplayB)
       // videoClientC.setImageDisplay(imageDisplayC)
       // videoClientD.setImageDisplay(imageDisplayD)   
 
    }

    Connections { 
        target: videoClientA
        onImageReady:function(image) { imageDisplayA.setImage(image) }
    }
    Connections { 
        target: videoClientB
        onImageReady: function(image) {imageDisplayB.setImage(image)}
    }
    Connections { 
        target: videoClientC
        onImageReady: function(image) {imageDisplayC.setImage(image)}
    }
    Connections { 
        target: videoClientD
        onImageReady: function(image) {imageDisplayD.setImage(image)}
    }
      
    Timer {
        interval: 300
        running: true
        repeat: true
        onTriggered: {
            // frametimer is in milliseconds
            if (videoClientA.frametime > 0 )fpsMeter.insertPoint(  1000.0/(videoClientA.frametime )  )
            sensorClientTemp.update()            
        }
    }
     



}
