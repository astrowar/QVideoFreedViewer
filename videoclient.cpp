#include "videoclient.h"
#include <QtCore>
#include <QImage>
#include <QtNetwork>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQuickPaintedItem>
#include <QPen>
#include <QPainter>

 #include <iostream>


ImageDisplay::ImageDisplay(QQuickItem *parent) : QQuickPaintedItem(parent) {
    m_image = QImage();
   // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ImageDisplay::setImage(QImage  image) {
  m_image = image;   
  update();
}

void ImageDisplay::paint(QPainter *painter) {
    if (m_image.isNull()) {
     //   return;
    }
    QColor m_color = QColor(255, 0, 0);
    QPen pen(m_color, 2);
    painter->setPen(pen);
    painter->setRenderHints(QPainter::Antialiasing, true);
    

    //drawi image 
     if (m_image.isNull() == false )
     {
        int awidth = boundingRect().width();
        int aheight = boundingRect().height();
        //keep aspect ratio
        //try fit width
        float w_scale = (float)awidth / m_image.width();

        int next_width =    w_scale * m_image.width();
        int next_height =   w_scale * m_image.height();

        if ( next_height > aheight){
            //dont wort , tri fit by weight
            float h_scale = (float)aheight / m_image.height();
            next_width =  h_scale * m_image.width();
            next_height =   h_scale * m_image.height();
        }
            
     
        int x = (boundingRect().width() - next_width) / 2;
        int y = (boundingRect().height() - next_height) / 2;
        
         painter->drawImage( QRect( x, y, next_width, next_height), m_image);
     }
     else{
        //draw an circle ans a X inside it
        int radius = std::min( boundingRect() .width() , boundingRect().height()) / 2;
        auto center = boundingRect().center();
        painter->drawEllipse(center, radius, radius);
        
    
 

     }
}

//==================================================================================================


static inline QByteArray IntToArray(qint32 source);

VideoClient::VideoClient(QObject *parent) : QObject(parent)
{
    connect(&qnam, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpFinished()));
    connected = false;
    reply = nullptr;
    imageDisplay = nullptr;
 
}

  void VideoClient::setImageDisplay( QVariant imageDisplay)
  {
    this->imageDisplay = imageDisplay.value<ImageDisplay*>();
    std::cout << "setImageDisplay" << std::endl;
        //connect(this, SIGNAL(imageReady(QImage)), display, SLOT(setImage(QImage)));

    }
 

void VideoClient::connectHttp(const QUrl &requestedUrl)
{
    //close current connection
    if (reply != nullptr) {
        reply->abort();
         reply = nullptr;
    }

    startRequest(requestedUrl);
}


 void printDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    std::cout << "Downloaded " << bytesReceived << " of " << bytesTotal << " bytes" << std::endl;
}
 
void  VideoClient::startRequest(const QUrl &requestedUrl)
{
    QUrl url = requestedUrl;

     if (imageDisplay!=nullptr)    imageDisplay->setImage( QImage() );
    //if is too short 
    if ( url.toString().length() < 5)
    {
            if (reply != nullptr) 
            {
                reply->abort();
                reply = nullptr;
            }
            return ;
    }
    
    std::cout << "startRequest at "   << url.toString().toStdString() << std::endl; 
    if (!url.isValid()) {
        std::cout << "Error: Invalid URL" << std::endl;
        emit error("Invalid URL");
        return;
    }
    QNetworkRequest req(  url );
    // close all previous connections
    if (reply != nullptr) {
        reply->abort();
        reply = nullptr;
    }

    reply = qnam.get(req);
    // connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(printDownloadProgress(qint64,qint64) ));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64));
    connect(reply, SIGNAL( errorOccurred(QNetworkReply::NetworkError)), this, SLOT( networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    //connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)) );
 
}


 

void VideoClient::httpFinished()
{
    std::cout << "httpFinished" << std::endl;
    QByteArray data = reply->readAll();
    std::cout << "data size " << data.size() << std::endl;
    QImage image; 
    connected = false  ;
    emit connectedChanged();  
}

void loadQImage(QImage &image,   QByteArray &data)
{
    //data starts with Content-Type: image/jpeg\n\n
    //search for "\r\n\r\n" in the data
    for (int i = 0; i < data.size() - 4; i++)
    {
        if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' && data[i + 3] == '\n')
        {            
             data = data.mid(i + 4); 
            break;
        }
    } 
 
    image.loadFromData(data , "JPEG");
 

}
 

void VideoClient::httpReadyRead()
{

   connected = true ;
    emit connectedChanged();     

    QByteArray data = reply->readAll();

    QImage image;
    loadQImage(image, data);
    if (imageDisplay!=nullptr)    imageDisplay->setImage(image);
    emit imageReady(image);

}

void VideoClient::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
 
}

void VideoClient::networkError(QNetworkReply::NetworkError code)
{
    std::cout << "networkError " <<  code << std::endl;
    if (reply->error() == QNetworkReply::OperationCanceledError)
        return; 
    emit message("Network error" + QString::number(code));
    
}
 

void VideoClient::cancelDownload()
{ 
    //httpRequestAborted = true;
    reply->abort();
}

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
