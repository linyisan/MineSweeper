#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
        ROW=9;
        COL=9;
        BOMBNUM=10;
        isRight=false;
        label_row = new QLabel(this);
        label_row->setText("雷区行数：");
        label_col = new QLabel(this);
        label_col->setText("雷区列数：");
        label_bombNum = new QLabel(this);
        label_bombNum->setText("地雷数目：");
        lineEdit_row = new QLineEdit(this);
        lineEdit_col = new QLineEdit(this);
        lineEdit_bombNum = new QLineEdit(this);
        btn_send = new QPushButton(this);
        btn_send->setText("提交");
        QGridLayout *layout = new QGridLayout(this);
        layout->addWidget(label_row,0,0);
        layout->addWidget(label_col,1,0);
        layout->addWidget(label_bombNum,2,0);
        layout->addWidget(lineEdit_row,0,1);
        layout->addWidget(lineEdit_col,1,1);
        layout->addWidget(lineEdit_bombNum,2,1);
        layout->addWidget(btn_send,3,1);
        connect(btn_send,SIGNAL(clicked()),this,SLOT(getSize()));

}

Dialog::~Dialog()
{
    delete ui;
}

//对话框输入检测
void Dialog::getSize()
{
    QString str;
    str = lineEdit_row->text();//获取行输入框文本
    ROW = str.toInt();//从字符串转换为整数
    str = lineEdit_col->text();//获取列输入框文本
    COL = str.toInt();
    str = lineEdit_bombNum->text();//获取地雷数输入框文本
    BOMBNUM = str.toInt();
    //如果转换失败，toInt()函数会自动赋值为0，以此来检测输入的是否是正整数
    if(ROW==0||COL==0||BOMBNUM==0)
    {
        //弹窗警告
        QMessageBox msg_checkInt(QMessageBox::Warning, "输入错误","请输入正整数！");
        msg_checkInt.exec();
    }
    //检测输入的行列数是否小于简单难度的设定
    else if(ROW<9 || COL<9)
    {
        //弹窗警告
        QMessageBox message(QMessageBox::NoIcon, "too low", "你还想设置的比初级还要简单？不改高点我就开枪了。");
        message.setIconPixmap(QPixmap("./res/low.png"));
        message.exec();
    }
    //检测地雷数
    else if(BOMBNUM>ROW*COL || BOMBNUM>1000)
    {
        //弹窗警告
        QMessageBox message(QMessageBox::Warning,"输入错误","请输入的雷数不要大于地图总格子数或本游戏上限雷数1000！");
        message.exec();
    }
    //正确输入的情况
    else
    {
        this->close();//关闭对话框窗口
        isRight=true;//修改输入合法标志为合法
        return;
    }
        isRight=false;//修改输入合法标志为非法
}


