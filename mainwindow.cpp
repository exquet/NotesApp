#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , noteCounter(1) // инициализация переменной из header
{
    ui->setupUi(this);

    noteList = findChild<QListWidget *>("listWidget");
    noteEditor = findChild<QTextEdit *>("textEdit");

    connect(noteList, &QListWidget::itemClicked, this, &MainWindow::onNoteSelected);
    connect(noteEditor, &QTextEdit::textChanged, this, &MainWindow::saveCurrentNote);

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

void MainWindow::on_delete_button_clicked()
{
    QListWidgetItem *item = noteList->currentItem();
    if (item) {
        QString title = item->text();
        notes.remove(title); // Удаляем заметку из QMap

        // Удаляем заметку из QListWidget
        delete noteList->takeItem(noteList->row(item));

        if (noteList->count() > 0){
            QString newTitle = noteList->currentItem()->text();
            noteEditor->setText(notes[newTitle]); // отображаем текст новой заметки
        }
        else {noteEditor->clear();}
    }
}


void MainWindow::on_add_button_clicked()
{
    QString newNoteTitle = "New Note " + QString::number(noteCounter++); // счетчик для названия
    notes[newNoteTitle] = ""; // создание новой Qmap
    noteList->addItem(newNoteTitle); // добавление item в QListWidget
    noteList->setCurrentRow(noteList->count() - 1); // выбираем новую заметку в списке (size - 1 = last item)
    noteEditor->clear();
    noteEditor->setFocus();
}

