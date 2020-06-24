#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include <mainwindow.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();


private:
    Ui::Dialog *ui;
    QLabel *label_row, *label_col, *label_bombNum;
    QLineEdit *lineEdit_row, *lineEdit_col, *lineEdit_bombNum;
    QPushButton *btn_send;
public:
    int ROW,COL,BOMBNUM;//输入的行，列，地雷数
    bool isRight;   //输入合法标志

private slots:
   void getSize();//获取并检测对话框中用户输入的参数
};

#endif // DIALOG_H
