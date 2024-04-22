#ifndef SENSORCLIENT_H
#define SENSORCLIENT_H

#include <QObject>
#include <QtCore>
#include <QtNetwork>
//an http based client 

class SensorClient : public QObject
{
    Q_OBJECT
public:
    explicit SensorClient(QObject *parent = 0);
    Q_PROPERTY(bool connected READ getConnected    NOTIFY connectedChanged) ;
    Q_PROPERTY(QString value READ getValue   NOTIFY  valueChanged) ; 
    Q_INVOKABLE void connectHttp(const QUrl &requestedUrl); 
    Q_INVOKABLE void update( ); 
    void startRequest(const QUrl &requestedUrl); //continuous request 


 signals:
    void connectedChanged();
    void valueChanged();
     void error(QString error);
 


 public slots:      
    void httpFinished();
    void httpReadyRead(); 
    void networkError(QNetworkReply::NetworkError code);


private:
    QUrl url;
    QNetworkAccessManager qnam; 
    QNetworkReply *reply;
    QString value;
    bool connected;

    bool getConnected() const { return connected; }
    QString getValue() const { return value; } 
    void setValue(QString value) { if(this->value != value){ this->value = value ;  emit valueChanged(); } }
};

#endif // SENSORCLIENT_H
