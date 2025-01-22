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

    notes["Example Note 1"] = "This is the first example note."; // тестовые заметки через map
    notes["Example Note 2"] = "This is the second example note.";

    for (const QString &title : notes.keys()) { // проход по всем ключам(title) в карте(notes)
        noteList->addItem(title); // добавление названий в QListWidget
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNoteSelected(){
    QListWidgetItem *item = noteList->currentItem(); // получение текущей заметки
    if (item) {
        QString title = item->text(); // получние названия заметки
        noteEditor->setText(notes[title]); // устанавливает текст заметки (из notes) в текстовый редактор
    }
}

void MainWindow::saveCurrentNote(){
    QListWidgetItem *item = noteList->currentItem();
    if (item) {
        QString title = item->text();
        notes[title] = noteEditor->toPlainText(); // cохраняет текст из текстового редактора в notes по ключу(название)
    }
}
