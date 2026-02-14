#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "login_controller.h"

#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginController = new Login_Controller(this); // Initialize Login_Controller

    ui->username_LineEdit->setFocus(); // Focus on username field
    ui->Login_Button->setDefault(true); // Set login button as default

    connect(ui->username_LineEdit, &QLineEdit::returnPressed, [=]() {
        ui->password_LineEdit->setFocus(); // Move focus to password
    });
      // Trigger login on Enter in password
    connect(ui->password_LineEdit, &QLineEdit::returnPressed, ui->Login_Button, &QPushButton::click);

    QRegularExpression passwordRegex("^[^\\s]+$"); // Prevent space in password
    QRegularExpressionValidator *passwordValidator = new QRegularExpressionValidator(passwordRegex, this);
    ui->password_LineEdit->setValidator(passwordValidator);

    setWindowFlag(Qt::FramelessWindowHint); // Set frameless window
    setAttribute(Qt::WA_TranslucentBackground, true); // Set transparent background

    ui->password_LineEdit->setEchoMode(QLineEdit::Password); // Hide password characters

    qDebug() << "Available SQL drivers:" << QSqlDatabase::drivers(); // Check available database drivers

    db = QSqlDatabase::addDatabase("QMYSQL"); // Connect to MySQL database
    db.setHostName(""); // host
    db.setPort("");
    db.setDatabaseName("student_mang_sys");
    db.setUserName(""); // username
    db.setPassword("");

    if (!db.open()) {
        qDebug() << "DB Connection failed: " << db.lastError().text(); // Log DB connection failure
        QMessageBox::critical(this, "Database Connection Error", "Failed to connect to database: " + db.lastError().text());
    } else {
        qDebug() << "DB Connected Successfully!"; // Log DB connection success
    }
      // Call login logic from controller
    connect(ui->Login_Button, &QPushButton::clicked, this, [=]() {
        loginController->performLogin(db, ui->username_LineEdit, ui->password_LineEdit, this);
    });
    // CANCEL BUTTON
    // Connect Cancel button to close window
    //connect(ui->Cancel_Button, &QPushButton::clicked, this, &QMainWindow::close);

      // Handle successful login
    connect(loginController, &Login_Controller::loginSuccessful, this, [=]() {
        qDebug() << "Login successful for user:";

    });
    // Handle failed login
    connect(loginController, &Login_Controller::loginFailed, this, [=](const QString &errorMessage) {
        qDebug() << "Login attempt failed with error:" << errorMessage;
    });
}

MainWindow::~MainWindow()
{ // Clean up UI resources
    delete ui;
}
