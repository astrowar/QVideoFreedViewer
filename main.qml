import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import QtMultimedia  2

import VideoClient 1.0
import ImageDisplay 1.0

Window {
    id:root 
    width: 720
    height: 480
    visible: true
    title: qsTr("Thunter Viewer")

    VideoClient{  id: videoClientA  }
    VideoClient{  id: videoClientB  }
    VideoClient{  id: videoClientC  }
    VideoClient{  id: videoClientD  }

    function nameServerURL( currentIndex ){
        if (currentIndex === 0) {  return ("") }
        if (currentIndex === 1) {  return ("http://127.0.0.1:5000/video_feed") }
        if (currentIndex === 2) {  return("http://127.0.0.1:5000/video_bw") }
        if (currentIndex === 3) {  return ("http://127.0.0.1:5000/video_yolo") }

        return "";
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
                       color:"#40F02020"
                       Layout.fillHeight: true
                       Layout.fillWidth: true

                       ImageDisplay{
                           id: imageDisplayA
                           anchors.horizontalCenter : parent.horizontalCenter
                           anchors.verticalCenter : parent.verticalCenter
                           width: parent.width
                           height: parent.height
                       }
                       ComboBox {
                           id: comboA
                            model: ["None","CAM", "BW", "YOLO"]
                            onCurrentIndexChanged: {
                                   videoClientA.connectHttp(nameServerURL(currentIndex) )
                              }
                         }
                   }
                    Rectangle {
                        color:"blue"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        ImageDisplay{
                            id: imageDisplayB
                            anchors.horizontalCenter : parent.horizontalCenter
                            anchors.verticalCenter : parent.verticalCenter
                            width: parent.width
                            height: parent.height
                        }
                        ComboBox {
                           id: comboB
                             model: ["None","CAM", "BW", "YOLO"]
                            onCurrentIndexChanged: {
                                videoClientB.connectHttp(nameServerURL(currentIndex) )
                            }
                         }
                    }
                 }
                 ColumnLayout{
                     Layout.fillHeight: true
                     Layout.fillWidth: true

                    Rectangle {
                        color:"green"
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        ImageDisplay{
                            id: imageDisplayC
                            anchors.horizontalCenter : parent.horizontalCenter
                            anchors.verticalCenter : parent.verticalCenter
                            width: parent.width
                            height: parent.height
                        }
                        ComboBox {
                           id: comboC
                            model: ["None","CAM", "BW", "YOLO"]
                            onCurrentIndexChanged: {
                               videoClientC.connectHttp(nameServerURL(currentIndex) )
                            }
                         }

                    }
                     Rectangle {
                         color:"yellow"
                         Layout.fillHeight: true
                         Layout.fillWidth: true
                         ImageDisplay{
                             id: imageDisplayD
                             anchors.horizontalCenter : parent.horizontalCenter
                             anchors.verticalCenter : parent.verticalCenter
                             width: parent.width
                             height: parent.height
                         }
                        ComboBox {
                           id: comboD
                            model: ["None","CAM", "BW", "YOLO"]
                            onCurrentIndexChanged: {
                                videoClientD.connectHttp(nameServerURL(currentIndex) )
                            }
                         }

                     }
                 }
        }


        ColumnLayout{
            Layout.preferredWidth:  200
            Layout.minimumWidth: 200
            Layout.maximumWidth: 200
            Layout.fillHeight: true
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
                    onClicked: {
                         comboA.currentIndex = 1
                         comboB.currentIndex = 2
                         comboC.currentIndex = 3
                         comboD.currentIndex = 0 
                    }
                }

                Button {
                    text: "Connect B"
                    onClicked: {
                      //  videoClientB.connectHttp("http://127.0.0.1:5000/video_feed")
                    }
                }
            }
        }

    //on load windows
    Component.onCompleted: {
        videoClientA.setImageDisplay(imageDisplayA)
        videoClientB.setImageDisplay(imageDisplayB)
        videoClientC.setImageDisplay(imageDisplayC)
        videoClientD.setImageDisplay(imageDisplayD)
    }
      

     Timer {
        interval: 500; running: true; repeat: true
       // onTriggered:  frame.source =   videoClient.requestFrame();
    }



}
