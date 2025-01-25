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
    connect(noteList, &QListWidget::itemChanged, this, &MainWindow::onNoteTitleChanged);
    connect(noteList, &QListWidget::itemDoubleClicked, this, &MainWindow::onNoteDoubleClicked);

    for (int i = 0; i < noteList->count(); ++i) {  // делаем все элементы QListWidget редактируемыми
        QListWidgetItem *item = noteList->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }

    for (const QString &title : notes.keys()) { // проход по всем ключам(title) в карте(notes)
        noteList->addItem(title); // добавление названий в QListWidget
    }

    loadNotesFromFile();
}

MainWindow::~MainWindow()
{
    saveNotesToFile();
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

void MainWindow::onNoteTitleChanged(QListWidgetItem *item)
{
    if (!item) {
        return; // если элемент не выбран, ничего не делаем
    }

    QString newTitle = item->text(); // получаем новое название заметки введенное пользователем

    if (newTitle.isEmpty()) { // введено пустая строка
        QMessageBox::warning(this, tr("Ошибка"), tr("Название заметки не может быть пустым!"));
        item->setText(oldTitle); // возвращает старое название заметки (oldTitle) в элемент
        return;
    }

    if (notes.contains(newTitle)) { // проверяет, существует ли заметка с названием newTitle
        QMessageBox::warning(this, tr("Ошибка"), tr("Заметка с таким названием уже существует!"));
        item->setText(oldTitle); // Возвращаем старое название
        return;
    }

    QString noteContent = notes[oldTitle]; // сохраняем текст заметки
    notes.remove(oldTitle); // удаляем старую заметку
    notes[newTitle] = noteContent; // добавляем заметку с новым названием и старым текстом

    if (noteList->currentItem() == item) { // отображения текста измененной заметки если он выбранна
        noteEditor->setText(notes[newTitle]);
    }
}
void MainWindow::onNoteDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        oldTitle = item->text(); // сохраняем старое название перед редактированием
    }
}


void MainWindow::on_add_button_clicked()
{
    QString newNoteTitle = "New Note " + QString::number(noteCounter++); // счетчик для названия
    notes[newNoteTitle] = ""; // добавление новой заметки в QMap
    QListWidgetItem *newItem = new QListWidgetItem(newNoteTitle); // создание нового элемента для QListWidget


    /*  Делаем элемент редактируемым
        newItem->flags() возвращает текущие флаги элемента(возможность выбора, возможность перетаскивания и т.д.)
        Qt::ItemIsEditable — это флаг, который позволяет редактировать текст элемента
        | - добавляет флаг Qt::ItemIsEditable к текущим флагам элемента.
    */
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);


    // Добавляем элемент в QListWidget
    noteList->addItem(newItem);
    noteList->setCurrentRow(noteList->count() - 1); // выбираем новую заметку в списке (size - 1 = last item)
    noteEditor->clear();
    noteEditor->setFocus();
}

void MainWindow::saveNotesToFile(){
    QFile file("notes.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // Открываем файл для записи
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл для записи!"));
        return;
    }
    QTextStream out(&file);

    for (const QString &title : notes.keys()) { // Проходим по всем заметкам
        out << title << "|" << notes[title] << "\n"; // Записываем название и текст заметки
    }

    file.close();
}

void MainWindow::loadNotesFromFile(){
    QFile file("notes.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // Открываем файл для чтения
        // Если файл не существует, просто выходим из метода
        return;
    }

    QTextStream in(&file); // Создаем поток для чтения из файла
    while (!in.atEnd()) { // Читаем файл до конца
        QString line = in.readLine(); // Читаем строку
        QStringList parts = line.split("|"); // Разделяем строку на название и текст заметки
        if (parts.size() == 2) { // Проверяем, что строка содержит и название, и текст
            QString title = parts[0];
            QString content = parts[1];
            notes[title] = content; // Добавляем заметку в QMap
            noteList->addItem(title); // Добавляем название заметки в QListWidget
        }
    }

    file.close(); // Закрываем файл
}
