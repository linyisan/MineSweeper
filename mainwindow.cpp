#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);//禁止最大化

    mBlock = new Block();
    mBlock->Create();
    setFixedSize(mBlock->mCol*BLOCK_WIDTH,mBlock->mRow*BLOCK_HEIGHT+BOARD_HEIGHT);
    mTimer=new QTimer(this);
    connect(mTimer,SIGNAL(timeout()),this,SLOT(on_updateTimer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
//更新游戏用时
void MainWindow::on_updateTimer()
{
    mBlock->mTime++;
    update();
}
//绘图事件
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter *painter = new QPainter(this);//创建画笔对象
    //加载位图
    QPixmap bmpblocks(":/res/blocks.bmp");//方块
    QPixmap bmpblast(":/res/boomblast.bmp");//雷爆炸
    QPixmap bmpnumber(":/res/number.bmp");//计数用红色数字
    QPixmap bmpfaces(":/res/faces.bmp");//表情
    QPixmap bmpflagerror(":/res/flagerror.bmp");//错误标记
   //绘制雷区
    for(int rowBlock=0;rowBlock<mBlock->mRow;rowBlock++)
    {
        for(int colBlock=0;colBlock<mBlock->mCol;colBlock++)
        {
            //根据方块的状态绘制
            switch (mBlock->map[rowBlock][colBlock].blockState) {
            case BS_CLOSE://未打开  10 第十个
                painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblocks,BLOCK_WIDTH*10,0,BLOCK_WIDTH,BLOCK_HEIGHT);;
                break;
            case BS_MARKED://标记
                painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblocks,BLOCK_WIDTH*11,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                break;
            case BS_OPEN://已打开的数字格子
                painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblocks,BLOCK_WIDTH*mBlock->map[rowBlock][colBlock].value,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                break;
            case BS_BOMB_BLAST://中雷
                painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblast,0,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                break;
            case BS_MARKED_ERROR://错误标记
                //若是游戏未结束则显示旗子
                if(mBlock->gameState==GS_FAULT||mBlock->gameState==GS_PLAYING)
                    painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblocks,BLOCK_WIDTH*11,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                //若是游戏已结束则显示错误标记
                else if(mBlock->gameState==GS_DEAD)
                    painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpflagerror,0,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                break;
            case BS_BOMB_LIVE://未挖的雷
                painter->drawPixmap(colBlock*BLOCK_WIDTH,rowBlock*BLOCK_HEIGHT+BOARD_HEIGHT,bmpblocks,BLOCK_WIDTH*9,0,BLOCK_WIDTH,BLOCK_HEIGHT);
                break;
            default:
                break;
            }
        }
    }
    //绘制表情
    switch (mBlock->gameState) {
    case GS_PLAYING://12是表情宽度的一半
        painter->drawPixmap(mBlock->mCol * BLOCK_WIDTH / 2 - EXPRESSION_WIDTH / 2, MENU_HEIGHT + 7, bmpfaces, 1 * EXPRESSION_WIDTH, 0, EXPRESSION_WIDTH, EXPRESSION_HEIGHT);
        break;
    case GS_WIN:
        painter->drawPixmap(mBlock->mCol * BLOCK_WIDTH  / 2 - EXPRESSION_WIDTH / 2, MENU_HEIGHT + 7, bmpfaces, 2 * EXPRESSION_WIDTH, 0, EXPRESSION_WIDTH, EXPRESSION_HEIGHT);
        break;
    case GS_DEAD:
        painter->drawPixmap(mBlock->mCol * BLOCK_WIDTH / 2 - EXPRESSION_WIDTH / 2, MENU_HEIGHT + 7, bmpfaces, 0 * EXPRESSION_WIDTH, 0, EXPRESSION_WIDTH, EXPRESSION_HEIGHT); // 24是笑脸的边长,锚点在左上，因为工具栏占了些，所以看起来不再中间
        break;
    default:
        painter->drawPixmap(mBlock->mCol * BLOCK_WIDTH / 2 - EXPRESSION_WIDTH / 2, MENU_HEIGHT + 7, bmpfaces, 1 * EXPRESSION_WIDTH, 0, EXPRESSION_WIDTH, EXPRESSION_HEIGHT);
        break;
    }
    //绘制剩余的雷数
    int rBombNum=mBlock->restBombNum;
    if(rBombNum<0) rBombNum=0;
        painter->drawPixmap(6,5+MENU_HEIGHT,bmpnumber,rBombNum/100*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
    if(rBombNum>=100) rBombNum%=100;
        painter->drawPixmap(6+ELENUM_WIDTH,5+MENU_HEIGHT,bmpnumber, rBombNum/10*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
        painter->drawPixmap(6+ELENUM_WIDTH*2,5+MENU_HEIGHT,bmpnumber,rBombNum%10*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
    //绘制当局游戏用时
    int rTime=mBlock->mTime;
    if(rTime>=1000)rTime%=1000;
        painter->drawPixmap(mBlock->mCol*ELENUM_WIDTH-66,5+MENU_HEIGHT,bmpnumber,rTime/100*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
    if(rTime>=100) rTime%=100;
     painter->drawPixmap(mBlock->mCol*ELENUM_WIDTH-6-ELENUM_WIDTH*2,5+MENU_HEIGHT,bmpnumber,rTime/10*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
     painter->drawPixmap(mBlock->mCol*ELENUM_WIDTH-6-ELENUM_WIDTH,5+MENU_HEIGHT,bmpnumber,rTime%10*ELENUM_WIDTH,0,ELENUM_WIDTH,ELENUM_HEIGHT);
}


//鼠标事件
//用户鼠标点击，判断左右键，确定落点，修改块信息，确定是否结束
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int click_x = event->x();
    int click_y = event->y();
    //先判断是否左键点击了笑脸
    if(event->buttons()==(Qt::LeftButton)
            && click_x>(mBlock->mCol*BLOCK_WIDTH/2-EXPRESSION_WIDTH/2)
            && click_x<(mBlock->mCol*BLOCK_WIDTH/2+EXPRESSION_WIDTH/2)
            && click_y<(7+MENU_HEIGHT+EXPRESSION_HEIGHT)
            && click_y>(7+MENU_HEIGHT))
    {
        mBlock->Restart();//重新开始游戏
        if(mTimer->isActive())
            mTimer->stop();//停止计时
        update();//更新界面
        return;
    }
    //落点在雷图
    else if(click_y<(BLOCK_HEIGHT*mBlock->mRow+BOARD_HEIGHT)
            &&click_y>(BOARD_HEIGHT))
    {
        //游戏结束的情况下，禁止点击方块
        if(mBlock->gameState != GS_WIN && mBlock->gameState != GS_DEAD)
        {
            //开始计时
            if(!(mTimer->isActive()))
                mTimer->start(1000);
            //确定落点(行列)
            int colBlock = click_x/BLOCK_WIDTH;//确定落点所在行
            int rowBlock = (click_y-BOARD_HEIGHT)/BLOCK_HEIGHT;//确定落点所在列
            if(event->button()==(Qt::LeftButton))//若按下的是鼠标左键
                mBlock->OpenBlock(rowBlock,colBlock);//进行打开方块处理

            else if(event->button()==(Qt::RightButton))//若按下的是鼠标右键
                mBlock->MarkBomb(rowBlock,colBlock);//进行标记方块处理
        }
    }
    //落点在菜单栏
    else if(click_y<=MENU_HEIGHT) return;
    mBlock->CheckGame();
    //游戏结束时，停止计时
    if(mBlock->gameState==GS_WIN || mBlock->gameState==GS_DEAD)
    {
        if(mTimer->isActive())
            mTimer->stop();
    }
    update();//更新界面
}


void MainWindow::on_actionReStart_triggered()
{
    mBlock->Restart();
    update();
}
//初级
void MainWindow::on_actionPrimary_triggered()
{
    mBlock->Create(9,9,10);
    setFixedSize(mBlock->mCol*BLOCK_WIDTH,mBlock->mRow*BLOCK_HEIGHT+BOARD_HEIGHT);
}
//中级
void MainWindow::on_actionInternediate_triggered()
{
    mBlock->Create(16,16,40);
    setFixedSize(mBlock->mCol*BLOCK_WIDTH,mBlock->mRow*BLOCK_HEIGHT+BOARD_HEIGHT);
}
//高级
void MainWindow::on_actionHigh_triggered()
{
    mBlock->Create(16,30,99);
    setFixedSize(mBlock->mCol*BLOCK_WIDTH,mBlock->mRow*BLOCK_HEIGHT+BOARD_HEIGHT);
}
//用户自定义难度
void MainWindow::on_actionUser_Define_triggered()
{
    Dialog *dialog = new Dialog();
    dialog->exec();//显示模态对话框
    //检测输入合法标志是否合法
    if(true==dialog->isRight)
    {
        mBlock->Create(dialog->ROW,dialog->COL,dialog->BOMBNUM);//按输入参数创建雷图
        setFixedSize(mBlock->mCol*BLOCK_WIDTH,mBlock->mRow*BLOCK_HEIGHT+BOARD_HEIGHT);//固定程序窗体尺寸，使之不可拉伸
    }
}
//退出
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionHow_to_play_triggered()
{
    QMessageBox message(QMessageBox::NoIcon, "so easy", "扫雷游戏目标是在最短的时间内根据点击格子出现的数字找出所有非雷格子，同时避免踩雷，踩到一个雷即全盘皆输。\n点开的数字是几，则说明该数字旁边的8个位置中有几个雷。");
    message.exec();
}
