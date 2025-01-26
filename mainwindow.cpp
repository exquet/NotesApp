#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , noteCounter(1) // инициализация переменной из header
    , currentFontSize(12)
{
    ui->setupUi(this);

    noteList = findChild<QListWidget *>("listWidget");
    noteEditor = findChild<QTextEdit *>("textEdit");


    connect(noteList, &QListWidget::itemClicked, this, &MainWindow::onNoteSelected);
    connect(noteEditor, &QTextEdit::textChanged, this, &MainWindow::saveCurrentNote);
    connect(noteList, &QListWidget::itemChanged, this, &MainWindow::onNoteTitleChanged);
    connect(noteList, &QListWidget::itemDoubleClicked, this, &MainWindow::onNoteDoubleClicked);
    connect(ui->increaseFontButton, &QToolButton::clicked, this, &MainWindow::increaseFontSize);
    connect(ui->decreaseFontButton, &QToolButton::clicked, this, &MainWindow::decreaseFontSize);


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
    QFile file("notes.txt");  // создание файла
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {// открываем файл для записи, с флагом записи текста
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл для записи!"));
        return;
    }
    QTextStream out(&file);

    for (const QString &title : notes.keys()) { // проходим по всем заметкам
        out << title << "|" << notes[title] << "\n"; // название | текст \n
    }

    file.close();
}

void MainWindow::loadNotesFromFile(){
    QFile file("notes.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // открываем файл для чтения, с флагом записи текстом
        return;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split("|"); // разделяем строку на части с помощью символа |
        if (parts.size() == 2) { // проверяем, что строка содержит и название, и текст
            QString title = parts[0]; // назавание
            QString content = parts[1]; // текст
            notes[title] = content;
            noteList->addItem(title);
        }
    }

    file.close();
}

void MainWindow::increaseFontSize(){
    if (currentFontSize < 64) {
        currentFontSize++;
        applyFontSizeToAllText();
    }
}

void MainWindow::decreaseFontSize(){
    if (currentFontSize > 6) {
        currentFontSize--;
        applyFontSizeToAllText();
    }
}

void MainWindow::applyFontSizeToAllText(){
    QFont font = noteEditor->font();
    font.setPointSize(currentFontSize);
    noteEditor->setFont(font); // применение к QTextEdit
    noteEditor->setPlainText(noteEditor->toPlainText()); // обновление текста
}
