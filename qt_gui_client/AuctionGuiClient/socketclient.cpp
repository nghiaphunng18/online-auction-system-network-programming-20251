#include "socketclient.h"

#include <QAbstractSocket>



static QString socketErrorToVi(QAbstractSocket::SocketError err) {
    switch (err) {
        case QAbstractSocket::ConnectionRefusedError: return QStringLiteral("Bị từ chối kết nối.");
        case QAbstractSocket::RemoteHostClosedError: return QStringLiteral("Máy chủ đã đóng kết nối.");
        case QAbstractSocket::HostNotFoundError: return QStringLiteral("Không tìm thấy máy chủ.");
        case QAbstractSocket::SocketAccessError: return QStringLiteral("Không có quyền truy cập socket.");
        case QAbstractSocket::SocketResourceError: return QStringLiteral("Thiếu tài nguyên socket.");
        case QAbstractSocket::SocketTimeoutError: return QStringLiteral("Kết nối bị hết thời gian chờ.");
        case QAbstractSocket::DatagramTooLargeError: return QStringLiteral("Gói tin quá lớn.");
        case QAbstractSocket::NetworkError: return QStringLiteral("Lỗi mạng.");
        case QAbstractSocket::AddressInUseError: return QStringLiteral("Địa chỉ đang được sử dụng.");
        case QAbstractSocket::SocketAddressNotAvailableError: return QStringLiteral("Địa chỉ không khả dụng.");
        case QAbstractSocket::UnsupportedSocketOperationError: return QStringLiteral("Thao tác socket không được hỗ trợ.");
        case QAbstractSocket::UnfinishedSocketOperationError: return QStringLiteral("Thao tác socket chưa hoàn tất.");
        case QAbstractSocket::ProxyAuthenticationRequiredError: return QStringLiteral("Proxy yêu cầu xác thực.");
        case QAbstractSocket::ProxyConnectionRefusedError: return QStringLiteral("Proxy từ chối kết nối.");
        case QAbstractSocket::ProxyConnectionClosedError: return QStringLiteral("Proxy đã đóng kết nối.");
        case QAbstractSocket::ProxyConnectionTimeoutError: return QStringLiteral("Kết nối proxy bị hết thời gian chờ.");
        case QAbstractSocket::ProxyNotFoundError: return QStringLiteral("Không tìm thấy proxy.");
        case QAbstractSocket::ProxyProtocolError: return QStringLiteral("Lỗi giao thức proxy.");
        case QAbstractSocket::SslHandshakeFailedError: return QStringLiteral("Bắt tay SSL thất bại.");
        case QAbstractSocket::TemporaryError: return QStringLiteral("Lỗi tạm thời.");
        default: return QStringLiteral("Lỗi socket không xác định.");
    }
}

SocketClient::SocketClient(QObject* parent) : QObject(parent) {
    connect(&sock, &QTcpSocket::connected, this, &SocketClient::connected);
    connect(&sock, &QTcpSocket::disconnected, this, &SocketClient::disconnected);
    connect(&sock, &QTcpSocket::readyRead, this, &SocketClient::onReadyRead);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(&sock, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err) {
        emit errorText(socketErrorToVi(err));
    });
#else
    connect(&sock, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            this, [this](QAbstractSocket::SocketError err) { emit errorText(socketErrorToVi(err)); });
#endif
}

void SocketClient::connectTo(const QString& host, quint16 port) {
    if (sock.state() != QAbstractSocket::UnconnectedState) {
        sock.abort(); // stop current connect / connection
    }
    buffer.clear();
    sock.connectToHost(host, port);
}

void SocketClient::disconnectFromHost() {
    if (sock.state() == QAbstractSocket::UnconnectedState) return;
    sock.disconnectFromHost();
}

bool SocketClient::isConnected() const {
    return sock.state() == QAbstractSocket::ConnectedState;
}

void SocketClient::sendLine(const QString& line) {
    if (!isConnected()) {
        emit errorText("Chưa kết nối.");
        return;
    }
    QByteArray data = line.toUtf8();
    if (!data.endsWith('\n')) data.append('\n');
    sock.write(data);
    sock.flush();
}

void SocketClient::onReadyRead() {
    buffer.append(sock.readAll());

    while (true) {
        int idx = buffer.indexOf('\n');
        if (idx < 0) break;

        QByteArray one = buffer.left(idx);
        buffer.remove(0, idx + 1);

        QString line = QString::fromUtf8(one).trimmed();
        if (!line.isEmpty())
            emit lineReceived(line);
    }
}
