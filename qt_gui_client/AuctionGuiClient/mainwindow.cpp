#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QDateTime>
#include <QTimeZone>
#include <QMap>
#include <algorithm>

static QString joinTokens(const QStringList &tok, int start)
{
    QStringList out;
    for (int i = start; i < tok.size(); ++i)
        out << tok[i];
    return out.join(' ');
}

static QString translateServerErr(const QStringList &tok, const QString &raw)
{
    // Expected: ERR <CODE> <message...>
    if (tok.size() < 2)
        return QStringLiteral("Có lỗi xảy ra (phản hồi không hợp lệ từ máy chủ).");

    const QString code = tok[1].trimmed().toUpper();
    const QString detail = (tok.size() >= 3) ? joinTokens(tok, 2).trimmed() : QString();

    // Các lỗi thường gặp (ưu tiên hiển thị tiếng Việt rõ ràng)
    static const QMap<QString, QString> map = {
        {"AUTH_FAILED", QStringLiteral("Sai tên đăng nhập hoặc mật khẩu.")},
        {"NOT_AUTHENTICATED", QStringLiteral("Bạn chưa đăng nhập. Vui lòng đăng nhập trước.")},
        {"NO_SESSION", QStringLiteral("Bạn chưa đăng nhập. Vui lòng đăng nhập trước.")},
        {"FORBIDDEN", QStringLiteral("Bạn không có quyền thực hiện thao tác này.")},
        {"USER_EXISTS", QStringLiteral("Tên đăng nhập đã tồn tại. Vui lòng chọn tên khác.")},
        {"KICKED", QStringLiteral("Tài khoản này đã đăng nhập ở nơi khác. Bạn đã bị đăng xuất.")},

        {"ROOM_NOT_FOUND", QStringLiteral("Không tìm thấy phòng đấu giá.")},
        {"ROOM_ENDED", QStringLiteral("Phòng đã kết thúc.")},
        {"ROOM_NOT_ENDED", QStringLiteral("Phòng chưa kết thúc, không thể xem lịch sử.")},
        {"ROOM_NOT_PENDING", QStringLiteral("Phòng không ở trạng thái 'Chờ'.")},
        {"ROOM_NOT_STARTED", QStringLiteral("Phòng chưa bắt đầu đấu giá.")},

        {"NOT_IN_ROOM", QStringLiteral("Bạn chưa vào phòng. Hãy tham gia phòng trước.")},
        {"NOT_OWNER", QStringLiteral("Chỉ chủ phòng mới được thực hiện thao tác này.")},

        {"USER_NOT_FOUND", QStringLiteral("Không tìm thấy người dùng.")},
        {"USER_OFFLINE", QStringLiteral("Người dùng hiện đang offline.")},

        {"DRAFT_EMPTY", QStringLiteral("Chưa có vật phẩm nháp.")},
        {"NO_ITEMS", QStringLiteral("Chưa có vật phẩm để bắt đầu đấu giá.")},

        {"ITEM_NOT_FOUND", QStringLiteral("Không tìm thấy vật phẩm.")},
        {"ITEM_NOT_RUNNING", QStringLiteral("Vật phẩm hiện không trong trạng thái đang đấu giá.")},
        {"BUY_NOW_NOT_AVAILABLE", QStringLiteral("Vật phẩm này không có giá mua ngay.")},
        {"BUY_NOW_DISABLED", QStringLiteral("Không thể mua ngay vì giá hiện tại đã đạt hoặc vượt giá mua ngay.")},

        {"AUTO_BID_NOT_AVAILABLE", QStringLiteral("Không thể bật Auto-bid cho vật phẩm hiện tại.")},
        {"AUTO_BID_TOO_LOW", QStringLiteral("Giá tối đa quá thấp để tự động đặt giá (cần >= giá tối thiểu).")},

        {"UNKNOWN_CMD", QStringLiteral("Lệnh không hợp lệ.")},
        {"NOT_FOUND", QStringLiteral("Không tìm thấy dữ liệu.")},
    };

    if (map.contains(code))
        return map.value(code);

    // Lỗi cần đọc thêm thông tin
    if (code == "BID_TOO_LOW")
    {
        // detail thường dạng: min=12345
        const int pos = detail.indexOf("min=");
        if (pos >= 0)
        {
            const QString minv = detail.mid(pos + 4).trimmed();
            if (!minv.isEmpty())
                return QStringLiteral("Giá đặt quá thấp. Tối thiểu: %1").arg(minv);
        }
        return QStringLiteral("Giá đặt quá thấp.");
    }

    if (code == "INVALID" || code == "INVALID_PRICE" || code == "INVALID_ITEM_FORMAT")
    {
        const QString d = detail.toLower();
        // Tránh lộ tiếng Anh kiểu 'Usage: ...'
        if (d.startsWith("usage:") || d.contains("expects") || d.contains("required"))
        {
            return QStringLiteral("Sai cú pháp hoặc thiếu thông tin. Vui lòng kiểm tra lại thao tác.");
        }

        // Một vài câu chi tiết phổ biến
        if (d.contains("room_id") && (d.contains("must be a number") || d.contains("must be number")))
        {
            return QStringLiteral("ID phòng phải là số.");
        }
        if (d.contains("item_id") && (d.contains("must be a number") || d.contains("must be number")))
        {
            return QStringLiteral("ID vật phẩm phải là số.");
        }
        if (d.contains("amount") && (d.contains("must be numbers") || d.contains("must be number") || d.contains("must be a number")))
        {
            return QStringLiteral("Số tiền/giá trị nhập vào phải là số.");
        }
        if (d.contains("message required"))
        {
            return QStringLiteral("Tin nhắn không được để trống.");
        }
        if (d.contains("recipient") && d.contains("required"))
        {
            return QStringLiteral("Thiếu người nhận tin nhắn.");
        }
        if (d.contains("start must be > 0"))
        {
            return QStringLiteral("Giá khởi điểm phải lớn hơn 0.");
        }
        if (d.contains("duration"))
{
    // Support both old/new server rules (>=10 or >=40)
    if (d.contains("40"))
        return QStringLiteral("Thời lượng phải tối thiểu 40 giây.");
    if (d.contains(">= 10") || d.contains("10"))
        return QStringLiteral("Thời lượng phải tối thiểu 10 giây.");
}
        if (d.contains("buy_now"))
{
    if (d.contains("2x") || d.contains("gấp đôi") || d.contains("double") || d.contains("2 *") || d.contains("2*"))
        return QStringLiteral("Giá mua ngay phải bằng 0 hoặc tối thiểu gấp đôi giá khởi điểm.");
    if (d.contains(">= start") || d.contains(">=start"))
        return QStringLiteral("Giá mua ngay phải bằng 0 hoặc lớn hơn/ bằng giá khởi điểm.");
}

        return QStringLiteral("Yêu cầu không hợp lệ hoặc không thể thực hiện.");
    }

    // fallback: không hiển thị raw/detail tiếng Anh dài dòng
    return QStringLiteral("Có lỗi xảy ra (mã lỗi: %1).").arg(code);
}

static QString defaultHost()
{
    const QString h = qEnvironmentVariable("AUCTION_HOST").trimmed();
    return h.isEmpty() ? QStringLiteral("127.0.0.1") : h;
}

static quint16 defaultPort()
{
    bool ok = false;
    const int p = qEnvironmentVariableIntValue("AUCTION_PORT", &ok);
    if (ok && p > 0 && p <= 65535)
        return (quint16)p;
    return 8080;
}

namespace
{
    static QString statusToVi(const QString &code)
    {
        const QString c = code.trimmed().toUpper();
        if (c == "PENDING" || c == "ROOM_PENDING")
            return QStringLiteral("Chờ");
        if (c == "STARTED" || c == "ROOM_STARTED")
            return QStringLiteral("Đang đấu giá");
        if (c == "ENDED" || c == "ROOM_ENDED")
            return QStringLiteral("Đã kết thúc");
        if (c == "NONE")
            return QStringLiteral("-");
        return code; // fallback: giữ nguyên nếu không nhận diện
    }

    static QString comboStatusWantedCode(int idx)
    {
        // cmbRoomStatus: 0 = tất cả, 1 = PENDING, 2 = STARTED, 3 = ENDED
        switch (idx)
        {
        case 1:
            return QStringLiteral("PENDING");
        case 2:
            return QStringLiteral("STARTED");
        case 3:
            return QStringLiteral("ENDED");
        default:
            return QString();
        }
    }
} // namespace

#include <QHeaderView>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTableWidgetItem>
#include <QtGlobal>
#include <stdexcept>
#include <utility>
#include "authwindow.h"

MainWindow::MainWindow(SocketClient *client, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      sock(client)
{

    ui->setupUi(this);

    // Default server (ẩn user không phải nhập IP/port)
    if (ui->edtHost)
        ui->edtHost->setText(defaultHost());
    if (ui->spinPort)
        ui->spinPort->setValue((int)defaultPort());

    // Ẩn host/port nếu bạn muốn UI gọn hơn
    if (ui->edtHost)
        ui->edtHost->hide();
    if (ui->spinPort)
        ui->spinPort->hide();
    if (ui->lblHost)
        ui->lblHost->hide();
    if (ui->lblPort)
        ui->lblPort->hide();
    if (ui->btnConnect)
        ui->btnConnect->setText("Thu ket noi lai");

    if (!sock)
    {
        QMessageBox::critical(this, "Lỗi nghiêm trọng", "SocketClient bị rỗng (null)!");
        throw std::runtime_error("SocketClient bị rỗng (null)");
    }

    /* =======================
     * Render Room as a TAB (no new window)
     * ======================= */
    if (ui->stackedWidget && ui->pageRoom && ui->tabHome)
    {
        ui->stackedWidget->removeWidget(ui->pageRoom);
        m_tabRoomIndex = ui->tabHome->addTab(ui->pageRoom, "Phòng hiện tại");
        ui->tabHome->setTabEnabled(m_tabRoomIndex, false);
    }

    // Chat tab index (created in .ui)
    if (ui->tabHome && ui->tabChat)
    {
        m_tabChatIndex = ui->tabHome->indexOf(ui->tabChat);
    }
    if (ui->lblChatPeer)
        ui->lblChatPeer->setText("Đang chat với: -");
    if (ui->btnChatSend)
        ui->btnChatSend->setEnabled(false);

    // Make chat user panel narrower than chat box
    if (ui->grpChatUsers)
    {
        ui->grpChatUsers->setMinimumWidth(240);
        ui->grpChatUsers->setMaximumWidth(320);
        ui->grpChatUsers->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    if (ui->hChat)
    {
        ui->hChat->setStretch(0, 0); // users: fixed
        ui->hChat->setStretch(1, 1); // messages: stretch
    }

    //  Create room-chat input ONCE
    ensureRoomChatBar();
    setRoomChatEnabled(false);

    // countdown timer (client-side)
    m_countdownTimer = new QTimer(this);
    m_countdownTimer->setInterval(1000);
    connect(m_countdownTimer, &QTimer::timeout, this, &MainWindow::tickCountdown);

    /* =======================
     * Init tables
     * ======================= */
    ui->tblRooms->setColumnCount(5);
    ui->tblRooms->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblRooms->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblRooms->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblRooms->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tblQueue->setColumnCount(5);
    ui->tblQueue->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblQueue->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tblDraft->setColumnCount(6);
    ui->tblDraft->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblDraft->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblDraft->setHorizontalHeaderLabels(QStringList() << "Mã nháp" << "Tên" << "Giá khởi điểm" << "Mua ngay" << "Thời lượng" << "Xóa");

    ui->tblEnded->setColumnCount(6);
    ui->tblEnded->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblEnded->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblEnded->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tblHistory->setColumnCount(8);
    ui->tblHistory->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

    /* =======================
     * Connect UI signals
     * ======================= */
    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectClicked);

    // Auto-connect khi mở app
    QTimer::singleShot(0, this, [this]()
                       {
    if (!sock || sock->isConnected()) return;
    onConnectClicked(); });

    connect(ui->btnDisconnect, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);

    connect(ui->btnBackToConnect, &QPushButton::clicked, this, &MainWindow::onBackToConnect);
    connect(ui->btnLogin, &QPushButton::clicked, this, &MainWindow::onLoginClicked);

    connect(ui->btnRefreshRooms, &QPushButton::clicked, this, &MainWindow::onRefreshRooms);
    connect(ui->btnCreateRoom, &QPushButton::clicked, this, &MainWindow::onCreateRoom);
    connect(ui->btnJoinRoom, &QPushButton::clicked, this, &MainWindow::onJoinRoom);
    connect(ui->btnJoinSelected, &QPushButton::clicked, this, &MainWindow::onJoinSelected);
    connect(ui->btnOpenHistory, &QPushButton::clicked, this, &MainWindow::onOpenHistoryTab);
    connect(ui->btnRefreshHistory, &QPushButton::clicked, this, &MainWindow::onRefreshHistory);

    connect(ui->edtRoomSearch, &QLineEdit::textChanged, this, &MainWindow::onRoomFilterChanged);
    connect(ui->cmbRoomScope, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onRoomFilterChanged);
    connect(ui->cmbRoomStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onRoomFilterChanged);
    connect(ui->tblRooms, &QTableWidget::itemSelectionChanged, this, &MainWindow::onRoomsSelectionChanged);

    // Rooms tab: selected room users -> chat quick access
    if (ui->edtSelRoomUserSearch)
    {
        connect(ui->edtSelRoomUserSearch, &QLineEdit::textChanged, this, &MainWindow::onSelRoomUserSearchChanged);
    }
    if (ui->lstSelRoomUsers)
    {
        connect(ui->lstSelRoomUsers, &QListWidget::itemDoubleClicked, this, &MainWindow::onSelRoomUserDoubleClicked);
    }

    connect(ui->edtHistSearch, &QLineEdit::textChanged, this, &MainWindow::onHistoryFilterChanged);
    connect(ui->btnLogout, &QPushButton::clicked, this, &MainWindow::onLogout);

    // Room tab
    connect(ui->btnLeaveRoom, &QPushButton::clicked, this, &MainWindow::onLeaveRoom);
    connect(ui->btnStartRoom, &QPushButton::clicked, this, &MainWindow::onStartRoom);
    connect(ui->btnEndRoom, &QPushButton::clicked, this, &MainWindow::onEndRoom);
    connect(ui->btnViewQueued, &QPushButton::clicked, this, &MainWindow::onViewQueued);
    connect(ui->btnViewEnded, &QPushButton::clicked, this, &MainWindow::onViewEnded);

    // Seller draft
    connect(ui->btnAddDraft, &QPushButton::clicked, this, &MainWindow::onAddDraft);
    connect(ui->btnDraftList, &QPushButton::clicked, this, &MainWindow::onDraftList);
    connect(ui->btnPublishOk, &QPushButton::clicked, this, &MainWindow::onPublishOk);

    // Buyer actions
    connect(ui->btnBid, &QPushButton::clicked, this, &MainWindow::onBid);
    connect(ui->btnBuyNow, &QPushButton::clicked, this, &MainWindow::onBuyNow);

    
    // Auto-bid (server-side)
    if (ui->btnAutoBidApply)
        connect(ui->btnAutoBidApply, &QPushButton::clicked, this, &MainWindow::onAutoBidApply);
    if (ui->btnAutoBidOff)
        connect(ui->btnAutoBidOff, &QPushButton::clicked, this, &MainWindow::onAutoBidOff);
// Chat
    if (ui->btnRefreshChatUsers)
        connect(ui->btnRefreshChatUsers, &QPushButton::clicked, this, &MainWindow::onRefreshChatUsers);
    if (ui->btnChatUsers)
        connect(ui->btnChatUsers, &QPushButton::clicked, this, &MainWindow::onRefreshChatUsers);
    if (ui->btnChatSend)
        connect(ui->btnChatSend, &QPushButton::clicked, this, &MainWindow::onChatSend);
    if (ui->edtChatMsg)
        connect(ui->edtChatMsg, &QLineEdit::returnPressed, this, &MainWindow::onChatSend);

    if (ui->lstChatUsers)
    {
        connect(ui->lstChatUsers, &QListWidget::itemClicked, this, [this](QListWidgetItem *it)
                {
            if (!it) return;
            const QString peer = it->text().trimmed();
            if (!peer.isEmpty()) openChatWith(peer, false); });
    }
    if (ui->lstRoomUsers)
    {
        connect(ui->lstRoomUsers, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *it)
                {
            if (!it) return;
            const QString peer = it->text().trimmed();
            if (!peer.isEmpty()) openChatWith(peer, true); });
    }

    /* =======================
     * Connect socket signals
     * ======================= */
    connect(sock, &SocketClient::connected, this, &MainWindow::onSockConnected, Qt::UniqueConnection);
    connect(sock, &SocketClient::disconnected, this, &MainWindow::onSockDisconnected, Qt::UniqueConnection);
    connect(sock, &SocketClient::errorText, this, &MainWindow::onSockError, Qt::UniqueConnection);
    connect(sock, &SocketClient::lineReceived, this, &MainWindow::onLine, Qt::UniqueConnection);

    /* =======================
     * UI defaults
     * ======================= */
    ui->btnDisconnect->setEnabled(sock->isConnected());
    ui->grpDraft->setVisible(false);
    ui->tabHome->setCurrentIndex(0);
    ui->btnJoinSelected->setEnabled(false);
    ui->lblHistoryStatus->setText("Trạng thái: -");

    closeRoomTab(false);

    // If socket already connected (AuthWindow connected first)
    if (sock->isConnected())
    {
        ui->lblConnStatus->setText("Trạng thái: Đã kết nối");
        statusBar()->showMessage("Đã kết nối (từ màn hình đăng nhập).");
        gotoPage(2);
    }
    else
    {
        ui->lblConnStatus->setText("Trạng thái: Chưa kết nối");
        gotoPage(0);
    }

    statusBar()->showMessage("Sẵn sàng.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSession(const QString &user, const QString &userRole)
{
    username = user.trimmed();
    role = userRole;
    m_hasSession = !username.isEmpty();

    ui->lblAuthStatus->setText(QString("Trạng thái: Đã đăng nhập: %1 (%2)").arg(username, role));
    ui->lblUserInfo->setText(QString("Xin chào: %1 (vai trò: %2)").arg(username, role));

    const bool isSeller = (role == "SELLER" || role == "BOTH");
    ui->grpDraft->setVisible(isSeller);

    gotoPage(2);
    ui->tabHome->setCurrentIndex(0);
    onRefreshRooms();
}

void MainWindow::gotoPage(int idx)
{
    ui->stackedWidget->setCurrentIndex(idx);
}

void MainWindow::logLine(const QString &s)
{
    ui->txtLog->appendPlainText(s);
}

bool MainWindow::suppressDuplicateNotice(const QString &key, int windowMs)
{
    const QString k = key.trimmed();
    if (k.isEmpty())
        return false;

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 last = m_noticeLastMs.value(k, -1);

    if (last >= 0 && (now - last) < windowMs)
    {
        return true;
    }

    m_noticeLastMs.insert(k, now);

    // Prune to keep memory small
    if (m_noticeLastMs.size() > 256)
    {
        // remove oldest ~64 entries
        QList<QPair<QString, qint64>> items;
        items.reserve(m_noticeLastMs.size());
        for (auto it = m_noticeLastMs.constBegin(); it != m_noticeLastMs.constEnd(); ++it)
        {
            items.append({it.key(), it.value()});
        }
        std::sort(items.begin(), items.end(), [](const auto &a, const auto &b)
                  { return a.second < b.second; });
        const int removeCount = qMin(64, items.size());
        for (int i = 0; i < removeCount; ++i)
            m_noticeLastMs.remove(items[i].first);
    }
    return false;
}

void MainWindow::showNotice(const QString &key,
                            const QString &title,
                            const QString &message,
                            bool popup)
{
    const QString msg = message.trimmed().isEmpty()
                            ? QStringLiteral("Có lỗi xảy ra.")
                            : message.trimmed();

    if (suppressDuplicateNotice(key, 800))
    {
        // Still update status bar quietly (no popup) so user can see the latest status.
        statusBar()->showMessage(msg, 2000);
        return;
    }

    statusBar()->showMessage(msg, 4000);
    if (popup)
    {
        QMessageBox::warning(this, title, msg);
    }
}

QStringList MainWindow::splitWs(const QString &s)
{
    return s.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}

/* =======================
 * Room chat bar (create once)
 * ======================= */
void MainWindow::ensureRoomChatBar()
{
    if (m_roomChatEdit && m_roomChatSendBtn)
        return;
    if (!ui->vLeft || !ui->pageRoom)
        return;

    QWidget *bar = new QWidget(ui->pageRoom);
    auto *h = new QHBoxLayout(bar);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(8);

    m_roomChatEdit = new QLineEdit(bar);
    m_roomChatEdit->setPlaceholderText("Nhập chat phòng...");
    m_roomChatSendBtn = new QPushButton("Gửi", bar);

    h->addWidget(m_roomChatEdit, 1);
    h->addWidget(m_roomChatSendBtn, 0);

    ui->vLeft->addWidget(bar);

    connect(m_roomChatSendBtn, &QPushButton::clicked, this, &MainWindow::onRoomChatSend, Qt::UniqueConnection);
    connect(m_roomChatEdit, &QLineEdit::returnPressed, this, &MainWindow::onRoomChatSend, Qt::UniqueConnection);
}

void MainWindow::setRoomChatEnabled(bool on)
{
    ensureRoomChatBar();
    if (m_roomChatEdit)
    {
        m_roomChatEdit->setEnabled(on);
        if (!on)
            m_roomChatEdit->clear();
    }
    if (m_roomChatSendBtn)
        m_roomChatSendBtn->setEnabled(on);
}

void MainWindow::setRoomHeader(const QString &status, int participants)
{
    m_roomStatus = statusToVi(status);
    setRoomParticipants(participants);
}

void MainWindow::setRoomParticipants(int participants)
{
    m_roomParticipants = participants;
    if (!ui->lblRoomStatus)
        return;

    const QString st = m_roomStatus.isEmpty() ? QString("-") : m_roomStatus;
    const QString part = (participants >= 0) ? QString::number(participants) : QString("-");
    ui->lblRoomStatus->setText(QString("Trạng thái: %1 | Người trong phòng: %2").arg(st, part));
}

void MainWindow::stopCountdown()
{
    m_timeLeftSec = -1;
    if (m_countdownTimer)
        m_countdownTimer->stop();
}

int MainWindow::parseMmssToSeconds(const QString &mmss)
{
    const QString s = mmss.trimmed();
    const int colon = s.indexOf(':');
    if (colon < 0)
        return -1;

    bool okM = false;
    bool okS = false;
    const int m = s.left(colon).toInt(&okM);
    const int sec = s.mid(colon + 1).toInt(&okS);
    if (!okM || !okS || m < 0 || sec < 0)
        return -1;
    return m * 60 + sec;
}

QString MainWindow::formatSecondsToMmss(int sec)
{
    if (sec < 0)
        return "-";
    int s = sec;
    if (s < 0)
        s = 0;
    const int m = s / 60;
    const int r = s % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(r, 2, 10, QChar('0'));
}

void MainWindow::syncCountdownFromMmss(const QString &mmss)
{
    const int sec = parseMmssToSeconds(mmss);
    if (sec < 0)
        return;

    m_timeLeftSec = sec;
    if (ui->lblItemTime)
        ui->lblItemTime->setText(formatSecondsToMmss(m_timeLeftSec));
    if (m_countdownTimer && m_timeLeftSec > 0)
        m_countdownTimer->start();
    if (m_countdownTimer && m_timeLeftSec <= 0)
        m_countdownTimer->stop();
}

void MainWindow::tickCountdown()
{
    if (m_timeLeftSec < 0)
    {
        if (m_countdownTimer)
            m_countdownTimer->stop();
        return;
    }

    if (m_timeLeftSec > 0)
    {
        m_timeLeftSec -= 1;
        if (ui->lblItemTime)
            ui->lblItemTime->setText(formatSecondsToMmss(m_timeLeftSec));
    }

    if (m_timeLeftSec <= 0 && m_countdownTimer)
    {
        m_countdownTimer->stop();
    }
}

/* =======================
 * Room tab helpers
 * ======================= */
void MainWindow::openRoomTab()
{
    if (!ui->tabHome || m_tabRoomIndex < 0)
        return;
    ui->tabHome->setTabEnabled(m_tabRoomIndex, true);
    ui->tabHome->setCurrentIndex(m_tabRoomIndex);
}

void MainWindow::closeRoomTab(bool switchToRoomsTab)
{
    // Reset chế độ xem phòng đã kết thúc (không JOIN) về mặc định
    setProperty("ended_view_mode", false);
    if (ui->btnLeaveRoom)
        ui->btnLeaveRoom->setText("Rời phòng");
    // Re-enable các nút thao tác phòng (phòng thường)
    if (ui->btnStartRoom)
        ui->btnStartRoom->setEnabled(true);
    if (ui->btnEndRoom)
        ui->btnEndRoom->setEnabled(true);
    if (ui->btnViewQueued)
        ui->btnViewQueued->setEnabled(true);
    if (ui->btnViewEnded)
        ui->btnViewEnded->setEnabled(true);
    if (ui->btnAddDraft)
        ui->btnAddDraft->setEnabled(true);
    if (ui->btnDraftList)
        ui->btnDraftList->setEnabled(true);
    if (ui->btnPublishOk)
        ui->btnPublishOk->setEnabled(true);
    if (ui->btnBid)
        ui->btnBid->setEnabled(true);
    if (ui->btnBuyNow)
        ui->btnBuyNow->setEnabled(true);
    if (ui->edtBidAmount)
        ui->edtBidAmount->setEnabled(true);
    if (ui->edtDraftName)
        ui->edtDraftName->setEnabled(true);
    if (ui->edtDraftStart)
        ui->edtDraftStart->setEnabled(true);
    if (ui->edtDraftBuy)
        ui->edtDraftBuy->setEnabled(true);
    if (ui->edtDraftDur)
        ui->edtDraftDur->setEnabled(true);
    if (ui->tabHome && m_tabRoomIndex >= 0)
    {
        ui->tabHome->setTabEnabled(m_tabRoomIndex, false);
        if (switchToRoomsTab)
            ui->tabHome->setCurrentIndex(0);
    }

    currentRoomId = 0;
    currentItemId = 0;
    m_roomStatus.clear();
    m_roomParticipants = 0;
    stopCountdown();

    ui->lblRoomTitle->setText("Phòng: -");
    ui->lblRoomStatus->setText("Trạng thái: -");
    ui->txtLog->clear();

    queueClear();
    draftClear();
    endedClear();

    ui->lblItemName->setText("-");
    ui->lblItemSeller->setText("-");
    ui->lblItemStart->setText("-");
    ui->lblItemBuyNow->setText("-");
    ui->lblItemPrice->setText("-");
    ui->lblItemLeader->setText("-");
    ui->lblItemTime->setText("-");
    ui->lblMinBid->setText("Giá tối thiểu: -");

    // chat reset
    if (ui->lstRoomUsers)
        ui->lstRoomUsers->clear();
    if (ui->lstChatUsers)
        ui->lstChatUsers->clear();
    if (ui->txtChat)
        ui->txtChat->clear();
    m_chatPeer.clear();
    if (ui->lblChatPeer)
        ui->lblChatPeer->setText("Đang chat với: -");
    if (ui->btnChatSend)
        ui->btnChatSend->setEnabled(false);

    //   important: do NOT recreate the bar here
    setRoomChatEnabled(false);
}

/* =======================
 * UI actions
 * ======================= */
void MainWindow::onConnectClicked()
{
    const QString host = ui->edtHost->text().trimmed();
    const quint16 port = (quint16)ui->spinPort->value();
    ui->lblConnStatus->setText("Trạng thái: Đang kết nối...");
    sock->connectTo(host, port);
}

void MainWindow::onDisconnectClicked()
{
    sock->disconnectFromHost();
}

void MainWindow::onBackToConnect()
{
    gotoPage(0);
}

void MainWindow::onLoginClicked()
{
    if (!sock->isConnected())
    {
        QMessageBox::warning(this, "Chưa kết nối", "Vui lòng kết nối tới máy chủ trước.");
        return;
    }
    const QString u = ui->edtUser->text().trimmed();
    const QString p = ui->edtPass->text();

    if (u.isEmpty() || p.isEmpty())
    {
        QMessageBox::warning(this, "Thiếu thông tin", "Cần nhập tên đăng nhập và mật khẩu.");
        return;
    }
    sock->sendLine(QString("LOGIN %1 %2").arg(u, p));
    ui->lblAuthStatus->setText("Trạng thái: Đang đăng nhập...");
}

void MainWindow::onRefreshRooms()
{
    roomsTableClear();
    ui->lblRoomsStatus->setText("Trạng thái: Đang tải danh sách phòng...");

    const bool myRooms = (ui->cmbRoomScope->currentIndex() == 1);
    sock->sendLine(myRooms ? "LIST_MY_ROOMS" : "LIST_ROOMS");
}

void MainWindow::onCreateRoom()
{
    const QString name = ui->edtCreateRoom->text().trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Thiếu thông tin", "Cần nhập tên phòng.");
        return;
    }
    QString safeName = name;
    safeName.replace(' ', '_');
    sock->sendLine(QString("CREATE_ROOM %1").arg(safeName));
}

void MainWindow::onJoinRoom()
{
    const QString rid = ui->edtJoinRoomId->text().trimmed();
    if (rid.isEmpty())
    {
        QMessageBox::warning(this, "Thiếu thông tin", "Cần nhập mã phòng hoặc tên phòng.");
        return;
    }
    sock->sendLine(QString("JOIN_ROOM %1").arg(rid));
}

void MainWindow::onJoinSelected()
{
    const auto items = ui->tblRooms->selectedItems();
    if (items.isEmpty())
    {
        QMessageBox::information(this, "Chưa chọn phòng", "Bạn hãy chọn 1 phòng trong danh sách.");
        return;
    }
    const int row = items.first()->row();

    auto get = [&](int col) -> QString
    {
        auto it = ui->tblRooms->item(row, col);
        return it ? it->text() : "";
    };

    const QString ridStr = get(0);
    const QString roomName = get(1);
    const QString ownerName = get(4).trimmed();

    if (ridStr.isEmpty())
    {
        QMessageBox::warning(this, "Lỗi", "Không lấy được ID phòng.");
        return;
    }

    // Lấy status code (ENDED/PENDING/STARTED)
    auto itSt = ui->tblRooms->item(row, 2);
    QString stCode = itSt ? itSt->data(Qt::UserRole).toString().trimmed() : QString();
    if (stCode.isEmpty())
        stCode = get(2).trimmed();

    const bool isOwner = !username.isEmpty() && ownerName.compare(username.trimmed(), Qt::CaseInsensitive) == 0;

    // Phòng đã kết thúc: owner mở chế độ xem (không JOIN) để xem end queue
    if (stCode == "ENDED" && isOwner)
    {
        bool okId = false;
        const int rid = ridStr.toInt(&okId);
        if (!okId || rid <= 0)
        {
            QMessageBox::warning(this, "Lỗi", "ID phòng không hợp lệ.");
            return;
        }

        // Bật chế độ view-only cho phòng đã kết thúc
        setProperty("ended_view_mode", true);
        currentRoomId = rid;
        currentItemId = 0;
        stopCountdown();

        // Disable các thao tác cần JOIN/đấu giá
        if (ui->btnLeaveRoom)
            ui->btnLeaveRoom->setText("Đóng");
        if (ui->btnStartRoom)
            ui->btnStartRoom->setEnabled(false);
        if (ui->btnEndRoom)
            ui->btnEndRoom->setEnabled(false);
        if (ui->btnViewQueued)
            ui->btnViewQueued->setEnabled(false);
        // ViewEnded vẫn bật để refresh kết quả
        if (ui->btnAddDraft)
            ui->btnAddDraft->setEnabled(false);
        if (ui->btnDraftList)
            ui->btnDraftList->setEnabled(false);
        if (ui->btnPublishOk)
            ui->btnPublishOk->setEnabled(false);
        if (ui->btnBid)
            ui->btnBid->setEnabled(false);
        if (ui->btnBuyNow)
            ui->btnBuyNow->setEnabled(false);
        if (ui->edtBidAmount)
            ui->edtBidAmount->setEnabled(false);
        if (ui->edtDraftName)
            ui->edtDraftName->setEnabled(false);
        if (ui->edtDraftStart)
            ui->edtDraftStart->setEnabled(false);
        if (ui->edtDraftBuy)
            ui->edtDraftBuy->setEnabled(false);
        if (ui->edtDraftDur)
            ui->edtDraftDur->setEnabled(false);

        // Reset UI phòng và mở tab phòng
        ui->txtLog->clear();
        queueClear();
        draftClear();
        endedClear();

        ui->lblRoomTitle->setText(QString("Phòng #%1: %2").arg(ridStr, roomName.isEmpty() ? "-" : roomName));
        setRoomHeader(statusToVi("ENDED"), 0);

        ui->lblItemName->setText("-");
        ui->lblItemSeller->setText("-");
        ui->lblItemStart->setText("-");
        ui->lblItemBuyNow->setText("-");
        ui->lblItemPrice->setText("-");
        ui->lblItemLeader->setText("-");
        ui->lblItemTime->setText("-");
        ui->lblMinBid->setText("Giá tối thiểu: -");

        setRoomChatEnabled(false);

        gotoPage(2);
        openRoomTab();

        // Load danh sách vật phẩm đã kết thúc của phòng này
        sock->sendLine(QString("ROOM_VIEW_ENDED %1").arg(rid));
        sock->sendLine(QString("ROOM_VIEW_CHAT %1 200").arg(rid)); // load last 200 public messages
        return;
    }

    // Phòng chưa kết thúc -> JOIN bình thường
    // Nếu trước đó đang ở chế độ xem ENDED (view-only) thì reset lại trước khi JOIN
    if (property("ended_view_mode").toBool())
    {
        closeRoomTab(true);
    }

    ui->edtJoinRoomId->setText(ridStr);
    onJoinRoom();
}

void MainWindow::onOpenHistoryTab()
{
    ui->tabHome->setCurrentIndex(1);
    onRefreshHistory();
}

void MainWindow::onRefreshHistory()
{
    historyClear();
    ui->lblHistoryStatus->setText("Trạng thái: Đang tải lịch sử...");
    sock->sendLine("HISTORY ME");
}

void MainWindow::onRoomFilterChanged()
{
    static int lastScope = -1;
    const int scope = ui->cmbRoomScope ? ui->cmbRoomScope->currentIndex() : 0;

    if (scope != lastScope)
    {
        lastScope = scope;
        onRefreshRooms();
        return;
    }
    applyRoomFilter();
}

void MainWindow::onRoomsSelectionChanged()
{
    const auto items = ui->tblRooms->selectedItems();
    if (items.isEmpty())
    {
        ui->lblSelRoomId->setText("-");
        ui->lblSelRoomName->setText("-");
        ui->lblSelRoomStatus->setText("-");
        ui->lblSelRoomParticipants->setText("-");
        ui->lblSelRoomOwner->setText("-");
        ui->btnJoinSelected->setEnabled(false);
        ui->btnJoinSelected->setText("Tham gia phòng đang chọn");

        m_selectedRoomId = 0;
        m_selectedRoomUsers.clear();
        if (ui->edtSelRoomUserSearch)
            ui->edtSelRoomUserSearch->clear();
        renderSelectedRoomUsers();
        return;
    }
    const int row = items.first()->row();

    auto get = [&](int col) -> QString
    {
        auto it = ui->tblRooms->item(row, col);
        return it ? it->text() : "-";
    };

    ui->lblSelRoomId->setText(get(0));
    ui->lblSelRoomName->setText(get(1));
    ui->lblSelRoomStatus->setText(get(2));
    ui->lblSelRoomParticipants->setText(get(3));
    ui->lblSelRoomOwner->setText(get(4));

    // Nếu status trên bảng đã được Việt hoá thì dùng "mã" gốc trong UserRole để quyết định join.
    auto itSt = ui->tblRooms->item(row, 2);
    QString stCode = itSt ? itSt->data(Qt::UserRole).toString().trimmed() : QString();
    if (stCode.isEmpty())
        stCode = get(2); // fallback

    const QString ownerName = get(4).trimmed();
    const bool isOwner = !username.isEmpty() && ownerName.compare(username.trimmed(), Qt::CaseInsensitive) == 0;

    // Phòng ENDED: chỉ owner mới được "xem" (không JOIN). Phòng khác: join bình thường.
    if (stCode == "ENDED")
    {
        ui->btnJoinSelected->setText("Xem phòng đã kết thúc");
        ui->btnJoinSelected->setEnabled(isOwner);
    }
    else
    {
        ui->btnJoinSelected->setText("Tham gia phòng đang chọn");
        ui->btnJoinSelected->setEnabled(true);
    }

    bool okId = false;
    const int rid = get(0).toInt(&okId);
    m_selectedRoomId = okId ? rid : 0;
    requestSelectedRoomUsers(m_selectedRoomId);
}

void MainWindow::onSelRoomUserSearchChanged()
{
    renderSelectedRoomUsers();
}

void MainWindow::onSelRoomUserDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    const QString peer = item->text().trimmed();
    if (peer.isEmpty())
        return;
    openChatWith(peer, true);
}

void MainWindow::onHistoryFilterChanged() { applyHistoryFilter(); }

void MainWindow::onLogout()
{
    // (Tuỳ chọn) báo server logout nếu bạn có cmd này
    if (sock && sock->isConnected())
    {
        sock->sendLine("LOGOUT");
    }

    // ===== Reset state/UI như bạn đang làm =====
    username.clear();
    role.clear();
    m_hasSession = false;

    closeRoomTab(false);

    ui->lblAuthStatus->setText("Trạng thái: Chưa đăng nhập");
    ui->lblRoomsStatus->setText("Trạng thái: -");
    ui->lblUserInfo->setText("Xin chào: - (vai trò: -)");
    roomsTableClear();

    ui->lblSelRoomId->setText("-");
    ui->lblSelRoomName->setText("-");
    ui->lblSelRoomStatus->setText("-");
    ui->lblSelRoomParticipants->setText("-");
    ui->lblSelRoomOwner->setText("-");
    ui->btnJoinSelected->setEnabled(false);

    ui->tabHome->setCurrentIndex(0);

    // ===== Quan trọng: tạm ngắt nhận line để AuthWindow xử lý login =====
    if (sock)
    {
        disconnect(sock, &SocketClient::lineReceived, this, &MainWindow::onLine);
    }

    // Ẩn MainWindow và mở lại AuthWindow
    this->hide();

    AuthWindow auth(sock, nullptr);
    const int r = auth.exec();

    // Nối lại lineReceived cho MainWindow
    if (sock)
    {
        connect(sock, &SocketClient::lineReceived, this, &MainWindow::onLine, Qt::UniqueConnection);
    }

    if (r == QDialog::Accepted)
    {
        // Login lại thành công -> vào lại main
        setSession(auth.loggedUser(), auth.loggedRole());
        this->show();
        this->raise();
        this->activateWindow();
    }
    else
    {
        // User đóng auth -> thoát app
        this->close();
    }
}

void MainWindow::onLeaveRoom()
{
    // Nếu đang xem phòng đã kết thúc (không JOIN), chỉ cần đóng tab phòng
    if (property("ended_view_mode").toBool())
    {
        closeRoomTab(true);
        gotoPage(2);
        return;
    }

    sock->sendLine("LEAVE_ROOM");
    closeRoomTab(true);
    gotoPage(2);
}

void MainWindow::onStartRoom() { sock->sendLine("START"); }
void MainWindow::onEndRoom() { sock->sendLine("END"); }

void MainWindow::onViewQueued()
{
    queueClear();
    sock->sendLine("VIEW QUEUED");
}

void MainWindow::onViewEnded()
{
    endedClear();

    // Nếu đang xem phòng đã kết thúc (không JOIN) -> lấy danh sách ended theo room_id
    if (property("ended_view_mode").toBool())
    {
        if (currentRoomId <= 0)
            return;
        sock->sendLine(QString("ROOM_VIEW_ENDED %1").arg(currentRoomId));
        return;
    }

    sock->sendLine("VIEW ENDED");
}

/* =======================
 * Seller draft
 * ======================= */
// =======================
// Draft input validation helpers (Seller)
// =======================
void MainWindow::clearDraftInputErrors()
{
    auto resetOne = [](QLineEdit* w)
    {
        if (!w) return;
        const QVariant base = w->property("baseStyle");
        if (base.isValid())
            w->setStyleSheet(base.toString());
        w->setToolTip(QString());
    };

    resetOne(ui->edtDraftName);
    resetOne(ui->edtDraftStart);
    resetOne(ui->edtDraftBuy);
    resetOne(ui->edtDraftDur);
}

void MainWindow::markInputInvalid(QLineEdit* w, const QString& msg)
{
    if (!w) return;

    if (!w->property("baseStyle").isValid())
        w->setProperty("baseStyle", w->styleSheet());

    const QString base = w->property("baseStyle").toString();
    // Chỉ highlight nhẹ, không thay đổi layout/logic khác
    w->setStyleSheet(base + "border:2px solid #d32f2f; border-radius:6px;");
    w->setToolTip(msg);
}

bool MainWindow::validateDraftInputs(QString& outName, int& outStart, int& outBuy, int& outDur)
{
    clearDraftInputErrors();

    QStringList errors;
    QLineEdit* firstInvalid = nullptr;

    auto addErr = [&](QLineEdit* w, const QString& msg)
    {
        errors << QStringLiteral("• ") + msg;
        markInputInvalid(w, msg);
        if (!firstInvalid) firstInvalid = w;
    };

    // name
    outName = ui->edtDraftName ? ui->edtDraftName->text().trimmed() : QString();
    if (outName.isEmpty())
        addErr(ui->edtDraftName, QStringLiteral("Tên vật phẩm không được để trống."));
    else
        outName.replace(' ', '_'); // protocol friendly

    // start price
    const QString startStr = ui->edtDraftStart ? ui->edtDraftStart->text().trimmed() : QString();
    bool ok = false;
    outStart = startStr.toInt(&ok);
    if (startStr.isEmpty())
        addErr(ui->edtDraftStart, QStringLiteral("Giá khởi điểm không được để trống."));
    else if (!ok)
        addErr(ui->edtDraftStart, QStringLiteral("Giá khởi điểm phải là số nguyên."));
    else if (outStart <= 0)
        addErr(ui->edtDraftStart, QStringLiteral("Giá khởi điểm phải lớn hơn 0."));

    // duration
    const int kMinDurationSec = 40; // yêu cầu của bạn
    const QString durStr = ui->edtDraftDur ? ui->edtDraftDur->text().trimmed() : QString();
    outDur = durStr.toInt(&ok);
    if (durStr.isEmpty())
        addErr(ui->edtDraftDur, QStringLiteral("Thời lượng không được để trống."));
    else if (!ok)
        addErr(ui->edtDraftDur, QStringLiteral("Thời lượng phải là số nguyên (giây)."));
    else if (outDur < kMinDurationSec)
        addErr(ui->edtDraftDur, QStringLiteral("Thời lượng phải tối thiểu %1 giây.").arg(kMinDurationSec));

    // buy-now (optional)
    const QString buyStrRaw = ui->edtDraftBuy ? ui->edtDraftBuy->text().trimmed() : QString();
    const QString buyStr = buyStrRaw.isEmpty() ? QStringLiteral("0") : buyStrRaw;
    outBuy = buyStr.toInt(&ok);
    if (!ok)
        addErr(ui->edtDraftBuy, QStringLiteral("Giá mua ngay phải là số nguyên."));
    else if (outBuy < 0)
        addErr(ui->edtDraftBuy, QStringLiteral("Giá mua ngay không được âm."));
    else if (outBuy != 0 && outStart > 0 && outBuy < 2 * outStart)
        addErr(ui->edtDraftBuy, QStringLiteral("Giá mua ngay phải bằng 0 hoặc tối thiểu gấp đôi giá khởi điểm."));

    if (!errors.isEmpty())
    {
        QMessageBox::warning(this,
                             QStringLiteral("Thông tin chưa hợp lệ"),
                             QStringLiteral("Vui lòng kiểm tra:\n") + errors.join('\n'));

        if (firstInvalid)
        {
            firstInvalid->setFocus();
            firstInvalid->selectAll();
        }
        return false;
    }

    return true;
}

void MainWindow::onAddDraft()
{
    QString name;
    int start = 0, buy = 0, dur = 0;

    if (!validateDraftInputs(name, start, buy, dur))
        return;

    sock->sendLine(QString("ADD_ITEM %1|%2|%3|%4").arg(name).arg(start).arg(buy).arg(dur));

    // Clear inputs after sending
    if (ui->edtDraftName) ui->edtDraftName->clear();
    if (ui->edtDraftStart) ui->edtDraftStart->clear();
    if (ui->edtDraftBuy) ui->edtDraftBuy->clear();
    if (ui->edtDraftDur) ui->edtDraftDur->clear();

    clearDraftInputErrors();
}

void MainWindow::onDraftList()
{
    draftClear();
    sock->sendLine("DRAFT_LIST");
}

void MainWindow::onPublishOk() { sock->sendLine("OK"); }

/* =======================
 * Buyer actions
 * ======================= */
void MainWindow::onBid()
{
    if (currentItemId == 0)
    {
        QMessageBox::warning(this, "Không có vật phẩm", "Hiện không có vật phẩm đang đấu giá để đặt giá.");
        return;
    }
    const QString amt = ui->edtBidAmount->text().trimmed();
    if (amt.isEmpty())
    {
        QMessageBox::warning(this, "Thiếu thông tin", "Cần nhập số tiền đặt giá.");
        return;
    }
    sock->sendLine(QString("BID %1 %2").arg(currentItemId).arg(amt));
    ui->edtBidAmount->clear();
}

void MainWindow::onBuyNow()
{
    if (currentItemId == 0)
    {
        QMessageBox::warning(this, "Không có vật phẩm", "Hiện không có vật phẩm đang đấu giá để mua ngay.");
        return;
    }
    sock->sendLine(QString("BUY_NOW %1").arg(currentItemId));
}

/* =======================
 * Auto-bid (server-side)
 * ======================= */
void MainWindow::onAutoBidApply()
{
    if (!sock || !sock->isConnected())
    {
        QMessageBox::warning(this, QStringLiteral("Chưa kết nối"), QStringLiteral("Bạn chưa kết nối tới máy chủ."));
        return;
    }

    if (currentItemId == 0)
    {
        QMessageBox::warning(this, QStringLiteral("Không có vật phẩm"),
                             QStringLiteral("Hiện không có vật phẩm đang đấu giá để bật Auto-bid."));
        return;
    }

    // Clear previous input highlight (reuse helper style property)
    if (ui->edtAutoBidMax)
    {
        const QVariant base = ui->edtAutoBidMax->property("baseStyle");
        if (base.isValid())
            ui->edtAutoBidMax->setStyleSheet(base.toString());
        ui->edtAutoBidMax->setToolTip(QString());
    }

    const QString maxStr = ui->edtAutoBidMax ? ui->edtAutoBidMax->text().trimmed() : QString();
    bool ok = false;
    const int maxPrice = maxStr.toInt(&ok);

    if (maxStr.isEmpty())
    {
        markInputInvalid(ui->edtAutoBidMax, QStringLiteral("Vui lòng nhập giá tối đa."));
        QMessageBox::warning(this, QStringLiteral("Thiếu thông tin"), QStringLiteral("Vui lòng nhập giá tối đa."));
        return;
    }
    if (!ok || maxPrice <= 0)
    {
        markInputInvalid(ui->edtAutoBidMax, QStringLiteral("Giá tối đa phải là số nguyên > 0."));
        QMessageBox::warning(this, QStringLiteral("Không hợp lệ"), QStringLiteral("Giá tối đa phải là số nguyên > 0."));
        return;
    }

    // Ask server to enable Auto-bid for the current running item
    sock->sendLine(QStringLiteral("AUTO_BID %1").arg(maxPrice));

    if (ui->chkAutoBid)
        ui->chkAutoBid->setChecked(true);

    statusBar()->showMessage(QStringLiteral("Đã gửi yêu cầu bật Auto-bid."), 2500);
}

void MainWindow::onAutoBidOff()
{
    if (!sock || !sock->isConnected())
    {
        QMessageBox::warning(this, QStringLiteral("Chưa kết nối"), QStringLiteral("Bạn chưa kết nối tới máy chủ."));
        return;
    }

    sock->sendLine(QStringLiteral("AUTO_BID_OFF"));

    if (ui->chkAutoBid)
        ui->chkAutoBid->setChecked(false);

    statusBar()->showMessage(QStringLiteral("Đã tắt Auto-bid."), 2500);
}


/* =======================
 * Socket events
 * ======================= */
void MainWindow::onSockConnected()
{
    ui->lblConnStatus->setText("Trạng thái: Đã kết nối");
    ui->btnDisconnect->setEnabled(true);
    statusBar()->showMessage("Đã kết nối.");

    if (m_hasSession)
    {
        gotoPage(2);
        return;
    }
    gotoPage(1);
}

void MainWindow::onSockDisconnected()
{
    ui->lblConnStatus->setText("Trạng thái: Chưa kết nối");
    ui->btnDisconnect->setEnabled(false);
    statusBar()->showMessage("Đã ngắt kết nối.");

    username.clear();
    role.clear();
    m_hasSession = false;
    closeRoomTab(false);

    gotoPage(0);
}

void MainWindow::onSockError(const QString &msg)
{
    const QString m = msg.trimmed();

    // "Chưa kết nối." thường xảy ra khi người dùng thao tác nhanh hoặc vừa logout,
    // không nên spam popup.
    const bool popup = (m != QStringLiteral("Chưa kết nối."));
    showNotice(QStringLiteral("SOCK:") + m,
               QStringLiteral("Lỗi kết nối"),
               QStringLiteral("Lỗi kết nối: %1").arg(m),
               popup);
}

void MainWindow::onLine(const QString &line)
{
    const QStringList tok = splitWs(line);
    if (tok.isEmpty())
        return;

    if (tok[0] == "OK")
        handleOK(tok, line);
    else if (tok[0] == "ERR")
        handleERR(tok, line);
    else if (tok[0] == "EVT")
        handleEVT(tok, line);
}

/* =======================
 * Table helpers
 * ======================= */
void MainWindow::roomsTableClear() { ui->tblRooms->setRowCount(0); }

void MainWindow::roomsTableAddRow(const QStringList &cols)
{
    int r = ui->tblRooms->rowCount();
    ui->tblRooms->insertRow(r);

    for (int c = 0; c < cols.size() && c < ui->tblRooms->columnCount(); ++c)
    {
        // Cột 2: status (PENDING/STARTED/ENDED) -> hiển thị tiếng Việt nhưng vẫn lưu "code" để lọc/logic.
        if (c == 2)
        {
            const QString code = cols[c].trimmed();
            auto *it = new QTableWidgetItem(statusToVi(code));
            it->setData(Qt::UserRole, code); // lưu mã gốc
            ui->tblRooms->setItem(r, c, it);
        }
        else
        {
            ui->tblRooms->setItem(r, c, new QTableWidgetItem(cols[c]));
        }
    }

    applyRoomFilter();
}

void MainWindow::historyClear() { ui->tblHistory->setRowCount(0); }

void MainWindow::historyAddRow(const QStringList &cols)
{
    int r = ui->tblHistory->rowCount();
    ui->tblHistory->insertRow(r);
    for (int c = 0; c < cols.size() && c < ui->tblHistory->columnCount(); ++c)
    {
        ui->tblHistory->setItem(r, c, new QTableWidgetItem(cols[c]));
    }
    applyHistoryFilter();
}

void MainWindow::queueClear() { ui->tblQueue->setRowCount(0); }
void MainWindow::queueAddRow(const QStringList &cols)
{
    int r = ui->tblQueue->rowCount();
    ui->tblQueue->insertRow(r);
    for (int c = 0; c < cols.size() && c < ui->tblQueue->columnCount(); ++c)
    {
        ui->tblQueue->setItem(r, c, new QTableWidgetItem(cols[c]));
    }
}

void MainWindow::draftClear() { ui->tblDraft->setRowCount(0); }
void MainWindow::draftAddRow(const QStringList &cols)
{
    int r = ui->tblDraft->rowCount();
    ui->tblDraft->insertRow(r);

    // Fill data columns (DraftID, Name, Start, BuyNow, Dur)
    const int dataCols = qMin(cols.size(), 5);
    for (int c = 0; c < dataCols; ++c)
    {
        ui->tblDraft->setItem(r, c, new QTableWidgetItem(cols[c]));
    }

    // Add Remove button (col 5)
    if (ui->tblDraft->columnCount() >= 6)
    {
        const QString did = cols.value(0).trimmed();
        auto *btn = new QPushButton("Xóa", ui->tblDraft);
        btn->setProperty("draft_id", did);
        btn->setToolTip("Xóa draft item này");
        connect(btn, &QPushButton::clicked, this, [this, did]()
                {
            if (!sock || !sock->isConnected()) {
                QMessageBox::warning(this, "Chưa kết nối", "Socket chưa được kết nối.");
                return;
            }
            if (did.isEmpty()) return;
            sock->sendLine(QString("REMOVE_ITEM %1").arg(did)); });
        ui->tblDraft->setCellWidget(r, 5, btn);
    }
}

void MainWindow::draftRemoveById(const QString &draftId)
{
    if (!ui->tblDraft)
        return;
    for (int r = 0; r < ui->tblDraft->rowCount(); ++r)
    {
        auto *it = ui->tblDraft->item(r, 0);
        if (it && it->text().trimmed() == draftId.trimmed())
        {
            ui->tblDraft->removeRow(r);
            return;
        }
    }
}

void MainWindow::endedClear() { ui->tblEnded->setRowCount(0); }
void MainWindow::endedAddRow(const QStringList &cols)
{
    int r = ui->tblEnded->rowCount();
    ui->tblEnded->insertRow(r);

    for (int c = 0; c < cols.size() && c < ui->tblEnded->columnCount(); ++c)
    {
        // Cột 1: status -> hiển thị tiếng Việt
        if (c == 1)
        {
            const QString code = cols[c].trimmed();
            auto *it = new QTableWidgetItem(statusToVi(code));
            it->setData(Qt::UserRole, code);
            ui->tblEnded->setItem(r, c, it);
        }
        else
        {
            ui->tblEnded->setItem(r, c, new QTableWidgetItem(cols[c]));
        }
    }
}

/* =======================
 * Filtering
 * ======================= */
void MainWindow::applyRoomFilter()
{
    if (!ui->tblRooms)
        return;

    const QString q = ui->edtRoomSearch->text().trimmed().toLower();
    const int scope = ui->cmbRoomScope->currentIndex(); // 0 all, 1 my
    const int statusIdx = ui->cmbRoomStatus ? ui->cmbRoomStatus->currentIndex() : 0;
    const QString statusWanted = comboStatusWantedCode(statusIdx);

    for (int r = 0; r < ui->tblRooms->rowCount(); ++r)
    {
        auto itId = ui->tblRooms->item(r, 0);
        auto itName = ui->tblRooms->item(r, 1);
        auto itSt = ui->tblRooms->item(r, 2);
        auto itP = ui->tblRooms->item(r, 3);
        auto itOw = ui->tblRooms->item(r, 4);

        const QString id = itId ? itId->text() : "";
        const QString name = itName ? itName->text() : "";
        const QString stDisp = itSt ? itSt->text() : "";
        QString stCode;
        if (itSt)
        {
            stCode = itSt->data(Qt::UserRole).toString();
            if (stCode.isEmpty())
                stCode = stDisp;
        }
        const QString p = itP ? itP->text() : "";
        const QString ow = itOw ? itOw->text() : "";

        const QString userNorm = username.trimmed();
        const QString ownerNorm = ow.trimmed();

        bool ok = true;

        // scope == 0: homepage chỉ hiện phòng của seller khác + ẩn ENDED
        if (scope == 0)
        {
            if (stCode == "ENDED")
                ok = false;
            if (!userNorm.isEmpty() && ownerNorm.compare(userNorm, Qt::CaseInsensitive) == 0)
                ok = false;
        }

        // scope == 1: phòng của tôi -> chỉ owner = username (bao gồm ENDED)
        if (scope == 1 && !username.isEmpty())
        {
            if (ownerNorm.compare(userNorm, Qt::CaseInsensitive) != 0)
                ok = false;
        }

        if (!statusWanted.isEmpty())
        {
            if (stCode != statusWanted)
                ok = false;
        }

        if (!q.isEmpty())
        {
            const QString hay = (id + " " + name + " " + stDisp + " " + stCode + " " + p + " " + ow).toLower();
            if (!hay.contains(q))
                ok = false;
        }

        ui->tblRooms->setRowHidden(r, !ok);
    }
}

void MainWindow::applyHistoryFilter()
{
    if (!ui->tblHistory)
        return;
    const QString q = ui->edtHistSearch->text().trimmed().toLower();
    for (int r = 0; r < ui->tblHistory->rowCount(); ++r)
    {
        QString rowText;
        for (int c = 0; c < ui->tblHistory->columnCount(); ++c)
        {
            auto it = ui->tblHistory->item(r, c);
            if (it)
                rowText += it->text() + " ";
        }
        const bool ok = q.isEmpty() || rowText.toLower().contains(q);
        ui->tblHistory->setRowHidden(r, !ok);
    }
}

/* =======================
 * Protocol handlers
 * ======================= */
void MainWindow::handleOK(const QStringList &tok, const QString &raw)
{
    if (tok.size() >= 2 && tok[1] == "LOGIN")
    {
        if (tok.size() >= 4)
        {
            role = tok[3];
            username = ui->edtUser->text().trimmed();
            m_hasSession = !username.isEmpty();

            ui->lblAuthStatus->setText(QString("Trạng thái: Đã đăng nhập: %1 (%2)").arg(username, role));
            ui->lblUserInfo->setText(QString("Xin chào: %1 (vai trò: %2)").arg(username, role));

            const bool isSeller = (role == "SELLER" || role == "BOTH");
            ui->grpDraft->setVisible(isSeller);

            gotoPage(2);
            ui->tabHome->setCurrentIndex(0);
            onRefreshRooms();
        }
        return;
    }

    if (tok.size() >= 2 && tok[1] == "ROOM_JOINED")
    {
        ui->txtLog->clear();
        queueClear();
        draftClear();
        endedClear();
        currentItemId = 0;
        ui->lblItemName->setText("Đang tải...");
        ui->lblRoomStatus->setText("Trạng thái: Đang vào phòng...");
        return;
    }

    if (tok.size() >= 2 && tok[1] == "ROOM_LEFT")
    {
        ui->lblRoomsStatus->setText("Trạng thái: Đã rời phòng.");
        closeRoomTab(true);
        return;
    }

    if (tok.size() >= 2 && tok[1] == "ROOM_CREATED")
    {
        ui->lblRoomsStatus->setText("Trạng thái: Đã tạo phòng.");
        if (ui->edtCreateRoom)
            ui->edtCreateRoom->clear();

        // Note: "Tất cả phòng" intentionally hides rooms owned by the current user.
        // After creating a room, switch to "Phòng của tôi" so the new room is visible.
        if (ui->cmbRoomScope && ui->cmbRoomScope->currentIndex() == 0)
        {
            ui->cmbRoomScope->setCurrentIndex(1); // triggers refresh via onRoomFilterChanged()
        }
        else
        {
            onRefreshRooms();
        }
        return;
    }

    if (tok.size() >= 2 && tok[1] == "ROOMS")
    {
        ui->lblRoomsStatus->setText("Trạng thái: Đã tải danh sách phòng.");
        applyRoomFilter();
        return;
    }

    if (tok.size() >= 2 && tok[1] == "DRAFT_REMOVED")
    {
        const QString did = tok.value(2).trimmed();
        if (!did.isEmpty())
        {
            draftRemoveById(did);
            statusBar()->showMessage(QString("Đã xóa nháp: %1").arg(did), 1500);
        }
        return;
    }

    statusBar()->showMessage("Thao tác thành công.", 2000);
}

void MainWindow::handleERR(const QStringList &tok0, const QString &raw)
{
    // Use provided tokens if possible (avoid splitting twice)
    const QStringList tok = tok0.isEmpty() ? splitWs(raw) : tok0;

    const QString code = tok.value(1).trimmed().toUpper();
    const QString detail = (tok.size() >= 3) ? joinTokens(tok, 2).trimmed() : QString();

    const QString msg = translateServerErr(tok, raw);

// Nếu lỗi là validate draft, highlight đúng ô input để người dùng thấy rõ
if (code == "INVALID" || code == "INVALID_PRICE" || code == "INVALID_ITEM_FORMAT")
{
    const QString d = detail.toLower();

    if (d.contains("name"))
        markInputInvalid(ui->edtDraftName, msg);
    if (d.contains("start"))
        markInputInvalid(ui->edtDraftStart, msg);
    if (d.contains("buy_now"))
        markInputInvalid(ui->edtDraftBuy, msg);
    if (d.contains("duration"))
        markInputInvalid(ui->edtDraftDur, msg);
}


    // ===== Dedup key strategy =====
    // - For most server codes, we dedup by CODE only (avoid double popups when BE sends similar messages twice).
    // - For input/validation errors, keep detail in key so user still sees different reasons.
    QString key;
    if (!code.isEmpty())
    {
        key = QStringLiteral("ERR:") + code;

        if (code == "FORBIDDEN")
        {
            const QString d = detail.toLower();
            if (d.contains("seller") && d.contains("buy"))
            {
                key = QStringLiteral("ERR:FORBIDDEN:SELLER_BUY_NOW");
            }
        }

        if (code == "INVALID" || code == "INVALID_PRICE" || code == "INVALID_ITEM_FORMAT" || code == "BID_TOO_LOW")
        {
            key += QStringLiteral(":") + detail;
        }
    }
    else
    {
        key = QStringLiteral("ERR:RAW:") + raw;
    }

    showNotice(key, QStringLiteral("Thông báo"), msg, true);
}

void MainWindow::handleEVT(const QStringList &tok, const QString &raw)
{
    if (tok.size() < 2)
        return;
    const QString ev = tok[1];

    if (ev == "ROOMS_CHANGED")
    {
        if (m_hasSession)
            onRefreshRooms();
        return;
    }

    if (ev == "ROOM")
    {
        if (tok.size() >= 6)
        {
            const QString ownerName = (tok.size() >= 8) ? tok[7] : (tok.size() >= 7 ? tok[6] : "-");
            roomsTableAddRow({tok.value(2), tok.value(3), tok.value(4), tok.value(5), ownerName});
        }
        return;
    }
    if (ev == "ROOMS_END")
    {
        ui->lblRoomsStatus->setText("Trạng thái: Đã tải danh sách phòng.");
        return;
    }

    if (ev == "ROOM_USERS")
    {
        bool okId = false;
        const int rid = tok.value(2).toInt(&okId);
        if (!okId)
            return;
        if (rid != m_selectedRoomId)
            return;

        QStringList users;
        for (int i = 4; i < tok.size(); ++i)
        {
            const QString u = tok[i].trimmed();
            if (!u.isEmpty())
                users << u;
        }
        m_selectedRoomUsers = users;
        renderSelectedRoomUsers();
        return;
    }

    if (ev == "HIST")
    {
        if (tok.size() >= 10)
        {
            historyAddRow({tok[9], tok[3], tok[2], tok[4], tok[5], tok[6], tok[7], tok[8]});
        }
        return;
    }
    if (ev == "HISTORY_END")
    {
        ui->lblHistoryStatus->setText("Trạng thái: Đã tải lịch sử.");
        return;
    }

    if (ev == "ROOM_SNAPSHOT")
    {
        if (tok.size() >= 6)
        {
            currentRoomId = tok[2].toInt();
            ui->lblRoomTitle->setText(QString("Phòng #%1: %2").arg(tok[2], tok[3]));
            bool okP = false;
            const int p = tok[5].toInt(&okP);
            setRoomHeader(tok[4], okP ? p : -1);
            queueClear();
            draftClear();
            endedClear();
        }
        return;
    }

    if (ev == "ROOM_STARTED")
    {
        setRoomHeader(statusToVi("STARTED"), m_roomParticipants);
        return;
    }

    if (ev == "ROOM_PENDING")
    {
        setRoomHeader(statusToVi("PENDING"), m_roomParticipants);
        stopCountdown();
        return;
    }

    if (ev == "ROOM_ENDED")
    {
        setRoomHeader(statusToVi("ENDED"), m_roomParticipants);
        stopCountdown();
        setRoomChatEnabled(false);
        closeRoomTab(true);
        onRefreshRooms();
        return;
    }

    if (ev == "SNAPSHOT_END")
    {
        // Vào phòng thành công -> đảm bảo thoát chế độ view-only của ENDED rooms
        setProperty("ended_view_mode", false);
        if (ui->btnLeaveRoom)
            ui->btnLeaveRoom->setText("Rời phòng");
        if (ui->btnStartRoom)
            ui->btnStartRoom->setEnabled(true);
        if (ui->btnEndRoom)
            ui->btnEndRoom->setEnabled(true);
        if (ui->btnViewQueued)
            ui->btnViewQueued->setEnabled(true);
        if (ui->btnViewEnded)
            ui->btnViewEnded->setEnabled(true);
        if (ui->btnAddDraft)
            ui->btnAddDraft->setEnabled(true);
        if (ui->btnDraftList)
            ui->btnDraftList->setEnabled(true);
        if (ui->btnPublishOk)
            ui->btnPublishOk->setEnabled(true);
        if (ui->btnBid)
            ui->btnBid->setEnabled(true);
        if (ui->btnBuyNow)
            ui->btnBuyNow->setEnabled(true);
        if (ui->edtBidAmount)
            ui->edtBidAmount->setEnabled(true);
        if (ui->edtDraftName)
            ui->edtDraftName->setEnabled(true);
        if (ui->edtDraftStart)
            ui->edtDraftStart->setEnabled(true);
        if (ui->edtDraftBuy)
            ui->edtDraftBuy->setEnabled(true);
        if (ui->edtDraftDur)
            ui->edtDraftDur->setEnabled(true);

        gotoPage(2);
        openRoomTab();
        onRefreshChatUsers();
        setRoomChatEnabled(true); // vào phòng mới cho chat phòng
        return;
    }

    if (ev == "CURRENT")
    {
        if (tok.size() >= 3 && tok[2] == "NONE")
        {
            currentItemId = 0;
            ui->lblItemName->setText("Không có vật phẩm");
            ui->lblItemSeller->setText("-");
            ui->lblItemStart->setText("-");
            ui->lblItemBuyNow->setText("-");
            ui->lblItemPrice->setText("-");
            ui->lblItemLeader->setText("-");
            ui->lblItemTime->setText("-");
            stopCountdown();
            ui->lblMinBid->setText("Giá tối thiểu: -");
            return;
        }
        if (tok.size() >= 10)
        {
            currentItemId = tok[2].toInt();
            ui->lblItemName->setText(QString("#%1 %2").arg(tok[2], tok[3]));
            ui->lblItemSeller->setText(tok[4]);
            ui->lblItemStart->setText(tok[5]);
            ui->lblItemBuyNow->setText(tok[6]);
            ui->lblItemPrice->setText(tok[7]);
            ui->lblItemLeader->setText(tok[8]);
            syncCountdownFromMmss(tok[9]);

            bool ok = false;
            long long cur = tok[7].toLongLong(&ok);
            ui->lblMinBid->setText(ok ? QString("Giá tối thiểu: %1").arg(cur + 10000) : "Giá tối thiểu: -");
        }
        return;
    }

    if (ev == "ITEM_START")
    {
        // EVT ITEM_START <id> <name> <seller> <start> <buy_now> <current> <leader> <mm:ss>
        if (tok.size() >= 10)
        {
            currentItemId = tok[2].toInt();
            ui->lblItemName->setText(QString("#%1 %2").arg(tok[2], tok[3]));
            ui->lblItemSeller->setText(tok[4]);
            ui->lblItemStart->setText(tok[5]);
            ui->lblItemBuyNow->setText(tok[6]);
            ui->lblItemPrice->setText(tok[7]);
            ui->lblItemLeader->setText(tok[8]);
            syncCountdownFromMmss(tok[9]);

            bool ok = false;
            const long long cur = tok[7].toLongLong(&ok);
            ui->lblMinBid->setText(ok ? QString("Giá tối thiểu: %1").arg(cur + 10000) : "Giá tối thiểu: -");
        }
        return;
    }

    if (ev == "PRICE_UPDATE")
    {
        // EVT PRICE_UPDATE <id> <price> <leader> <mm:ss>
        if (tok.size() >= 6)
        {
            bool okId = false;
            const int iid = tok[2].toInt(&okId);
            if (okId && iid == currentItemId)
            {
                ui->lblItemPrice->setText(tok[3]);
                ui->lblItemLeader->setText(tok[4]);
                syncCountdownFromMmss(tok[5]);

                bool okPrice = false;
                const long long cur = tok[3].toLongLong(&okPrice);
                ui->lblMinBid->setText(okPrice ? QString("Giá tối thiểu: %1").arg(cur + 10000) : "Giá tối thiểu: -");
            }
        }
        return;
    }

    if (ev == "ALERT_30S")
    {
        // EVT ALERT_30S <item_id> <mm:ss>
        if (tok.size() >= 4)
        {
            bool okId = false;
            const int iid = tok[2].toInt(&okId);
            if (okId && iid == currentItemId)
                syncCountdownFromMmss(tok[3]);
        }
        return;
    }

    if (ev == "ITEM_END" || ev == "SOLD" || ev == "EXPIRED")
    {
        stopCountdown();
        return;
    }

    if (ev == "QUEUED")
    {
        if (tok.size() >= 7)
            queueAddRow({tok[2], tok[3], tok[4], tok[5], tok[6]});
        return;
    }
    if (ev == "QUEUED_END")
    {
        statusBar()->showMessage("Đã tải danh sách hàng đợi.", 1500);
        return;
    }

    if (ev == "DRAFT_ITEM" || ev == "DRAFT")
    {
        if (tok.size() >= 7)
            draftAddRow({tok[2], tok[3], tok[4], tok[5], tok[6]});
        return;
    }
    if (ev == "PUBLISHED")
    {
        statusBar()->showMessage(QString("Đã đăng lên %1 vật phẩm.").arg(tok.value(3)), 2000);
        draftClear();
        return;
    }

    if (ev == "ENDED")
    {
        if (tok.size() >= 8)
            endedAddRow({tok[2], tok[3], tok[4], tok[5], tok[6], tok[7]});
        return;
    }
    if (ev == "ENDED_END")
    {
        statusBar()->showMessage("Đã tải danh sách vật phẩm đã kết thúc.", 1500);
        return;
    }

    /* =======================
     * Chat events
     * ======================= */
    if (ev == "CHAT_USERS")
    {
        if (ui->lstRoomUsers)
            ui->lstRoomUsers->clear();
        if (ui->lstChatUsers)
            ui->lstChatUsers->clear();

        int startIdx = 2;
        bool okN = false;
        int n = tok.value(2).toInt(&okN);
        if (okN)
            startIdx = 3;

        QStringList users;
        for (int i = startIdx; i < tok.size(); ++i)
        {
            const QString u = tok[i].trimmed();
            if (!u.isEmpty() && u != username)
                users << u;
        }
        users.removeDuplicates();
        users.sort(Qt::CaseInsensitive);

        // Update participants in Room tab
        if (currentRoomId != 0)
        {
            if (okN)
                setRoomParticipants(n);
            else if (!username.isEmpty())
                setRoomParticipants(users.size() + 1);
        }

        if (ui->lstRoomUsers)
            ui->lstRoomUsers->addItems(users);
        if (ui->lstChatUsers)
            ui->lstChatUsers->addItems(users);

        if (!m_chatPeer.isEmpty() && !users.contains(m_chatPeer, Qt::CaseInsensitive))
        {
            m_chatPeer.clear();
            if (ui->lblChatPeer)
                ui->lblChatPeer->setText("Đang chat với: -");
            if (ui->btnChatSend)
                ui->btnChatSend->setEnabled(false);
            if (ui->txtChat)
                ui->txtChat->clear();
        }
        return;
    }

    if (ev == "CHAT_FROM")
    {
        const QString prefix = "EVT CHAT_FROM ";
        QString payload = raw;
        if (payload.startsWith(prefix))
            payload = payload.mid(prefix.size());
        const int bar = payload.indexOf('|');
        if (bar < 0)
            return;

        const QString from = payload.left(bar).trimmed();
        const QString msg = payload.mid(bar + 1).trimmed();

        if (!from.isEmpty() && !msg.isEmpty())
        {
            appendChatLine(from, QString("%1: %2").arg(from, msg));
        }
        return;
    }

    if (ev == "CHAT_ROOM")
    {
        const QString prefix = "EVT CHAT_ROOM ";
        QString payload = raw;
        if (payload.startsWith(prefix))
            payload = payload.mid(prefix.size());
        const int bar = payload.indexOf('|');
        if (bar < 0)
            return;

        const QString from = payload.left(bar).trimmed();
        const QString msg = payload.mid(bar + 1).trimmed();
        if (!from.isEmpty() && !msg.isEmpty())
        {
            const QString who = (from.compare(username, Qt::CaseInsensitive) == 0) ? "Tôi" : from;
            logLine(QString("%1: %2").arg(who, msg));
        }
        return;
    }

    // History of public room chat (loaded for ENDED room view)
    if (ev == "ROOM_CHAT")
    {
        // EVT ROOM_CHAT <ts> <from>|<msg...>
        const QString prefix = "EVT ROOM_CHAT ";
        QString payload = raw;
        if (payload.startsWith(prefix))
            payload = payload.mid(prefix.size());

        const int sp = payload.indexOf(' ');
        if (sp < 0)
            return;

        bool okTs = false;
        const qint64 ts = payload.left(sp).toLongLong(&okTs);
        if (!okTs)
            return;

        const QString rest = payload.mid(sp + 1);
        const int bar = rest.indexOf('|');
        if (bar < 0)
            return;

        const QString from = rest.left(bar).trimmed();
        const QString msg = rest.mid(bar + 1).trimmed();
        if (from.isEmpty() || msg.isEmpty())
            return;

        // Qt6: avoid deprecated TimeSpec overload
        const QString t = QDateTime::fromMSecsSinceEpoch(ts, QTimeZone::systemTimeZone())
                              .toString("yyyy-MM-dd HH:mm:ss");
        logLine(QString("[CHAT %1] %2: %3").arg(t, from, msg));
        return;
    }

    if (ev == "ROOM_CHAT_END")
    {
        statusBar()->showMessage("Đã tải lịch sử chat phòng.", 1500);
        return;
    }

    statusBar()->showMessage("Đã cập nhật từ máy chủ.", 2000);
}

/* =======================
 * Chat
 * ======================= */
QString MainWindow::sanitizeChatText(QString msg)
{
    msg.replace('\n', ' ');
    msg.replace('\r', ' ');
    msg.replace('|', QChar(0xFF5C));
    return msg.trimmed();
}

void MainWindow::onRefreshChatUsers()
{
    if (!sock || !sock->isConnected())
        return;
    if (currentRoomId == 0)
        return;
    sock->sendLine("CHAT_USERS");
}

void MainWindow::openChatWith(const QString &peer, bool switchTab)
{
    const QString p = peer.trimmed();
    if (p.isEmpty() || p == username)
        return;

    m_chatPeer = p;
    if (ui->lblChatPeer)
        ui->lblChatPeer->setText(QString("Đang chat với: %1").arg(m_chatPeer));
    if (ui->btnChatSend)
        ui->btnChatSend->setEnabled(true);

    renderChat(m_chatPeer);

    if (switchTab && ui->tabHome && ui->tabChat)
    {
        ui->tabHome->setCurrentWidget(ui->tabChat);
    }
    if (ui->edtChatMsg)
        ui->edtChatMsg->setFocus();
}

void MainWindow::renderChat(const QString &peer)
{
    if (!ui->txtChat)
        return;
    ui->txtChat->clear();
    const auto history = m_chatHistory.value(peer);
    for (const auto &line : history)
        ui->txtChat->appendPlainText(line);
}

void MainWindow::appendChatLine(const QString &peer, const QString &line)
{
    const QString p = peer.trimmed();
    if (p.isEmpty())
        return;

    m_chatHistory[p].append(line);
    if (m_chatHistory[p].size() > 200)
    {
        m_chatHistory[p].removeFirst();
    }

    if (m_chatPeer.compare(p, Qt::CaseInsensitive) == 0)
    {
        if (ui->txtChat)
            ui->txtChat->appendPlainText(line);
    }
    else
    {
        statusBar()->showMessage(QString("Tin nhắn mới từ %1").arg(p), 3000);
    }
}

void MainWindow::onChatSend()
{
    if (!sock || !sock->isConnected())
        return;
    if (m_chatPeer.isEmpty())
        return;
    if (!ui->edtChatMsg)
        return;

    QString msg = sanitizeChatText(ui->edtChatMsg->text());
    if (msg.isEmpty())
        return;

    sock->sendLine(QString("CHAT_TO %1|%2").arg(m_chatPeer, msg));
    appendChatLine(m_chatPeer, QString("Tôi: %1").arg(msg));

    ui->edtChatMsg->clear();
    ui->edtChatMsg->setFocus();
}

/* =======================
 * Room public chat send
 * ======================= */
void MainWindow::onRoomChatSend()
{
    if (!sock || !sock->isConnected())
        return;
    if (currentRoomId <= 0)
    {
        statusBar()->showMessage("Bạn chưa ở trong phòng.", 1500);
        return;
    }
    if (!m_roomChatEdit)
        return;

    QString msg = sanitizeChatText(m_roomChatEdit->text());
    if (msg.isEmpty())
        return;

    // Server expects: CHAT_ROOM |<msg>
    sock->sendLine(QString("CHAT_ROOM |%1").arg(msg));

    m_roomChatEdit->clear();
    m_roomChatEdit->setFocus();
}

/* =======================
 * Rooms tab: Selected room users (quick chat)
 * ======================= */
void MainWindow::requestSelectedRoomUsers(int roomId)
{
    if (!ui->lstSelRoomUsers)
        return;

    m_selectedRoomUsers.clear();
    renderSelectedRoomUsers();

    if (!sock || !sock->isConnected())
        return;
    if (!m_hasSession)
        return;
    if (roomId <= 0)
        return;

    sock->sendLine(QString("ROOM_USERS %1").arg(roomId));
}

void MainWindow::renderSelectedRoomUsers()
{
    if (!ui->lstSelRoomUsers)
        return;
    ui->lstSelRoomUsers->clear();

    const QString q = ui->edtSelRoomUserSearch ? ui->edtSelRoomUserSearch->text().trimmed() : QString();
    for (const QString &u : std::as_const(m_selectedRoomUsers))
    {
        if (u.isEmpty())
            continue;
        if (!q.isEmpty() && !u.contains(q, Qt::CaseInsensitive))
            continue;
        ui->lstSelRoomUsers->addItem(u);
    }
}
