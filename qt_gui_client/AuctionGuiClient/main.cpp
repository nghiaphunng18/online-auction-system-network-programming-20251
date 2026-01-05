#include "authwindow.h"
#include "mainwindow.h"

#include <QApplication>
#include <QDialog>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 1 SocketClient dùng chung cho toàn app (giữ kết nối xuyên suốt)
    SocketClient client;

    // Auth (modal)
    AuthWindow auth(&client);

    // Chạy dạng exec() để chờ đăng nhập xong
    if (auth.exec() != QDialog::Accepted) {
        return 0; // user đóng auth => thoát app
    }

    // Mở MainWindow và dùng lại socket đã connect + login
    MainWindow w(&client);
    w.setSession(auth.loggedUser(), auth.loggedRole());
    w.show();

    return a.exec();
}
