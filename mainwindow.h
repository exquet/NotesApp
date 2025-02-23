#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QTextEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QFont>
#include <QToolButton>
#include <QSpinBox>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QListWidget *noteList;
    QTextEdit *noteEditor;
    QMap<QString, QString> notes; // Карта заметок (название -> текст)
    int noteCounter; // счетчик для названия
    QString oldTitle;
    int currentFontSize;
    QMap<QString, QDateTime> noteDates;

    void updateDateLabel(const QString &title);

private slots:
    void onNoteSelected();
    void saveCurrentNote();
    void on_delete_button_clicked();
    void on_add_button_clicked();
    void onNoteTitleChanged(QListWidgetItem *item);
    void onNoteDoubleClicked(QListWidgetItem *item);
    void saveNotesToFile();
    void loadNotesFromFile();
    void increaseFontSize();
    void decreaseFontSize();
    void applyFontSizeToAllText();
    void changeFontSizeSpinBox(int value);
    void updateFontSizeSpinBox();
};
#endif // MAINWINDOW_H
