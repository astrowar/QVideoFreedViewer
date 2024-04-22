
#include "sensorclient.h"

#include <QtCore>
#include <QtNetwork>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <iostream>
#include <vector>
#include <string>



SensorClient::SensorClient(QObject *parent) : QObject(parent)
{
    connected = false;
    reply = nullptr;
    value = "None"; 
    url = " ";
} 


void  SensorClient::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl; 
    if ( url.toString().length() < 5)
    {
            if (reply != nullptr) 
            {
                reply->abort();
                reply = nullptr;
                setValue("None");
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
    if (reply != nullptr) {
        reply->abort();
        reply = nullptr;

    }

    reply = qnam.get(req); 
    connect(reply, SIGNAL( errorOccurred(QNetworkReply::NetworkError)), this, SLOT( networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
}


 void  SensorClient::connectHttp(const QUrl &requestedUrl)
{
    if (reply != nullptr) {
        reply->abort();
        reply = nullptr;
    }
    startRequest(requestedUrl); 
}

 

void SensorClient::httpFinished()
{ 
    connected = false  ;
    emit connectedChanged();  
}

QString loadValue(   QByteArray &data)
{
    for (int i = 0; i < data.size() - 4; i++)
    {
        if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' && data[i + 3] == '\n')
        {
            //std::vector<std::string> ret =  getHeaderComponents ( data.mid(0, i) );
            data = data.mid(i + 4); 
            break;
        }
    }
   
    QString str(data);
    return str;  
}

void SensorClient::httpReadyRead()
{
    connected = true ;
    emit connectedChanged();     
    QByteArray data = reply->readAll(); 
    loadValue(   data); 
    setValue( loadValue(   data) );
}

void SensorClient::networkError(QNetworkReply::NetworkError code)
{
     setValue("Error");
    std::cout << "networkError " <<  code << std::endl;
    if (reply->error() == QNetworkReply::OperationCanceledError)
        return; 
      //emit message("Network error" + QString::number(code));
}

void SensorClient::update( )
{
    if (reply != nullptr) {
        reply->abort();
        reply = nullptr;
    }
    startRequest( url );
}
 

