#include "authwindow.h"

// Lưu ý Linux/WSL phân biệt HOA/thường trong tên file.
// Project của bạn đang có socketclient.h/socketclient.cpp nên include đúng tên này.
#include "socketclient.h"

#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QMap>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QRegularExpression>
#include <QTimer>
#include <QtGlobal>

static QString joinFrom(const QStringList &tok, int start)
{
    QStringList out;
    for (int i = start; i < tok.size(); ++i)
        out << tok[i];
    return out.join(' ');
}

static QString translateErr(const QStringList &tok, const QString &raw)
{
    // Server format: ERR <CODE> <message...>
    if (tok.size() < 2)
        return raw;

    const QString code = tok[1].trimmed().toUpper();
    const QString detail = (tok.size() >= 3) ? joinFrom(tok, 2).trimmed() : QString();

    static const QMap<QString, QString> map = {
        {"AUTH_FAILED", QStringLiteral("Sai tên đăng nhập hoặc mật khẩu.")},
        {"USER_EXISTS", QStringLiteral("Tên đăng nhập đã tồn tại. Vui lòng chọn tên khác.")},
        {"INVALID", QStringLiteral("Yêu cầu không hợp lệ. Vui lòng kiểm tra lại thông tin.")},
        {"NOT_AUTHENTICATED", QStringLiteral("Bạn chưa đăng nhập. Vui lòng đăng nhập trước.")},
        {"NO_SESSION", QStringLiteral("Bạn chưa đăng nhập. Vui lòng đăng nhập trước.")},
        {"KICKED", QStringLiteral("Tài khoản này đã đăng nhập ở nơi khác. Bạn đã bị đăng xuất.")},
        {"FORBIDDEN", QStringLiteral("Bạn không có quyền thực hiện thao tác này.")},
        {"ROOM_NOT_FOUND", QStringLiteral("Không tìm thấy phòng đấu giá.")},
        {"ROOM_ENDED", QStringLiteral("Phòng đã kết thúc.")},
        {"BUY_NOW_NOT_AVAILABLE", QStringLiteral("Vật phẩm này không hỗ trợ mua ngay.")},
        {"BUY_NOW_DISABLED", QStringLiteral("Không thể mua ngay vì giá hiện tại đã đạt hoặc vượt giá mua ngay.")},
        {"BID_TOO_LOW", QStringLiteral("Giá đặt quá thấp. Vui lòng đặt cao hơn giá hiện tại.")},
        {"INVALID_PRICE", QStringLiteral("Giá/giá trị không hợp lệ. Vui lòng kiểm tra lại.")},
    };

    if (map.contains(code))
        return map.value(code);

    // Fallback: nếu server trả detail rõ ràng thì dùng, còn không thì raw
    if (!detail.isEmpty())
        return detail;
    return raw;
}

static QStringList splitWs(const QString &s)
{
    return s.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

static QString defaultHost()
{
    const QString h = qEnvironmentVariable("AUCTION_HOST").trimmed();
    // Khi Qt client chạy trên chính máy Windows chạy WSL: 127.0.0.1 là OK (portproxy)
    // Khi client chạy máy khác: set AUCTION_HOST = IP LAN của máy Windows chạy server (vd 192.168.1.10)
    return h.isEmpty() ? QStringLiteral("127.0.0.1") : h;
}

static quint16 defaultPort()
{
    bool ok = false;
    const int p = qEnvironmentVariableIntValue("AUCTION_PORT", &ok);
    if (ok && p > 0 && p <= 65535)
        return (quint16)p;
    return 8080; // default bạn đang chạy server
}

AuthWindow::AuthWindow(SocketClient *client, QWidget *parent)
    : QDialog(parent), m_client(client)
{

    if (!m_client)
    {
        QMessageBox::critical(this, QStringLiteral("Lỗi"),
                              QStringLiteral("SocketClient = null. Không thể mở cửa sổ xác thực."));
        reject();
        return;
    }

    setWindowTitle(QStringLiteral("Auction - Đăng nhập / Đăng ký"));
    setModal(true);
    resize(520, 340);

    buildUi();

    // signals from SocketClient
    connect(m_client, &SocketClient::connected, this, &AuthWindow::onConnected, Qt::UniqueConnection);
    connect(m_client, &SocketClient::disconnected, this, &AuthWindow::onDisconnected, Qt::UniqueConnection);
    connect(m_client, &SocketClient::lineReceived, this, &AuthWindow::onLineReceived, Qt::UniqueConnection);
    connect(m_client, &SocketClient::errorText, this, &AuthWindow::onErrorText, Qt::UniqueConnection);

    // buttons
    connect(btnConnect, &QPushButton::clicked, this, &AuthWindow::onConnectClicked);
    connect(btnLogin, &QPushButton::clicked, this, &AuthWindow::onLoginClicked);
    connect(btnRegister, &QPushButton::clicked, this, &AuthWindow::onRegisterClicked);

    // Ẩn phần nhập host/port để user không phải nhập tay
    if (edtHost)
        edtHost->hide();
    if (edtPort)
        edtPort->hide();
    if (btnConnect)
        btnConnect->setText(QStringLiteral("Thu ket noi lai"));

    // Tự động kết nối khi mở cửa sổ
    QTimer::singleShot(0, this, &AuthWindow::onConnectClicked);

    setBusy(false);
    setStatus(QStringLiteral("Dang ket noi toi may chu..."));
    updateUiState();
}

void AuthWindow::buildUi()
{
    auto *root = new QVBoxLayout(this);

    // ===== connect row =====
    {
        auto *row = new QHBoxLayout();

        edtHost = new QLineEdit();
        edtHost->setPlaceholderText(QStringLiteral("Host"));
        edtHost->setText(defaultHost());

        edtPort = new QLineEdit();
        edtPort->setPlaceholderText(QStringLiteral("Port (mac dinh 8080)"));
        edtPort->setValidator(new QIntValidator(1, 65535, edtPort));
        edtPort->setText(QString::number(defaultPort()));

        btnConnect = new QPushButton(QStringLiteral("Kết nối"));

        lblConn = new QLabel(QStringLiteral("Trạng thái: Chưa kết nối"));
        lblConn->setMinimumWidth(220);

        row->addWidget(edtHost, 3);
        row->addWidget(edtPort, 1);
        row->addWidget(btnConnect, 1);
        row->addWidget(lblConn, 3);

        root->addLayout(row);
    }

    // ===== tabs =====
    tabs = new QTabWidget();

    // --- login tab ---
    {
        auto *w = new QWidget();
        auto *v = new QVBoxLayout(w);

        auto *form = new QFormLayout();

        edtLoginUser = new QLineEdit();
        edtLoginUser->setPlaceholderText(QStringLiteral("username"));

        edtLoginPass = new QLineEdit();
        edtLoginPass->setPlaceholderText(QStringLiteral("password"));
        edtLoginPass->setEchoMode(QLineEdit::Password);

        form->addRow(QStringLiteral("Tên đăng nhập"), edtLoginUser);
        form->addRow(QStringLiteral("Mật khẩu"), edtLoginPass);

        btnLogin = new QPushButton(QStringLiteral("Đăng nhập"));

        v->addLayout(form);
        v->addWidget(btnLogin);

        tabs->addTab(w, QStringLiteral("Đăng nhập"));
    }

    // --- register tab ---
    {
        auto *w = new QWidget();
        auto *v = new QVBoxLayout(w);

        auto *form = new QFormLayout();

        edtRegUser = new QLineEdit();
        edtRegUser->setPlaceholderText(QStringLiteral("username"));

        edtRegPass = new QLineEdit();
        edtRegPass->setPlaceholderText(QStringLiteral("password"));
        edtRegPass->setEchoMode(QLineEdit::Password);

        edtRegPass2 = new QLineEdit(); //  confirm
        edtRegPass2->setPlaceholderText(QStringLiteral("confirm"));
        edtRegPass2->setEchoMode(QLineEdit::Password);

        form->addRow(QStringLiteral("Tên đăng nhập"), edtRegUser);
        form->addRow(QStringLiteral("Mật khẩu"), edtRegPass);
        form->addRow(QStringLiteral("Xác nhận mật khẩu"), edtRegPass2);

        btnRegister = new QPushButton(QStringLiteral("Đăng ký"));

        v->addLayout(form);
        v->addWidget(btnRegister);

        tabs->addTab(w, QStringLiteral("Đăng ký"));
    }

    root->addWidget(tabs);

    // ===== status =====
    lblStatus = new QLabel();
    lblStatus->setWordWrap(true);
    root->addWidget(lblStatus);
}

void AuthWindow::setBusy(bool busy)
{
    m_busy = busy;
    updateUiState();
}

void AuthWindow::setStatus(const QString &text)
{
    if (lblStatus)
        lblStatus->setText(text);
}

void AuthWindow::updateUiState()
{
    if (lblConn)
    {
        lblConn->setText(m_connected
                             ? QStringLiteral("Trạng thái: Đã kết nối")
                             : QStringLiteral("Trạng thái: Chưa kết nối"));
    }

    // chỉ cho connect khi chưa kết nối
    if (btnConnect)
        btnConnect->setEnabled(!m_busy && !m_connected);
    if (edtHost)
        edtHost->setEnabled(!m_busy && !m_connected);
    if (edtPort)
        edtPort->setEnabled(!m_busy && !m_connected);

    const bool canAuth = m_connected && !m_busy;
    if (btnLogin)
        btnLogin->setEnabled(canAuth);
    if (btnRegister)
        btnRegister->setEnabled(canAuth);

    if (edtLoginUser)
        edtLoginUser->setEnabled(!m_busy);
    if (edtLoginPass)
        edtLoginPass->setEnabled(!m_busy);

    if (edtRegUser)
        edtRegUser->setEnabled(!m_busy);
    if (edtRegPass)
        edtRegPass->setEnabled(!m_busy);
    if (edtRegPass2)
        edtRegPass2->setEnabled(!m_busy);
}

void AuthWindow::onConnectClicked()
{
    if (!m_client || m_busy)
        return;

    if (m_connected)
    {
        QMessageBox::information(this, QStringLiteral("Thông báo"),
                                 QStringLiteral("Bạn đang kết nối sẵn. Nếu muốn đổi server, hãy khởi động lại app."));
        return;
    }

    const QString host = edtHost->text().trimmed();
    const QString portStr = edtPort->text().trimmed();
    const int port = portStr.toInt();

    if (host.isEmpty() || port <= 0 || port > 65535)
    {
        QMessageBox::warning(this, QStringLiteral("Sai thông tin"),
                             QStringLiteral("Host/Port không hợp lệ."));
        return;
    }

    setBusy(true);
    setStatus(QStringLiteral("Đang kết nối tới server..."));
    m_client->connectTo(host, (quint16)port);
}

void AuthWindow::onLoginClicked()
{
    if (!m_client || !m_connected)
    {
        QMessageBox::warning(this, QStringLiteral("Chưa kết nối"),
                             QStringLiteral("Hãy kết nối server trước."));
        return;
    }
    if (m_busy)
        return;

    const QString u = edtLoginUser->text().trimmed();
    const QString p = edtLoginPass->text();

    if (u.isEmpty() || p.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("Thiếu thông tin"),
                             QStringLiteral("Cần nhập tên đăng nhập và mật khẩu."));
        return;
    }

    setBusy(true);
    setStatus(QStringLiteral("Đang đăng nhập..."));
    m_pending = PendingAction::Login;

    //  protocol server: LOGIN <u> <p>
    m_client->sendLine(QString("LOGIN %1 %2").arg(u, p));
}

void AuthWindow::onRegisterClicked()
{
    if (!m_client || !m_connected)
    {
        QMessageBox::warning(this, QStringLiteral("Chưa kết nối"),
                             QStringLiteral("Hãy kết nối server trước."));
        return;
    }
    if (m_busy)
        return;

    const QString u = edtRegUser->text().trimmed();
    const QString p = edtRegPass->text();
    const QString p2 = edtRegPass2 ? edtRegPass2->text() : QString();

    if (u.isEmpty() || p.isEmpty() || p2.isEmpty())
    {
        QMessageBox::warning(this, QStringLiteral("Thiếu thông tin"),
                             QStringLiteral("Cần nhập tên đăng nhập, mật khẩu và xác nhận mật khẩu."));
        return;
    }
    if (p != p2)
    {
        QMessageBox::warning(this, QStringLiteral("Không khớp"),
                             QStringLiteral("Mật khẩu xác nhận không khớp."));
        return;
    }

    setBusy(true);
    setStatus(QStringLiteral("Đang đăng ký..."));
    m_pending = PendingAction::Register;

    //  protocol server: REGISTER <u> <p> (không gửi p2)
    m_client->sendLine(QString("REGISTER %1 %2").arg(u, p));
}

void AuthWindow::onConnected()
{
    m_connected = true;
    setBusy(false);
    setStatus(QStringLiteral("Đã kết nối. Bạn có thể đăng nhập / đăng ký."));
    updateUiState();
}

void AuthWindow::onDisconnected()
{
    m_connected = false;
    setBusy(false);
    setStatus(QStringLiteral("Mat ket noi toi may chu. Hay thu ket noi lai."));
    updateUiState();
}

void AuthWindow::onErrorText(const QString &msg)
{
    setBusy(false);
    setStatus(QStringLiteral("Khong the ket noi. Ly do: ") + msg + QStringLiteral("\n- Neu client chay cung may Windows: host=127.0.0.1\n- Neu client may khac: set AUCTION_HOST=IP_LAN_WINDOWS\n- Port mac dinh: 8080"));
    updateUiState();
}

void AuthWindow::onLineReceived(const QString &line)
{
    const QStringList tok = splitWs(line);
    if (tok.isEmpty())
        return;

    // ===== ERR ... =====
    if (tok[0] == "ERR")
    {
        setBusy(false);
        const QString msg = translateErr(tok, line);

        // Nếu không ở trạng thái chờ login/register hoặc cửa sổ đang bị ẩn
        // thì bỏ qua để tránh hiện thông báo "lạc đề" và double popup.
        if (!this->isVisible() || m_pending == PendingAction::None)
        {
            setStatus(msg);
            updateUiState();
            return;
        }

        QString title = QStringLiteral("Lỗi");
        if (m_pending == PendingAction::Login)
            title = QStringLiteral("Đăng nhập thất bại");
        else if (m_pending == PendingAction::Register)
            title = QStringLiteral("Đăng ký thất bại");

        setStatus(msg);
        QMessageBox::warning(this, title, msg);

        m_pending = PendingAction::None;
        updateUiState();
        return;
    }

    // ===== OK ... =====
    if (tok[0] == "OK")
    {
        // Chỉ xử lý OK khi đang chờ phản hồi login/register.
        if (!this->isVisible() || m_pending == PendingAction::None)
            return;

        // OK LOGIN <uid> <role>
        if (tok.size() >= 4 && tok[1] == "LOGIN")
        {
            m_user = edtLoginUser->text().trimmed();
            m_role = tok[3];

            setBusy(false);
            setStatus(QStringLiteral("Đăng nhập thành công: ") + m_user + QStringLiteral(" (") + m_role + QStringLiteral(")"));
            m_pending = PendingAction::None;
            accept();
            return;
        }

        // OK REGISTERED <uid>
        if (tok.size() >= 3 && tok[1] == "REGISTERED")
        {
            setBusy(false);
            setStatus(QStringLiteral("Đăng ký thành công. Hãy chuyển sang tab Đăng nhập."));

            m_pending = PendingAction::None;

            if (edtRegPass)
                edtRegPass->clear();
            if (edtRegPass2)
                edtRegPass2->clear();

            if (tabs)
                tabs->setCurrentIndex(0);
            updateUiState();
            return;
        }

        // Server bạn có gửi OK HELLO ... / OK HINT ...
        // -> bỏ qua để tránh phá flow
        setBusy(false);
        m_pending = PendingAction::None;
        updateUiState();
        return;
    }

    // các dòng khác (EVT...) -> bỏ qua
}
