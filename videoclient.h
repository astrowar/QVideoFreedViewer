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

//Graphics for read and plot the image
class GraphicDisplay : public QQuickPaintedItem
{
  Q_OBJECT
public:
  GraphicDisplay(QQuickItem *parent = nullptr);  
  void paint(QPainter *painter) override;
  Q_INVOKABLE void clear();
  Q_PROPERTY(QColor lineColor READ getLineColor WRITE setLineColor NOTIFY lineColorChanged) ;

public slots:     
    void insertPoint(float relativeValue );
signals:
    void lineColorChanged();
private:
//list of values of Y 
  QList<float> values;
  QColor  lineColor; 
  QColor getLineColor() const { return lineColor; }
  void setLineColor(QColor value) { if(lineColor != value){ lineColor = value ;  emit lineColorChanged(); } }
};

//an http based client 

class VideoClient : public QObject
{
    Q_OBJECT
public:
    explicit VideoClient(QObject *parent = 0);
 
    Q_PROPERTY(bool connected READ getConnected    NOTIFY connectedChanged) ;
    Q_PROPERTY(int frametime READ getFrametime WRITE setFrametime NOTIFY  frametimeChanged) ;
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
    void frametimeChanged();
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

    int frametime;
    int getFrametime() const { return frametime; }
    void setFrametime(int value) { if(frametime != value){ frametime = value ;  emit frametimeChanged(); } }
    ImageDisplay *imageDisplay;
    
   

};


#endif // VIDEOCLIENT_H
