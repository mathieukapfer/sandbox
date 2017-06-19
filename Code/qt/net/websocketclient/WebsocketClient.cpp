
#include "WebsocketClient.h"
#include <QtCore/QDebug>
#include <unistd.h>

QT_USE_NAMESPACE

//! [constructor]
EchoClient::EchoClient(const QUrl &url, bool debug, QObject *parent) :
    QObject(parent),
    m_url(url),
    m_debug(debug),
    m_timerId1(0),
    m_timerId2(0)
{
    if (m_debug)
        qDebug() << "[Client] WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &EchoClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &EchoClient::closed);
    m_webSocket.open(QUrl(url));
}
//! [constructor]

//! [onConnected]
void EchoClient::onConnected()
{
    if (m_debug)
        qDebug() << "[Client] WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &EchoClient::onTextMessageReceived);
    m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
    sleep(1);
    m_timerId1 = startTimer(5000);
    m_timerId2 = startTimer(10000);
      //sleep(5);
}
//! [onConnected]

void EchoClient::timerEvent(QTimerEvent *event) {
  if (event->timerId() == m_timerId1) {
    m_webSocket.sendTextMessage(QStringLiteral("Hello again"));
    killTimer(m_timerId1);
  } else if (event->timerId() == m_timerId2) {
    m_webSocket.sendTextMessage(QStringLiteral("Bye bye!"));
    killTimer(m_timerId2);
  } else {
    m_webSocket.sendTextMessage(QStringLiteral("<undefined timer!!!>"));
  }
}
  

//! [onTextMessageReceived]
void EchoClient::onTextMessageReceived(QString message)
{
    if (m_debug)
        qDebug() << "[Client] Message received:" << message;
    if(message == "Bye bye!") {
      qDebug() << "[Client] close WebSocket";
      m_webSocket.close();
    }
}
//! [onTextMessageReceived]

