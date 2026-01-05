/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *stackedWidget;
    QWidget *pageConnect;
    QVBoxLayout *vConnect;
    QLabel *lblTitleConnect;
    QFormLayout *formConnect;
    QLabel *lblHost;
    QLineEdit *edtHost;
    QLabel *lblPort;
    QSpinBox *spinPort;
    QHBoxLayout *hConnectBtns;
    QPushButton *btnConnect;
    QPushButton *btnDisconnect;
    QSpacerItem *spConnect;
    QLabel *lblConnStatus;
    QSpacerItem *spConnectBottom;
    QWidget *pageLogin;
    QVBoxLayout *vLogin;
    QLabel *lblTitleLogin;
    QFormLayout *formLogin;
    QLabel *lblUser;
    QLineEdit *edtUser;
    QLabel *lblPass;
    QLineEdit *edtPass;
    QHBoxLayout *hLoginBtns;
    QPushButton *btnLogin;
    QPushButton *btnBackToConnect;
    QSpacerItem *spLogin;
    QLabel *lblAuthStatus;
    QSpacerItem *spLoginBottom;
    QWidget *pageRooms;
    QVBoxLayout *vRooms;
    QHBoxLayout *hHomeHeader;
    QLabel *lblTitleRooms;
    QSpacerItem *spHomeHeader;
    QLabel *lblUserInfo;
    QPushButton *btnLogout;
    QTabWidget *tabHome;
    QWidget *tabRooms;
    QHBoxLayout *hRoomsBody;
    QVBoxLayout *vRoomsLeft;
    QHBoxLayout *hRoomsTools;
    QLineEdit *edtRoomSearch;
    QComboBox *cmbRoomScope;
    QComboBox *cmbRoomStatus;
    QPushButton *btnRefreshRooms;
    QTableWidget *tblRooms;
    QVBoxLayout *vRoomsRight;
    QGroupBox *grpQuickActions;
    QVBoxLayout *vQuick;
    QHBoxLayout *hCreate;
    QLineEdit *edtCreateRoom;
    QPushButton *btnCreateRoom;
    QHBoxLayout *hJoin;
    QLineEdit *edtJoinRoomId;
    QPushButton *btnJoinRoom;
    QHBoxLayout *hJoinSelectedRow;
    QPushButton *btnJoinSelected;
    QPushButton *btnOpenHistory;
    QGroupBox *grpRoomDetails;
    QVBoxLayout *vSelRoom;
    QFormLayout *formRoomDetails;
    QLabel *lblSelIdTitle;
    QLabel *lblSelRoomId;
    QLabel *lblSelNameTitle;
    QLabel *lblSelRoomName;
    QLabel *lblSelStatusTitle;
    QLabel *lblSelRoomStatus;
    QLabel *lblSelPartTitle;
    QLabel *lblSelRoomParticipants;
    QLabel *lblSelOwnerTitle;
    QLabel *lblSelRoomOwner;
    QFrame *lineSelRoomUsers;
    QLabel *lblSelRoomUsersTitle;
    QLineEdit *edtSelRoomUserSearch;
    QListWidget *lstSelRoomUsers;
    QLabel *lblSelRoomUsersHint;
    QLabel *lblRoomsStatus;
    QSpacerItem *spRoomsRightBottom;
    QWidget *tabHistory;
    QVBoxLayout *vHistory;
    QHBoxLayout *hHistTools;
    QPushButton *btnRefreshHistory;
    QSpacerItem *spHistTools;
    QLineEdit *edtHistSearch;
    QTableWidget *tblHistory;
    QLabel *lblHistoryStatus;
    QWidget *tabChat;
    QHBoxLayout *hChat;
    QGroupBox *grpChatUsers;
    QVBoxLayout *vChatUsers;
    QHBoxLayout *hChatUserTools;
    QPushButton *btnChatUsers;
    QSpacerItem *spChatUsers;
    QListWidget *lstChatUsers;
    QGroupBox *grpChatBox;
    QVBoxLayout *vChatBox;
    QLabel *lblChatPeer;
    QPlainTextEdit *txtChat;
    QHBoxLayout *hChatInput;
    QLineEdit *edtChatMsg;
    QPushButton *btnChatSend;
    QWidget *pageRoom;
    QVBoxLayout *vRoom;
    QGroupBox *grpRoomUsers;
    QVBoxLayout *vRoomUsers;
    QHBoxLayout *hRoomUsersTools;
    QPushButton *btnRefreshChatUsers;
    QLabel *lblRoomUsersHint;
    QSpacerItem *spRoomUsers;
    QListWidget *lstRoomUsers;
    QHBoxLayout *hRoomHeader;
    QLabel *lblRoomTitle;
    QSpacerItem *spRoomHeader;
    QPushButton *btnLeaveRoom;
    QPushButton *btnStartRoom;
    QPushButton *btnEndRoom;
    QHBoxLayout *hRoomMain;
    QVBoxLayout *vLeft;
    QGroupBox *grpCurrent;
    QFormLayout *formCurrent;
    QLabel *lblItemNameTitle;
    QLabel *lblItemName;
    QLabel *lblItemSellerTitle;
    QLabel *lblItemSeller;
    QLabel *lblItemStartTitle;
    QLabel *lblItemStart;
    QLabel *lblItemBuyNowTitle;
    QLabel *lblItemBuyNow;
    QLabel *lblItemPriceTitle;
    QLabel *lblItemPrice;
    QLabel *lblItemLeaderTitle;
    QLabel *lblItemLeader;
    QLabel *lblItemTimeTitle;
    QLabel *lblItemTime;
    QLabel *lblMinBid;
    QHBoxLayout *hBidRow;
    QLineEdit *edtBidAmount;
    QPushButton *btnBid;
    QPushButton *btnBuyNow;
    QLabel *lblRoomStatus;
    QPlainTextEdit *txtLog;
    QVBoxLayout *vRight;
    QGroupBox *grpQueue;
    QVBoxLayout *vQueue;
    QTableWidget *tblQueue;
    QHBoxLayout *hQueueBtns;
    QPushButton *btnViewQueued;
    QSpacerItem *spQueueBtns;
    QGroupBox *grpDraft;
    QVBoxLayout *vDraft;
    QHBoxLayout *hDraftAdd;
    QLineEdit *edtDraftName;
    QLineEdit *edtDraftStart;
    QLineEdit *edtDraftBuy;
    QLineEdit *edtDraftDur;
    QPushButton *btnAddDraft;
    QTableWidget *tblDraft;
    QHBoxLayout *hDraftBtns;
    QPushButton *btnDraftList;
    QPushButton *btnPublishOk;
    QSpacerItem *spDraftBtns;
    QGroupBox *grpEnded;
    QVBoxLayout *vEnded;
    QTableWidget *tblEnded;
    QHBoxLayout *hEndedBtns;
    QPushButton *btnViewEnded;
    QSpacerItem *spEndedBtns;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        pageConnect = new QWidget();
        pageConnect->setObjectName("pageConnect");
        vConnect = new QVBoxLayout(pageConnect);
        vConnect->setObjectName("vConnect");
        lblTitleConnect = new QLabel(pageConnect);
        lblTitleConnect->setObjectName("lblTitleConnect");

        vConnect->addWidget(lblTitleConnect);

        formConnect = new QFormLayout();
        formConnect->setObjectName("formConnect");
        lblHost = new QLabel(pageConnect);
        lblHost->setObjectName("lblHost");

        formConnect->setWidget(0, QFormLayout::ItemRole::LabelRole, lblHost);

        edtHost = new QLineEdit(pageConnect);
        edtHost->setObjectName("edtHost");

        formConnect->setWidget(0, QFormLayout::ItemRole::FieldRole, edtHost);

        lblPort = new QLabel(pageConnect);
        lblPort->setObjectName("lblPort");

        formConnect->setWidget(1, QFormLayout::ItemRole::LabelRole, lblPort);

        spinPort = new QSpinBox(pageConnect);
        spinPort->setObjectName("spinPort");
        spinPort->setMinimum(1);
        spinPort->setMaximum(65535);
        spinPort->setValue(5555);

        formConnect->setWidget(1, QFormLayout::ItemRole::FieldRole, spinPort);


        vConnect->addLayout(formConnect);

        hConnectBtns = new QHBoxLayout();
        hConnectBtns->setObjectName("hConnectBtns");
        btnConnect = new QPushButton(pageConnect);
        btnConnect->setObjectName("btnConnect");

        hConnectBtns->addWidget(btnConnect);

        btnDisconnect = new QPushButton(pageConnect);
        btnDisconnect->setObjectName("btnDisconnect");

        hConnectBtns->addWidget(btnDisconnect);

        spConnect = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hConnectBtns->addItem(spConnect);


        vConnect->addLayout(hConnectBtns);

        lblConnStatus = new QLabel(pageConnect);
        lblConnStatus->setObjectName("lblConnStatus");

        vConnect->addWidget(lblConnStatus);

        spConnectBottom = new QSpacerItem(20, 200, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vConnect->addItem(spConnectBottom);

        stackedWidget->addWidget(pageConnect);
        pageLogin = new QWidget();
        pageLogin->setObjectName("pageLogin");
        vLogin = new QVBoxLayout(pageLogin);
        vLogin->setObjectName("vLogin");
        lblTitleLogin = new QLabel(pageLogin);
        lblTitleLogin->setObjectName("lblTitleLogin");

        vLogin->addWidget(lblTitleLogin);

        formLogin = new QFormLayout();
        formLogin->setObjectName("formLogin");
        lblUser = new QLabel(pageLogin);
        lblUser->setObjectName("lblUser");

        formLogin->setWidget(0, QFormLayout::ItemRole::LabelRole, lblUser);

        edtUser = new QLineEdit(pageLogin);
        edtUser->setObjectName("edtUser");

        formLogin->setWidget(0, QFormLayout::ItemRole::FieldRole, edtUser);

        lblPass = new QLabel(pageLogin);
        lblPass->setObjectName("lblPass");

        formLogin->setWidget(1, QFormLayout::ItemRole::LabelRole, lblPass);

        edtPass = new QLineEdit(pageLogin);
        edtPass->setObjectName("edtPass");
        edtPass->setEchoMode(QLineEdit::Password);

        formLogin->setWidget(1, QFormLayout::ItemRole::FieldRole, edtPass);


        vLogin->addLayout(formLogin);

        hLoginBtns = new QHBoxLayout();
        hLoginBtns->setObjectName("hLoginBtns");
        btnLogin = new QPushButton(pageLogin);
        btnLogin->setObjectName("btnLogin");

        hLoginBtns->addWidget(btnLogin);

        btnBackToConnect = new QPushButton(pageLogin);
        btnBackToConnect->setObjectName("btnBackToConnect");

        hLoginBtns->addWidget(btnBackToConnect);

        spLogin = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hLoginBtns->addItem(spLogin);


        vLogin->addLayout(hLoginBtns);

        lblAuthStatus = new QLabel(pageLogin);
        lblAuthStatus->setObjectName("lblAuthStatus");

        vLogin->addWidget(lblAuthStatus);

        spLoginBottom = new QSpacerItem(20, 200, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vLogin->addItem(spLoginBottom);

        stackedWidget->addWidget(pageLogin);
        pageRooms = new QWidget();
        pageRooms->setObjectName("pageRooms");
        vRooms = new QVBoxLayout(pageRooms);
        vRooms->setObjectName("vRooms");
        hHomeHeader = new QHBoxLayout();
        hHomeHeader->setObjectName("hHomeHeader");
        lblTitleRooms = new QLabel(pageRooms);
        lblTitleRooms->setObjectName("lblTitleRooms");

        hHomeHeader->addWidget(lblTitleRooms);

        spHomeHeader = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hHomeHeader->addItem(spHomeHeader);

        lblUserInfo = new QLabel(pageRooms);
        lblUserInfo->setObjectName("lblUserInfo");

        hHomeHeader->addWidget(lblUserInfo);

        btnLogout = new QPushButton(pageRooms);
        btnLogout->setObjectName("btnLogout");

        hHomeHeader->addWidget(btnLogout);


        vRooms->addLayout(hHomeHeader);

        tabHome = new QTabWidget(pageRooms);
        tabHome->setObjectName("tabHome");
        tabRooms = new QWidget();
        tabRooms->setObjectName("tabRooms");
        hRoomsBody = new QHBoxLayout(tabRooms);
        hRoomsBody->setObjectName("hRoomsBody");
        vRoomsLeft = new QVBoxLayout();
        vRoomsLeft->setObjectName("vRoomsLeft");
        hRoomsTools = new QHBoxLayout();
        hRoomsTools->setObjectName("hRoomsTools");
        edtRoomSearch = new QLineEdit(tabRooms);
        edtRoomSearch->setObjectName("edtRoomSearch");

        hRoomsTools->addWidget(edtRoomSearch);

        cmbRoomScope = new QComboBox(tabRooms);
        cmbRoomScope->addItem(QString());
        cmbRoomScope->addItem(QString());
        cmbRoomScope->setObjectName("cmbRoomScope");

        hRoomsTools->addWidget(cmbRoomScope);

        cmbRoomStatus = new QComboBox(tabRooms);
        cmbRoomStatus->addItem(QString());
        cmbRoomStatus->addItem(QString());
        cmbRoomStatus->addItem(QString());
        cmbRoomStatus->addItem(QString());
        cmbRoomStatus->setObjectName("cmbRoomStatus");

        hRoomsTools->addWidget(cmbRoomStatus);

        btnRefreshRooms = new QPushButton(tabRooms);
        btnRefreshRooms->setObjectName("btnRefreshRooms");

        hRoomsTools->addWidget(btnRefreshRooms);


        vRoomsLeft->addLayout(hRoomsTools);

        tblRooms = new QTableWidget(tabRooms);
        if (tblRooms->columnCount() < 5)
            tblRooms->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tblRooms->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tblRooms->setObjectName("tblRooms");
        tblRooms->setAlternatingRowColors(true);
        tblRooms->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblRooms->setSelectionMode(QAbstractItemView::SingleSelection);
        tblRooms->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblRooms->setRowCount(0);
        tblRooms->setColumnCount(5);

        vRoomsLeft->addWidget(tblRooms);


        hRoomsBody->addLayout(vRoomsLeft);

        vRoomsRight = new QVBoxLayout();
        vRoomsRight->setObjectName("vRoomsRight");
        grpQuickActions = new QGroupBox(tabRooms);
        grpQuickActions->setObjectName("grpQuickActions");
        vQuick = new QVBoxLayout(grpQuickActions);
        vQuick->setObjectName("vQuick");
        hCreate = new QHBoxLayout();
        hCreate->setObjectName("hCreate");
        edtCreateRoom = new QLineEdit(grpQuickActions);
        edtCreateRoom->setObjectName("edtCreateRoom");

        hCreate->addWidget(edtCreateRoom);

        btnCreateRoom = new QPushButton(grpQuickActions);
        btnCreateRoom->setObjectName("btnCreateRoom");

        hCreate->addWidget(btnCreateRoom);


        vQuick->addLayout(hCreate);

        hJoin = new QHBoxLayout();
        hJoin->setObjectName("hJoin");
        edtJoinRoomId = new QLineEdit(grpQuickActions);
        edtJoinRoomId->setObjectName("edtJoinRoomId");

        hJoin->addWidget(edtJoinRoomId);

        btnJoinRoom = new QPushButton(grpQuickActions);
        btnJoinRoom->setObjectName("btnJoinRoom");

        hJoin->addWidget(btnJoinRoom);


        vQuick->addLayout(hJoin);

        hJoinSelectedRow = new QHBoxLayout();
        hJoinSelectedRow->setObjectName("hJoinSelectedRow");
        btnJoinSelected = new QPushButton(grpQuickActions);
        btnJoinSelected->setObjectName("btnJoinSelected");

        hJoinSelectedRow->addWidget(btnJoinSelected);

        btnOpenHistory = new QPushButton(grpQuickActions);
        btnOpenHistory->setObjectName("btnOpenHistory");

        hJoinSelectedRow->addWidget(btnOpenHistory);


        vQuick->addLayout(hJoinSelectedRow);


        vRoomsRight->addWidget(grpQuickActions);

        grpRoomDetails = new QGroupBox(tabRooms);
        grpRoomDetails->setObjectName("grpRoomDetails");
        vSelRoom = new QVBoxLayout(grpRoomDetails);
        vSelRoom->setObjectName("vSelRoom");
        formRoomDetails = new QFormLayout();
        formRoomDetails->setObjectName("formRoomDetails");
        lblSelIdTitle = new QLabel(grpRoomDetails);
        lblSelIdTitle->setObjectName("lblSelIdTitle");

        formRoomDetails->setWidget(0, QFormLayout::ItemRole::LabelRole, lblSelIdTitle);

        lblSelRoomId = new QLabel(grpRoomDetails);
        lblSelRoomId->setObjectName("lblSelRoomId");

        formRoomDetails->setWidget(0, QFormLayout::ItemRole::FieldRole, lblSelRoomId);

        lblSelNameTitle = new QLabel(grpRoomDetails);
        lblSelNameTitle->setObjectName("lblSelNameTitle");

        formRoomDetails->setWidget(1, QFormLayout::ItemRole::LabelRole, lblSelNameTitle);

        lblSelRoomName = new QLabel(grpRoomDetails);
        lblSelRoomName->setObjectName("lblSelRoomName");

        formRoomDetails->setWidget(1, QFormLayout::ItemRole::FieldRole, lblSelRoomName);

        lblSelStatusTitle = new QLabel(grpRoomDetails);
        lblSelStatusTitle->setObjectName("lblSelStatusTitle");

        formRoomDetails->setWidget(2, QFormLayout::ItemRole::LabelRole, lblSelStatusTitle);

        lblSelRoomStatus = new QLabel(grpRoomDetails);
        lblSelRoomStatus->setObjectName("lblSelRoomStatus");

        formRoomDetails->setWidget(2, QFormLayout::ItemRole::FieldRole, lblSelRoomStatus);

        lblSelPartTitle = new QLabel(grpRoomDetails);
        lblSelPartTitle->setObjectName("lblSelPartTitle");

        formRoomDetails->setWidget(3, QFormLayout::ItemRole::LabelRole, lblSelPartTitle);

        lblSelRoomParticipants = new QLabel(grpRoomDetails);
        lblSelRoomParticipants->setObjectName("lblSelRoomParticipants");

        formRoomDetails->setWidget(3, QFormLayout::ItemRole::FieldRole, lblSelRoomParticipants);

        lblSelOwnerTitle = new QLabel(grpRoomDetails);
        lblSelOwnerTitle->setObjectName("lblSelOwnerTitle");

        formRoomDetails->setWidget(4, QFormLayout::ItemRole::LabelRole, lblSelOwnerTitle);

        lblSelRoomOwner = new QLabel(grpRoomDetails);
        lblSelRoomOwner->setObjectName("lblSelRoomOwner");

        formRoomDetails->setWidget(4, QFormLayout::ItemRole::FieldRole, lblSelRoomOwner);


        vSelRoom->addLayout(formRoomDetails);

        lineSelRoomUsers = new QFrame(grpRoomDetails);
        lineSelRoomUsers->setObjectName("lineSelRoomUsers");
        lineSelRoomUsers->setFrameShape(QFrame::HLine);
        lineSelRoomUsers->setFrameShadow(QFrame::Sunken);

        vSelRoom->addWidget(lineSelRoomUsers);

        lblSelRoomUsersTitle = new QLabel(grpRoomDetails);
        lblSelRoomUsersTitle->setObjectName("lblSelRoomUsersTitle");

        vSelRoom->addWidget(lblSelRoomUsersTitle);

        edtSelRoomUserSearch = new QLineEdit(grpRoomDetails);
        edtSelRoomUserSearch->setObjectName("edtSelRoomUserSearch");

        vSelRoom->addWidget(edtSelRoomUserSearch);

        lstSelRoomUsers = new QListWidget(grpRoomDetails);
        lstSelRoomUsers->setObjectName("lstSelRoomUsers");
        lstSelRoomUsers->setMinimumHeight(120);

        vSelRoom->addWidget(lstSelRoomUsers);

        lblSelRoomUsersHint = new QLabel(grpRoomDetails);
        lblSelRoomUsersHint->setObjectName("lblSelRoomUsersHint");
        lblSelRoomUsersHint->setWordWrap(true);

        vSelRoom->addWidget(lblSelRoomUsersHint);


        vRoomsRight->addWidget(grpRoomDetails);

        lblRoomsStatus = new QLabel(tabRooms);
        lblRoomsStatus->setObjectName("lblRoomsStatus");
        lblRoomsStatus->setWordWrap(true);

        vRoomsRight->addWidget(lblRoomsStatus);

        spRoomsRightBottom = new QSpacerItem(20, 120, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vRoomsRight->addItem(spRoomsRightBottom);


        hRoomsBody->addLayout(vRoomsRight);

        tabHome->addTab(tabRooms, QString());
        tabHistory = new QWidget();
        tabHistory->setObjectName("tabHistory");
        vHistory = new QVBoxLayout(tabHistory);
        vHistory->setObjectName("vHistory");
        hHistTools = new QHBoxLayout();
        hHistTools->setObjectName("hHistTools");
        btnRefreshHistory = new QPushButton(tabHistory);
        btnRefreshHistory->setObjectName("btnRefreshHistory");

        hHistTools->addWidget(btnRefreshHistory);

        spHistTools = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hHistTools->addItem(spHistTools);

        edtHistSearch = new QLineEdit(tabHistory);
        edtHistSearch->setObjectName("edtHistSearch");

        hHistTools->addWidget(edtHistSearch);


        vHistory->addLayout(hHistTools);

        tblHistory = new QTableWidget(tabHistory);
        if (tblHistory->columnCount() < 8)
            tblHistory->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(3, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(4, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(5, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(6, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tblHistory->setHorizontalHeaderItem(7, __qtablewidgetitem12);
        tblHistory->setObjectName("tblHistory");
        tblHistory->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        tblHistory->setAlternatingRowColors(true);
        tblHistory->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tblHistory->setRowCount(0);
        tblHistory->setColumnCount(8);

        vHistory->addWidget(tblHistory);

        lblHistoryStatus = new QLabel(tabHistory);
        lblHistoryStatus->setObjectName("lblHistoryStatus");
        lblHistoryStatus->setWordWrap(true);

        vHistory->addWidget(lblHistoryStatus);

        tabHome->addTab(tabHistory, QString());
        tabChat = new QWidget();
        tabChat->setObjectName("tabChat");
        hChat = new QHBoxLayout(tabChat);
        hChat->setObjectName("hChat");
        grpChatUsers = new QGroupBox(tabChat);
        grpChatUsers->setObjectName("grpChatUsers");
        vChatUsers = new QVBoxLayout(grpChatUsers);
        vChatUsers->setObjectName("vChatUsers");
        hChatUserTools = new QHBoxLayout();
        hChatUserTools->setObjectName("hChatUserTools");
        btnChatUsers = new QPushButton(grpChatUsers);
        btnChatUsers->setObjectName("btnChatUsers");

        hChatUserTools->addWidget(btnChatUsers);

        spChatUsers = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hChatUserTools->addItem(spChatUsers);


        vChatUsers->addLayout(hChatUserTools);

        lstChatUsers = new QListWidget(grpChatUsers);
        lstChatUsers->setObjectName("lstChatUsers");
        lstChatUsers->setMinimumWidth(200);

        vChatUsers->addWidget(lstChatUsers);


        hChat->addWidget(grpChatUsers);

        grpChatBox = new QGroupBox(tabChat);
        grpChatBox->setObjectName("grpChatBox");
        vChatBox = new QVBoxLayout(grpChatBox);
        vChatBox->setObjectName("vChatBox");
        lblChatPeer = new QLabel(grpChatBox);
        lblChatPeer->setObjectName("lblChatPeer");

        vChatBox->addWidget(lblChatPeer);

        txtChat = new QPlainTextEdit(grpChatBox);
        txtChat->setObjectName("txtChat");
        txtChat->setReadOnly(true);
        txtChat->setMinimumHeight(260);

        vChatBox->addWidget(txtChat);

        hChatInput = new QHBoxLayout();
        hChatInput->setObjectName("hChatInput");
        edtChatMsg = new QLineEdit(grpChatBox);
        edtChatMsg->setObjectName("edtChatMsg");

        hChatInput->addWidget(edtChatMsg);

        btnChatSend = new QPushButton(grpChatBox);
        btnChatSend->setObjectName("btnChatSend");

        hChatInput->addWidget(btnChatSend);


        vChatBox->addLayout(hChatInput);


        hChat->addWidget(grpChatBox);

        tabHome->addTab(tabChat, QString());

        vRooms->addWidget(tabHome);

        stackedWidget->addWidget(pageRooms);
        pageRoom = new QWidget();
        pageRoom->setObjectName("pageRoom");
        vRoom = new QVBoxLayout(pageRoom);
        vRoom->setObjectName("vRoom");
        grpRoomUsers = new QGroupBox(pageRoom);
        grpRoomUsers->setObjectName("grpRoomUsers");
        vRoomUsers = new QVBoxLayout(grpRoomUsers);
        vRoomUsers->setObjectName("vRoomUsers");
        hRoomUsersTools = new QHBoxLayout();
        hRoomUsersTools->setObjectName("hRoomUsersTools");
        btnRefreshChatUsers = new QPushButton(grpRoomUsers);
        btnRefreshChatUsers->setObjectName("btnRefreshChatUsers");

        hRoomUsersTools->addWidget(btnRefreshChatUsers);

        lblRoomUsersHint = new QLabel(grpRoomUsers);
        lblRoomUsersHint->setObjectName("lblRoomUsersHint");

        hRoomUsersTools->addWidget(lblRoomUsersHint);

        spRoomUsers = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hRoomUsersTools->addItem(spRoomUsers);


        vRoomUsers->addLayout(hRoomUsersTools);

        lstRoomUsers = new QListWidget(grpRoomUsers);
        lstRoomUsers->setObjectName("lstRoomUsers");
        lstRoomUsers->setMinimumHeight(90);

        vRoomUsers->addWidget(lstRoomUsers);


        vRoom->addWidget(grpRoomUsers);

        hRoomHeader = new QHBoxLayout();
        hRoomHeader->setObjectName("hRoomHeader");
        lblRoomTitle = new QLabel(pageRoom);
        lblRoomTitle->setObjectName("lblRoomTitle");

        hRoomHeader->addWidget(lblRoomTitle);

        spRoomHeader = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hRoomHeader->addItem(spRoomHeader);

        btnLeaveRoom = new QPushButton(pageRoom);
        btnLeaveRoom->setObjectName("btnLeaveRoom");

        hRoomHeader->addWidget(btnLeaveRoom);

        btnStartRoom = new QPushButton(pageRoom);
        btnStartRoom->setObjectName("btnStartRoom");

        hRoomHeader->addWidget(btnStartRoom);

        btnEndRoom = new QPushButton(pageRoom);
        btnEndRoom->setObjectName("btnEndRoom");

        hRoomHeader->addWidget(btnEndRoom);


        vRoom->addLayout(hRoomHeader);

        hRoomMain = new QHBoxLayout();
        hRoomMain->setObjectName("hRoomMain");
        vLeft = new QVBoxLayout();
        vLeft->setObjectName("vLeft");
        grpCurrent = new QGroupBox(pageRoom);
        grpCurrent->setObjectName("grpCurrent");
        formCurrent = new QFormLayout(grpCurrent);
        formCurrent->setObjectName("formCurrent");
        lblItemNameTitle = new QLabel(grpCurrent);
        lblItemNameTitle->setObjectName("lblItemNameTitle");

        formCurrent->setWidget(0, QFormLayout::ItemRole::LabelRole, lblItemNameTitle);

        lblItemName = new QLabel(grpCurrent);
        lblItemName->setObjectName("lblItemName");

        formCurrent->setWidget(0, QFormLayout::ItemRole::FieldRole, lblItemName);

        lblItemSellerTitle = new QLabel(grpCurrent);
        lblItemSellerTitle->setObjectName("lblItemSellerTitle");

        formCurrent->setWidget(1, QFormLayout::ItemRole::LabelRole, lblItemSellerTitle);

        lblItemSeller = new QLabel(grpCurrent);
        lblItemSeller->setObjectName("lblItemSeller");

        formCurrent->setWidget(1, QFormLayout::ItemRole::FieldRole, lblItemSeller);

        lblItemStartTitle = new QLabel(grpCurrent);
        lblItemStartTitle->setObjectName("lblItemStartTitle");

        formCurrent->setWidget(2, QFormLayout::ItemRole::LabelRole, lblItemStartTitle);

        lblItemStart = new QLabel(grpCurrent);
        lblItemStart->setObjectName("lblItemStart");

        formCurrent->setWidget(2, QFormLayout::ItemRole::FieldRole, lblItemStart);

        lblItemBuyNowTitle = new QLabel(grpCurrent);
        lblItemBuyNowTitle->setObjectName("lblItemBuyNowTitle");

        formCurrent->setWidget(3, QFormLayout::ItemRole::LabelRole, lblItemBuyNowTitle);

        lblItemBuyNow = new QLabel(grpCurrent);
        lblItemBuyNow->setObjectName("lblItemBuyNow");

        formCurrent->setWidget(3, QFormLayout::ItemRole::FieldRole, lblItemBuyNow);

        lblItemPriceTitle = new QLabel(grpCurrent);
        lblItemPriceTitle->setObjectName("lblItemPriceTitle");

        formCurrent->setWidget(4, QFormLayout::ItemRole::LabelRole, lblItemPriceTitle);

        lblItemPrice = new QLabel(grpCurrent);
        lblItemPrice->setObjectName("lblItemPrice");

        formCurrent->setWidget(4, QFormLayout::ItemRole::FieldRole, lblItemPrice);

        lblItemLeaderTitle = new QLabel(grpCurrent);
        lblItemLeaderTitle->setObjectName("lblItemLeaderTitle");

        formCurrent->setWidget(5, QFormLayout::ItemRole::LabelRole, lblItemLeaderTitle);

        lblItemLeader = new QLabel(grpCurrent);
        lblItemLeader->setObjectName("lblItemLeader");

        formCurrent->setWidget(5, QFormLayout::ItemRole::FieldRole, lblItemLeader);

        lblItemTimeTitle = new QLabel(grpCurrent);
        lblItemTimeTitle->setObjectName("lblItemTimeTitle");

        formCurrent->setWidget(6, QFormLayout::ItemRole::LabelRole, lblItemTimeTitle);

        lblItemTime = new QLabel(grpCurrent);
        lblItemTime->setObjectName("lblItemTime");

        formCurrent->setWidget(6, QFormLayout::ItemRole::FieldRole, lblItemTime);


        vLeft->addWidget(grpCurrent);

        lblMinBid = new QLabel(pageRoom);
        lblMinBid->setObjectName("lblMinBid");

        vLeft->addWidget(lblMinBid);

        hBidRow = new QHBoxLayout();
        hBidRow->setObjectName("hBidRow");
        edtBidAmount = new QLineEdit(pageRoom);
        edtBidAmount->setObjectName("edtBidAmount");

        hBidRow->addWidget(edtBidAmount);

        btnBid = new QPushButton(pageRoom);
        btnBid->setObjectName("btnBid");

        hBidRow->addWidget(btnBid);

        btnBuyNow = new QPushButton(pageRoom);
        btnBuyNow->setObjectName("btnBuyNow");

        hBidRow->addWidget(btnBuyNow);


        vLeft->addLayout(hBidRow);

        lblRoomStatus = new QLabel(pageRoom);
        lblRoomStatus->setObjectName("lblRoomStatus");

        vLeft->addWidget(lblRoomStatus);

        txtLog = new QPlainTextEdit(pageRoom);
        txtLog->setObjectName("txtLog");
        txtLog->setReadOnly(true);

        vLeft->addWidget(txtLog);


        hRoomMain->addLayout(vLeft);

        vRight = new QVBoxLayout();
        vRight->setObjectName("vRight");
        grpQueue = new QGroupBox(pageRoom);
        grpQueue->setObjectName("grpQueue");
        vQueue = new QVBoxLayout(grpQueue);
        vQueue->setObjectName("vQueue");
        tblQueue = new QTableWidget(grpQueue);
        if (tblQueue->columnCount() < 5)
            tblQueue->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tblQueue->setHorizontalHeaderItem(0, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tblQueue->setHorizontalHeaderItem(1, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tblQueue->setHorizontalHeaderItem(2, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tblQueue->setHorizontalHeaderItem(3, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tblQueue->setHorizontalHeaderItem(4, __qtablewidgetitem17);
        tblQueue->setObjectName("tblQueue");
        tblQueue->setRowCount(0);
        tblQueue->setColumnCount(5);

        vQueue->addWidget(tblQueue);

        hQueueBtns = new QHBoxLayout();
        hQueueBtns->setObjectName("hQueueBtns");
        btnViewQueued = new QPushButton(grpQueue);
        btnViewQueued->setObjectName("btnViewQueued");

        hQueueBtns->addWidget(btnViewQueued);

        spQueueBtns = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hQueueBtns->addItem(spQueueBtns);


        vQueue->addLayout(hQueueBtns);


        vRight->addWidget(grpQueue);

        grpDraft = new QGroupBox(pageRoom);
        grpDraft->setObjectName("grpDraft");
        vDraft = new QVBoxLayout(grpDraft);
        vDraft->setObjectName("vDraft");
        hDraftAdd = new QHBoxLayout();
        hDraftAdd->setObjectName("hDraftAdd");
        edtDraftName = new QLineEdit(grpDraft);
        edtDraftName->setObjectName("edtDraftName");

        hDraftAdd->addWidget(edtDraftName);

        edtDraftStart = new QLineEdit(grpDraft);
        edtDraftStart->setObjectName("edtDraftStart");

        hDraftAdd->addWidget(edtDraftStart);

        edtDraftBuy = new QLineEdit(grpDraft);
        edtDraftBuy->setObjectName("edtDraftBuy");

        hDraftAdd->addWidget(edtDraftBuy);

        edtDraftDur = new QLineEdit(grpDraft);
        edtDraftDur->setObjectName("edtDraftDur");

        hDraftAdd->addWidget(edtDraftDur);

        btnAddDraft = new QPushButton(grpDraft);
        btnAddDraft->setObjectName("btnAddDraft");

        hDraftAdd->addWidget(btnAddDraft);


        vDraft->addLayout(hDraftAdd);

        tblDraft = new QTableWidget(grpDraft);
        if (tblDraft->columnCount() < 6)
            tblDraft->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(1, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(2, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(3, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(4, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tblDraft->setHorizontalHeaderItem(5, __qtablewidgetitem23);
        tblDraft->setObjectName("tblDraft");
        tblDraft->setRowCount(0);
        tblDraft->setColumnCount(6);

        vDraft->addWidget(tblDraft);

        hDraftBtns = new QHBoxLayout();
        hDraftBtns->setObjectName("hDraftBtns");
        btnDraftList = new QPushButton(grpDraft);
        btnDraftList->setObjectName("btnDraftList");

        hDraftBtns->addWidget(btnDraftList);

        btnPublishOk = new QPushButton(grpDraft);
        btnPublishOk->setObjectName("btnPublishOk");

        hDraftBtns->addWidget(btnPublishOk);

        spDraftBtns = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hDraftBtns->addItem(spDraftBtns);


        vDraft->addLayout(hDraftBtns);


        vRight->addWidget(grpDraft);

        grpEnded = new QGroupBox(pageRoom);
        grpEnded->setObjectName("grpEnded");
        vEnded = new QVBoxLayout(grpEnded);
        vEnded->setObjectName("vEnded");
        tblEnded = new QTableWidget(grpEnded);
        if (tblEnded->columnCount() < 6)
            tblEnded->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(1, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(2, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(3, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(4, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tblEnded->setHorizontalHeaderItem(5, __qtablewidgetitem29);
        tblEnded->setObjectName("tblEnded");
        tblEnded->setRowCount(0);
        tblEnded->setColumnCount(6);

        vEnded->addWidget(tblEnded);

        hEndedBtns = new QHBoxLayout();
        hEndedBtns->setObjectName("hEndedBtns");
        btnViewEnded = new QPushButton(grpEnded);
        btnViewEnded->setObjectName("btnViewEnded");

        hEndedBtns->addWidget(btnViewEnded);

        spEndedBtns = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        hEndedBtns->addItem(spEndedBtns);


        vEnded->addLayout(hEndedBtns);


        vRight->addWidget(grpEnded);


        hRoomMain->addLayout(vRight);


        vRoom->addLayout(hRoomMain);

        stackedWidget->addWidget(pageRoom);

        verticalLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabHome->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\341\273\250ng d\341\273\245ng \304\221\341\272\245u gi\303\241 (Qt)", nullptr));
        lblTitleConnect->setText(QCoreApplication::translate("MainWindow", "<h2>K\341\272\277t n\341\273\221i t\341\273\233i m\303\241y ch\341\273\247 \304\221\341\272\245u gi\303\241</h2>", nullptr));
        lblHost->setText(QCoreApplication::translate("MainWindow", "M\303\241y ch\341\273\247", nullptr));
        edtHost->setText(QCoreApplication::translate("MainWindow", "127.0.0.1", nullptr));
        lblPort->setText(QCoreApplication::translate("MainWindow", "C\341\273\225ng", nullptr));
        btnConnect->setText(QCoreApplication::translate("MainWindow", "K\341\272\277t n\341\273\221i", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("MainWindow", "Ng\341\272\257t k\341\272\277t n\341\273\221i", nullptr));
        lblConnStatus->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i: Ch\306\260a k\341\272\277t n\341\273\221i", nullptr));
        lblTitleLogin->setText(QCoreApplication::translate("MainWindow", "<h2>\304\220\304\203ng nh\341\272\255p</h2>", nullptr));
        lblUser->setText(QCoreApplication::translate("MainWindow", "T\303\252n \304\221\304\203ng nh\341\272\255p", nullptr));
        lblPass->setText(QCoreApplication::translate("MainWindow", "M\341\272\255t kh\341\272\251u", nullptr));
        btnLogin->setText(QCoreApplication::translate("MainWindow", "\304\220\304\203ng nh\341\272\255p", nullptr));
        btnBackToConnect->setText(QCoreApplication::translate("MainWindow", "Quay l\341\272\241i", nullptr));
        lblAuthStatus->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i: Ch\306\260a \304\221\304\203ng nh\341\272\255p", nullptr));
        lblTitleRooms->setText(QCoreApplication::translate("MainWindow", "<h2>Trang ch\341\273\247 \304\221\341\272\245u gi\303\241</h2>", nullptr));
        lblUserInfo->setText(QCoreApplication::translate("MainWindow", "Xin ch\303\240o: - (vai tr\303\262: -)", nullptr));
        btnLogout->setText(QCoreApplication::translate("MainWindow", "\304\220\304\203ng xu\341\272\245t", nullptr));
        edtRoomSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "T\303\254m ph\303\262ng (theo t\303\252n / m\303\243 / ch\341\273\247 ph\303\262ng)...", nullptr));
        cmbRoomScope->setItemText(0, QCoreApplication::translate("MainWindow", "T\341\272\245t c\341\272\243 ph\303\262ng", nullptr));
        cmbRoomScope->setItemText(1, QCoreApplication::translate("MainWindow", "Ph\303\262ng c\341\273\247a t\303\264i", nullptr));

        cmbRoomStatus->setItemText(0, QCoreApplication::translate("MainWindow", "T\341\272\245t c\341\272\243 tr\341\272\241ng th\303\241i", nullptr));
        cmbRoomStatus->setItemText(1, QCoreApplication::translate("MainWindow", "Ch\341\273\235", nullptr));
        cmbRoomStatus->setItemText(2, QCoreApplication::translate("MainWindow", "\304\220ang \304\221\341\272\245u gi\303\241", nullptr));
        cmbRoomStatus->setItemText(3, QCoreApplication::translate("MainWindow", "\304\220\303\243 k\341\272\277t th\303\272c", nullptr));

        btnRefreshRooms->setText(QCoreApplication::translate("MainWindow", "L\303\240m m\341\273\233i", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tblRooms->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "M\303\243", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tblRooms->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "T\303\252n ph\303\262ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tblRooms->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tblRooms->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i trong ph\303\262ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tblRooms->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Ch\341\273\247 ph\303\262ng", nullptr));
        grpQuickActions->setTitle(QCoreApplication::translate("MainWindow", "Thao t\303\241c nhanh", nullptr));
        edtCreateRoom->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nh\341\272\255p t\303\252n ph\303\262ng \304\221\341\273\203 t\341\272\241o", nullptr));
        btnCreateRoom->setText(QCoreApplication::translate("MainWindow", "T\341\272\241o ph\303\262ng", nullptr));
        edtJoinRoomId->setPlaceholderText(QCoreApplication::translate("MainWindow", "M\303\243 ph\303\262ng ho\341\272\267c t\303\252n ph\303\262ng \304\221\341\273\203 tham gia", nullptr));
        btnJoinRoom->setText(QCoreApplication::translate("MainWindow", "Tham gia", nullptr));
        btnJoinSelected->setText(QCoreApplication::translate("MainWindow", "Tham gia ph\303\262ng \304\221ang ch\341\273\215n", nullptr));
        btnOpenHistory->setText(QCoreApplication::translate("MainWindow", "Xem l\341\273\213ch s\341\273\255", nullptr));
        grpRoomDetails->setTitle(QCoreApplication::translate("MainWindow", "Ph\303\262ng \304\221ang ch\341\273\215n", nullptr));
        lblSelIdTitle->setText(QCoreApplication::translate("MainWindow", "M\303\243", nullptr));
        lblSelRoomId->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSelNameTitle->setText(QCoreApplication::translate("MainWindow", "T\303\252n", nullptr));
        lblSelRoomName->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSelStatusTitle->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i", nullptr));
        lblSelRoomStatus->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSelPartTitle->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i tham gia", nullptr));
        lblSelRoomParticipants->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSelOwnerTitle->setText(QCoreApplication::translate("MainWindow", "Ch\341\273\247 ph\303\262ng", nullptr));
        lblSelRoomOwner->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSelRoomUsersTitle->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i d\303\271ng (\304\221\341\273\203 chat)", nullptr));
        edtSelRoomUserSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "T\303\254m ng\306\260\341\273\235i d\303\271ng...", nullptr));
        lblSelRoomUsersHint->setText(QCoreApplication::translate("MainWindow", "Nh\341\272\245p \304\221\303\272p ng\306\260\341\273\235i d\303\271ng \304\221\341\273\203 m\341\273\237 chat", nullptr));
        lblRoomsStatus->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i: -", nullptr));
        tabHome->setTabText(tabHome->indexOf(tabRooms), QCoreApplication::translate("MainWindow", "Ph\303\262ng \304\221\341\272\245u gi\303\241", nullptr));
        btnRefreshHistory->setText(QCoreApplication::translate("MainWindow", "L\303\240m m\341\273\233i l\341\273\213ch s\341\273\255", nullptr));
        edtHistSearch->setPlaceholderText(QCoreApplication::translate("MainWindow", "T\303\254m trong l\341\273\213ch s\341\273\255 (ph\303\262ng / v\341\272\255t ph\341\272\251m)...", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tblHistory->horizontalHeaderItem(0);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Th\341\273\235i gian", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tblHistory->horizontalHeaderItem(1);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Ph\303\262ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tblHistory->horizontalHeaderItem(2);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "V\341\272\255t ph\341\272\251m", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tblHistory->horizontalHeaderItem(3);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Vai tr\303\262", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tblHistory->horizontalHeaderItem(4);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i b\303\241n", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tblHistory->horizontalHeaderItem(5);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i th\341\272\257ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tblHistory->horizontalHeaderItem(6);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "Gi\303\241", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tblHistory->horizontalHeaderItem(7);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "L\303\275 do", nullptr));
        lblHistoryStatus->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i: -", nullptr));
        tabHome->setTabText(tabHome->indexOf(tabHistory), QCoreApplication::translate("MainWindow", "L\341\273\213ch s\341\273\255", nullptr));
        grpChatUsers->setTitle(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i d\303\271ng (trong ph\303\262ng)", nullptr));
        btnChatUsers->setText(QCoreApplication::translate("MainWindow", "C\341\272\255p nh\341\272\255t", nullptr));
        grpChatBox->setTitle(QCoreApplication::translate("MainWindow", "Tin nh\341\272\257n", nullptr));
        lblChatPeer->setText(QCoreApplication::translate("MainWindow", "\304\220ang chat v\341\273\233i: -", nullptr));
        edtChatMsg->setPlaceholderText(QCoreApplication::translate("MainWindow", "Nh\341\272\255p tin nh\341\272\257n...", nullptr));
        btnChatSend->setText(QCoreApplication::translate("MainWindow", "G\341\273\255i", nullptr));
        tabHome->setTabText(tabHome->indexOf(tabChat), QCoreApplication::translate("MainWindow", "Tr\303\262 chuy\341\273\207n", nullptr));
        grpRoomUsers->setTitle(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i trong ph\303\262ng", nullptr));
        btnRefreshChatUsers->setText(QCoreApplication::translate("MainWindow", "C\341\272\255p nh\341\272\255t", nullptr));
        lblRoomUsersHint->setText(QCoreApplication::translate("MainWindow", "Nh\341\272\245p \304\221\303\272p ng\306\260\341\273\235i d\303\271ng \304\221\341\273\203 chat", nullptr));
        lblRoomTitle->setText(QCoreApplication::translate("MainWindow", "Ph\303\262ng: (ch\306\260a tham gia)", nullptr));
        btnLeaveRoom->setText(QCoreApplication::translate("MainWindow", "R\341\273\235i ph\303\262ng", nullptr));
        btnStartRoom->setText(QCoreApplication::translate("MainWindow", "B\341\272\257t \304\221\341\272\247u", nullptr));
        btnEndRoom->setText(QCoreApplication::translate("MainWindow", "K\341\272\277t th\303\272c", nullptr));
        grpCurrent->setTitle(QCoreApplication::translate("MainWindow", "\304\220\341\272\245u gi\303\241 hi\341\273\207n t\341\272\241i", nullptr));
        lblItemNameTitle->setText(QCoreApplication::translate("MainWindow", "V\341\272\255t ph\341\272\251m", nullptr));
        lblItemName->setText(QCoreApplication::translate("MainWindow", "KH\303\224NG C\303\223", nullptr));
        lblItemSellerTitle->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i b\303\241n", nullptr));
        lblItemSeller->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblItemStartTitle->setText(QCoreApplication::translate("MainWindow", "Gi\303\241 kh\341\273\237i \304\221i\341\273\203m", nullptr));
        lblItemStart->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblItemBuyNowTitle->setText(QCoreApplication::translate("MainWindow", "Mua ngay", nullptr));
        lblItemBuyNow->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblItemPriceTitle->setText(QCoreApplication::translate("MainWindow", "Hi\341\273\207n t\341\272\241i", nullptr));
        lblItemPrice->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblItemLeaderTitle->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i d\341\272\253n \304\221\341\272\247u", nullptr));
        lblItemLeader->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblItemTimeTitle->setText(QCoreApplication::translate("MainWindow", "Th\341\273\235i gian c\303\262n l\341\272\241i", nullptr));
        lblItemTime->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblMinBid->setText(QCoreApplication::translate("MainWindow", "Gi\303\241 t\341\273\221i thi\341\273\203u: -", nullptr));
        edtBidAmount->setPlaceholderText(QCoreApplication::translate("MainWindow", "S\341\273\221 ti\341\273\201n \304\221\341\272\267t gi\303\241", nullptr));
        btnBid->setText(QCoreApplication::translate("MainWindow", "\304\220\341\272\267t gi\303\241", nullptr));
        btnBuyNow->setText(QCoreApplication::translate("MainWindow", "Mua ngay", nullptr));
        lblRoomStatus->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i: -", nullptr));
        grpQueue->setTitle(QCoreApplication::translate("MainWindow", "H\303\240ng ch\341\273\235", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tblQueue->horizontalHeaderItem(0);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "M\303\243", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tblQueue->horizontalHeaderItem(1);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "T\303\252n", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tblQueue->horizontalHeaderItem(2);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Gi\303\241 kh\341\273\237i \304\221i\341\273\203m", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tblQueue->horizontalHeaderItem(3);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Mua ngay", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tblQueue->horizontalHeaderItem(4);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "Th\341\273\235i l\306\260\341\273\243ng", nullptr));
        btnViewQueued->setText(QCoreApplication::translate("MainWindow", "Xem h\303\240ng ch\341\273\235", nullptr));
        grpDraft->setTitle(QCoreApplication::translate("MainWindow", "Nh\303\241p ng\306\260\341\273\235i b\303\241n", nullptr));
        edtDraftName->setPlaceholderText(QCoreApplication::translate("MainWindow", "t\303\252n (kh\303\264ng kho\341\272\243ng tr\341\272\257ng)", nullptr));
        edtDraftStart->setPlaceholderText(QCoreApplication::translate("MainWindow", "gi\303\241 kh\341\273\237i \304\221i\341\273\203m", nullptr));
        edtDraftBuy->setPlaceholderText(QCoreApplication::translate("MainWindow", "mua ngay (0)", nullptr));
        edtDraftDur->setPlaceholderText(QCoreApplication::translate("MainWindow", "th\341\273\235i l\306\260\341\273\243ng (gi\303\242y)", nullptr));
        btnAddDraft->setText(QCoreApplication::translate("MainWindow", "Th\303\252m v\341\272\255t ph\341\272\251m", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tblDraft->horizontalHeaderItem(0);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "M\303\243 nh\303\241p", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = tblDraft->horizontalHeaderItem(1);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "T\303\252n", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = tblDraft->horizontalHeaderItem(2);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "Gi\303\241 kh\341\273\237i \304\221i\341\273\203m", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = tblDraft->horizontalHeaderItem(3);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "Mua ngay", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = tblDraft->horizontalHeaderItem(4);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Th\341\273\235i l\306\260\341\273\243ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = tblDraft->horizontalHeaderItem(5);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "X\303\263a", nullptr));
        btnDraftList->setText(QCoreApplication::translate("MainWindow", "Xem danh s\303\241ch nh\303\241p", nullptr));
        btnPublishOk->setText(QCoreApplication::translate("MainWindow", "OK (B\341\272\257t \304\221\341\272\247u \304\221\341\272\245u gi\303\241)", nullptr));
        grpEnded->setTitle(QCoreApplication::translate("MainWindow", "V\341\272\255t ph\341\272\251m \304\221\303\243 k\341\272\277t th\303\272c", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = tblEnded->horizontalHeaderItem(0);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "M\303\243 v\341\272\255t ph\341\272\251m", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = tblEnded->horizontalHeaderItem(1);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "Tr\341\272\241ng th\303\241i", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = tblEnded->horizontalHeaderItem(2);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "Ng\306\260\341\273\235i th\341\272\257ng", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = tblEnded->horizontalHeaderItem(3);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "Gi\303\241 cu\341\273\221i", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = tblEnded->horizontalHeaderItem(4);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "L\303\275 do", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = tblEnded->horizontalHeaderItem(5);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "K\341\272\277t th\303\272c l\303\272c", nullptr));
        btnViewEnded->setText(QCoreApplication::translate("MainWindow", "Xem \304\221\303\243 k\341\272\277t th\303\272c", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
