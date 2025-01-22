#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    noteList = findChild<QListWidget *>("listWidget");
    noteEditor = findChild<QTextEdit *>("textEdit");

    connect(noteList, &QListWidget::itemClicked, this, &MainWindow::onNoteSelected);
    connect(noteEditor, &QTextEdit::textChanged, this, &MainWindow::saveCurrentNote);
}

MainWindow::~MainWindow()
{
    delete ui;
}
