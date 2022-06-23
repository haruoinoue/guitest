#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString fileload;
    QString route;
    QString loadout;
    QString position;
    int f1d2;
    //bool input;
    bool output;
    int amount;

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_lineEdit_2_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QString identifying(QString rawload);
    //bool discontinue();
    void filefound(QString dirload);
    void filecheck(QString fileinfo);
    //QString get_position(){return position;};
};
#endif // MAINWINDOW_H
