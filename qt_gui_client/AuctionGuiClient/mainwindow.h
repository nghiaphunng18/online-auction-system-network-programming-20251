#pragma once

#include <QMainWindow>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

#include "socketclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(SocketClient* client, QWidget *parent = nullptr);
    ~MainWindow() override;

    // Called by main.cpp after AuthWindow succeeded
    void setSession(const QString& user, const QString& userRole);

private slots:
    // UI actions
    void onConnectClicked();
    void onDisconnectClicked();
    void onBackToConnect();
    void onLoginClicked();

    void onRefreshRooms();
    void onCreateRoom();
    void onJoinRoom();
    void onJoinSelected();

    void onOpenHistoryTab();
    void onRefreshHistory();

    void onRoomFilterChanged();
    void onRoomsSelectionChanged();
    void onSelRoomUserSearchChanged();
    void onSelRoomUserDoubleClicked(QListWidgetItem* item);
    void onHistoryFilterChanged();

    void onLogout();

    // Room tab
    void onLeaveRoom();
    void onStartRoom();
    void onEndRoom();
    void onViewQueued();
    void onViewEnded();

    // Seller draft
    void onAddDraft();
    void onDraftList();
    void onPublishOk();

    // Buyer actions
    void onBid();
    void onBuyNow();

    // Auto-bid (server-side)
    void onAutoBidApply();
    void onAutoBidOff();


    // Chat
    void onRefreshChatUsers();
    void onChatSend();
    void onRoomChatSend();

    // Socket events
    void onSockConnected();
    void onSockDisconnected();
    void onSockError(const QString& msg);
    void onLine(const QString& line);

private:
    Ui::MainWindow *ui = nullptr;
    SocketClient* sock = nullptr;

    // session
    QString username;
    QString role;
    bool m_hasSession = false;

    // room state
    int m_tabRoomIndex = -1;   // index in tabHome for "pageRoom"
    int m_tabChatIndex = -1;   // index in tabHome for "tabChat"
    int currentRoomId = 0;
    int currentItemId = 0;

    // room header state (Room tab)
    QString m_roomStatus;        // PENDING/STARTED/ENDED...
    int m_roomParticipants = 0;

    // countdown (client-side)
    QTimer* m_countdownTimer = nullptr;
    int m_timeLeftSec = -1;

    // chat state
    QString m_chatPeer;
    QMap<QString, QStringList> m_chatHistory;

    // public room chat input (Room tab)
    QLineEdit* m_roomChatEdit = nullptr;
    QPushButton* m_roomChatSendBtn = nullptr;

    // selected room users (Rooms tab)
    int m_selectedRoomId = 0;
    QStringList m_selectedRoomUsers;

    // navigation
    void gotoPage(int idx);
    void logLine(const QString& s);

    // helpers
    void ensureRoomChatBar();          // create room chat bar only once
    void setRoomChatEnabled(bool on);  // enable/disable room chat cleanly

    void openRoomTab();
    void closeRoomTab(bool switchToRoomsTab);

    // chat helpers
    void openChatWith(const QString& peer, bool switchTab = true);
    void renderChat(const QString& peer);
    void appendChatLine(const QString& peer, const QString& line);
    static QString sanitizeChatText(QString msg);

    void requestSelectedRoomUsers(int roomId);
    void renderSelectedRoomUsers();

    // room UI helpers
    void setRoomHeader(const QString& status, int participants);
    void setRoomParticipants(int participants);

    // countdown helpers
    void stopCountdown();
    void syncCountdownFromMmss(const QString& mmss);
    void tickCountdown();
    static int parseMmssToSeconds(const QString& mmss);
    static QString formatSecondsToMmss(int sec);


    // draft input validation (Seller)
    void clearDraftInputErrors();
    void markInputInvalid(QLineEdit* w, const QString& msg);
    bool validateDraftInputs(QString& outName, int& outStart, int& outBuy, int& outDur);

    // table helpers
    void roomsTableClear();
    void roomsTableAddRow(const QStringList& cols);

    void historyClear();
    void historyAddRow(const QStringList& cols);

    void queueClear();
    void queueAddRow(const QStringList& cols);

    void draftClear();
    void draftAddRow(const QStringList& cols);
    void draftRemoveById(const QString& draftId);

    void endedClear();
    void endedAddRow(const QStringList& cols);

    // filtering helpers
    void applyRoomFilter();
    void applyHistoryFilter();

    // ===== Deduplicate notifications (avoid showing the same popup twice) =====
    // Store last shown time for each key so interleaved messages still get deduped.
    QHash<QString, qint64> m_noticeLastMs;

    bool suppressDuplicateNotice(const QString& key, int windowMs = 800);
    void showNotice(const QString& key,
                    const QString& title,
                    const QString& message,
                    bool popup = true);

    // protocol handlers
    void handleOK(const QStringList& tok, const QString& raw);
    void handleERR(const QStringList& tok, const QString& raw);
    void handleEVT(const QStringList& tok, const QString& raw);

    static QStringList splitWs(const QString& s);
};
