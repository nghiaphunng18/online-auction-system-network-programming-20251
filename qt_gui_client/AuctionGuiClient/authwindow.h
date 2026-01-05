#pragma once

#include <QDialog>
#include <QString>

class SocketClient;

class QTabWidget;
class QLineEdit;
class QPushButton;
class QLabel;

class AuthWindow : public QDialog {
    Q_OBJECT
public:
    explicit AuthWindow(SocketClient* client, QWidget* parent = nullptr);

    QString loggedUser() const { return m_user; }
    QString loggedRole() const { return m_role; }

private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();

    void onConnected();
    void onDisconnected();
    void onLineReceived(const QString& line);
    void onErrorText(const QString& msg);

private:
    void buildUi();
    void setBusy(bool busy);
    void setStatus(const QString& text);
    void updateUiState();

private:
    SocketClient* m_client = nullptr;

    bool m_connected = false;
    bool m_busy = false;

    QString m_user;
    QString m_role;

    // Track which request is currently pending (login/register) to show
    // clearer Vietnamese messages.
    enum class PendingAction {
        None,
        Login,
        Register
    };
    PendingAction m_pending = PendingAction::None;

    // ===== UI: connect =====
    QLineEdit* edtHost = nullptr;
    QLineEdit* edtPort = nullptr;
    QPushButton* btnConnect = nullptr;
    QLabel* lblConn = nullptr;

    // ===== UI: tabs =====
    QTabWidget* tabs = nullptr;

    // Login tab
    QLineEdit* edtLoginUser = nullptr;
    QLineEdit* edtLoginPass = nullptr;
    QPushButton* btnLogin = nullptr;

    // Register tab
    QLineEdit* edtRegUser = nullptr;
    QLineEdit* edtRegPass = nullptr;
    QLineEdit* edtRegPass2 = nullptr;
    QPushButton* btnRegister = nullptr;

    // status
    QLabel* lblStatus = nullptr;
};
