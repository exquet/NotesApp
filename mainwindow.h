#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QTextEdit>
#include <QMainWindow>

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

private slots:
    void onNoteSelected();
    void saveCurrentNote();

};
#endif // MAINWINDOW_H
