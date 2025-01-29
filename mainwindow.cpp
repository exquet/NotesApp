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
    connect(ui->fontSize_SpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::changeFontSizeSpinBox);

    ui->fontSize_SpinBox->setValue(currentFontSize);
    ui->fontSize_SpinBox->setMinimum(1);

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
        updateDateLabel(title);
    }
}

void MainWindow::saveCurrentNote(){
    QListWidgetItem *item = noteList->currentItem();
    if (item) {
        QString title = item->text();
        notes[title] = noteEditor->toPlainText(); // cохраняет текст из текстового редактора в notes по ключу(название)
        noteDates[title] = QDateTime::currentDateTime(); // Обновляем дату последнего изменения
        updateDateLabel(title); // Обновляем дату на QLabel
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
    noteDates[newNoteTitle] = QDateTime::currentDateTime(); // Устанавливаем текущую дату и время
    QListWidgetItem *newItem = new QListWidgetItem(newNoteTitle); // создание нового элемента для QListWidget


    /*  Делаем элемент редактируемым
        newItem->flags() возвращает текущие флаги элемента(возможность выбора, возможность перетаскивания и т.д.)
        Qt::ItemIsEditable — это флаг, который позволяет редактировать текст элемента
        | - добавляет флаг Qt::ItemIsEditable к текущим флагам элемента.
    */
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);


    // добавляем элемент в QListWidget
    noteList->addItem(newItem);
    noteList->setCurrentRow(noteList->count() - 1); // выбираем новую заметку в списке (size - 1 = last item)
    noteEditor->clear();
    noteEditor->setFocus();
    updateDateLabel(newNoteTitle);
}

void MainWindow::saveNotesToFile(){
    QFile file("notes.txt");  // создание файла
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {// открываем файл для записи, с флагом записи текста
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл для записи!"));
        return;
    }
    QTextStream out(&file);

    for (const QString &title : notes.keys()) {
        // заменяем все переносы строк в тексте на специальную метку "\n" -> "\\n"
        QString content = notes[title].replace("\n", "\\n");
        out << title << "|" << content << "|" << noteDates[title].toString(Qt::ISODate) << "\n";
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
        QStringList parts = line.split("|");
        if (parts.size() == 3) {
            QString title = parts[0];
            // Восстанавливаем переносы строк из метки "\\n" -> "\n"
            QString content = parts[1].replace("\\n", "\n");
            QDateTime dateTime = QDateTime::fromString(parts[2], Qt::ISODate);
            notes[title] = content;
            noteDates[title] = dateTime;
            noteList->addItem(title);
        }
    }
    file.close();
}

void MainWindow::increaseFontSize(){
    if (currentFontSize < 99) {
        currentFontSize++;
        applyFontSizeToAllText();
        updateFontSizeSpinBox();
    }
}

void MainWindow::decreaseFontSize(){
    if (currentFontSize > 1) {
        currentFontSize--;
        applyFontSizeToAllText();
        updateFontSizeSpinBox();
    }
}

void MainWindow::applyFontSizeToAllText(){
    QFont font = noteEditor->font();
    font.setPointSize(currentFontSize);
    noteEditor->setFont(font); // применение к QTextEdit
    noteEditor->setPlainText(noteEditor->toPlainText()); // обновление текста
}

void MainWindow::changeFontSizeSpinBox(int value){
    currentFontSize = value;
    applyFontSizeToAllText();
}

void MainWindow::updateFontSizeSpinBox(){
    ui->fontSize_SpinBox->setValue(currentFontSize);
}

void MainWindow::updateDateLabel(const QString &title) {
    if (noteDates.contains(title)) {
        QDateTime dateTime = noteDates[title];
        QString dateString = dateTime.toString("dd.MM.yyyy"); // Форматируем дату в нужный формат
        ui->dateLabel->setText(dateString); // устанавливаем текст на QLabel
    }
    else {
        ui->dateLabel->clear(); // если даты нет, очищаем QLabel
    }
}
