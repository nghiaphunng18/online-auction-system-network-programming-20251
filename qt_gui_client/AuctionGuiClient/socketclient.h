#pragma once
#include <QObject>
#include <QTcpSocket>

class SocketClient : public QObject {
    Q_OBJECT
public:
    explicit SocketClient(QObject* parent=nullptr);

    void connectTo(const QString& host, quint16 port);
    void disconnectFromHost();
    void sendLine(const QString& line);

    bool isConnected() const ;

signals:
    void connected();
    void disconnected();
    void lineReceived(const QString& line);
    void errorText(const QString& msg);

private slots:
    void onReadyRead();

private:
    QTcpSocket sock;
    QByteArray buffer;
};
