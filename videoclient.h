#ifndef VIDEOCLIENT_H
#define VIDEOCLIENT_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>
#include <QImage>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QQuickPaintedItem>
#include <iostream>


// Image frame display 
class ImageDisplay : public QQuickPaintedItem
{
  Q_OBJECT
public:
  ImageDisplay(QQuickItem *parent = nullptr);
  void setImage(QImage image);
  void paint(QPainter *painter) override;

private:
  QImage  m_image; 

};


//an http based client 

class VideoClient : public QObject
{
    Q_OBJECT
public:
    explicit VideoClient(QObject *parent = 0);
 
    Q_PROPERTY(bool connected READ getConnected    NOTIFY connectedChanged) ;
    Q_INVOKABLE void connectHttp(const QUrl &requestedUrl);
    Q_INVOKABLE void setImageDisplay( QVariant imageDisplay);  
    void startRequest(const QUrl &requestedUrl);
     

public slots:     
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
    void networkError(QNetworkReply::NetworkError code);

//actions to be performed when the client is disconnected
signals:
    void imageReady(QImage image);
    void connectedChanged();
    void disconnected();
    void error(QString error);
    void downloadProgress(qint64,qint64);
    void message(QString msg);
    

private:
    QNetworkAccessManager qnam;
    //QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    QNetworkReply *reply;

 //property to check if the client is connected
    bool connected;
    bool getConnected() const { return connected; }
    ImageDisplay *imageDisplay;
    
   

};


#endif // VIDEOCLIENT_H
