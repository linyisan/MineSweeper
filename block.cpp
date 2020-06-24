#include "block.h"
#include<time.h>
#include <iostream>

Block::Block()
    : mRow(9),
      mCol(9),
      totalBombNum(10),
      mTime(0),
      map(NULL),
      gameState(GS_PLAYING)
{
    audioClick=new QSound(":/res/click.wav");
    audioBomb=new QSound(":/res/bomb.wav");
    audioOpenzero=new QSound(":/res/openzero.wav");
}

Block::~Block()
{
    delete audioClick;
    delete audioBomb;
    delete audioOpenzero;
    delete map;
}
//创建雷图
void Block::Create(int row,int col,int bombNum)
{
    //先清空原来的游戏图
    if(map!=NULL) //雷图指针map不为空
    {
        for(int i=0;i<mRow;i++)
        {
            delete map[i];//释放雷图指针map[i]所指向的堆空间
        }
        delete map;//释放雷图指针map所指向的堆空间
        map=NULL;//把雷图指针map设置成空指针
    }
    //设置游戏参数
    mRow=row;
    mCol=col;
    totalBombNum=bombNum;
    restBombNum=bombNum;
    mTime=0;
    gameState = GS_PLAYING;
    //初始化雷图
    map = new BombBlock *[mRow];//开辟一个数量为mRow、存放结构体BlombBlock指针的空间
    for(int rowBlock=0;rowBlock<mRow;rowBlock++)
    {
        map[rowBlock]=new BombBlock[mCol];//开辟一个数量为mCol、存放结构体BlombBlock的空间
        for(int colBlock=0;colBlock<mCol;colBlock++)
        {
            BombBlock bombBlock;                //声明一个方块
            bombBlock.blockState = BS_CLOSE;    //方块默认未打开
            bombBlock.value = 0;                //方块默认是空白方块
            map[rowBlock][colBlock]=bombBlock;  //把方块放入雷图中
        }
    }
    //随机布雷
    srand((unsigned)time(NULL));    //设置随机种子
    for(int n=0;n<totalBombNum;)    //直至埋下所有雷
    {
        int rowBlock = rand()%mRow;     //随机行 0~mRow-1
        int colBlock = rand()%mCol;     //随机列 0~mCol-1
        if(map[rowBlock][colBlock].value != -1) //该方块不是地雷
        {
            map[rowBlock][colBlock].value = -1; //修改方块的状态为雷
            n++;                        //成功埋下一颗雷
        }
    }
    //计算数字方块的数字
    for(int rowBlock=0;rowBlock<mRow;rowBlock++)//遍历整个雷区
    {
        for(int colBlock=0;colBlock<mCol;colBlock++)
        {
            // 根据相邻的八个方块（在地图范围内）的雷数量进行数目叠加
            // y为行偏移量，x为列偏移量
            // 前提条件是本方块不是雷
            if(map[rowBlock][colBlock].value != -1)
            {
                for(int y=-1;y<=1;y++)       //列
                    for(int x=-1;x<=1;x++)   //行
                        if(colBlock+x>=0                 //越界处理
                                && colBlock+x<mCol
                                && rowBlock+y>=0
                                && rowBlock+y<mRow
                                && !(x==0&&y==0)               //排除自身
                                && map[rowBlock+y][colBlock+x].value == -1)     //非雷块
                            map[rowBlock][colBlock].value++;
            }
        }
    }
}
//检查当前游戏状态
void Block::CheckGame()
{
    switch (gameState) {//判断游戏状态
    case GS_DEAD://游戏失败
        for(int rowBlock=0;rowBlock<mRow;rowBlock++)
        {                                                               //显示所有雷
            for(int colBlock=0;colBlock<mCol;colBlock++)
            {
                if(map[rowBlock][colBlock].value == -1                          //该方块是雷
                        && map[rowBlock][colBlock].blockState != BS_BOMB_BLAST  //非爆炸状态的雷
                        && map[rowBlock][colBlock].blockState != BS_MARKED)     //非正确标记的雷
                    map[rowBlock][colBlock].blockState = BS_BOMB_LIVE;  //修改方块的状态为未标记的雷
            }
        }
        break;
    case GS_FAULT:      //有标记错误，继续游戏
    case GS_WIN:        //游戏胜利
        break;
    default:            //当没有标记错误的情况下，检测所有的雷，若都已经打开或标记，则游戏胜利
        for(int rowBlock=0;rowBlock<mRow;rowBlock++)
            for(int colBlock=0;colBlock<mCol;colBlock++)
            {
                if(map[rowBlock][colBlock].blockState==BS_CLOSE)
                    return;
            }
        gameState=GS_WIN;//改变游戏状态为游戏胜利
        break;
    }
}
//打开方块
void Block::OpenBlock(int row, int col)
{
    //若游戏结束或已经打开、已标记的方块，则禁止继续点击方块
    if(gameState==GS_DEAD
            || gameState==GS_WIN
            || map[row][col].blockState==BS_OPEN
            || map[row][col].blockState==BS_MARKED
            || map[row][col].blockState==BS_MARKED_ERROR)
        return;
    //下面均是未打开、未标记方块的情况
    //打开的是雷方块
    else if(map[row][col].value==-1)
    {
        audioBomb->play();                          //播放雷爆炸音效
        map[row][col].blockState=BS_BOMB_BLAST;     //修改方块为雷爆炸状态
        gameState=GS_DEAD;                          //修改游戏状态为失败
        return;
    }
    //打开的是数字方块
    else if(map[row][col].value>0)
    {
        audioClick->play();                        //播放打开数字音效
        map[row][col].blockState=BS_OPEN;           //修改方块为打开状态
        return;
    }
    //打开的是空白方块，自动打开周围非雷方块(空白方块周围没有雷)
    else if(map[row][col].value==0)
    {
        audioOpenzero->play();                       //播放打开空白方块音效
        map[row][col].blockState=BS_OPEN;           //修改方块为打开状态
        for(int rowBlock=row-1;rowBlock<=row+1;rowBlock++)  //遍历相邻的方块
            for(int colBlock=col-1;colBlock<=col+1;colBlock++)
                if(rowBlock>=0                          //越界检测
                        && rowBlock<mRow
                        && colBlock>=0
                        && colBlock<mCol)
                    OpenBlock(rowBlock,colBlock);       //递归挖雷
    }
}

//标记方块
void Block::MarkBomb(int row, int col)
{
    //若游戏结束或已经打开的方块，则禁止继续点击方块
    if(gameState==GS_DEAD
            || gameState==GS_WIN
            || map[row][col].blockState==BS_OPEN)
        return;
    //标记
    if(map[row][col].blockState==BS_CLOSE)  //若当前方块未打开
    {
        if(map[row][col].value != -1)       //若当前方块是雷方块
        {
            map[row][col].blockState=BS_MARKED_ERROR; //修改当前方块的状态为错误标记
            gameState=GS_FAULT;                       //修改当前游戏状态为错误标记
        }
        else                                          //若当前不是雷方块
            map[row][col].blockState=BS_MARKED;       //修改当前方块的状态为已标记
        restBombNum--;                                //当前剩余雷数减一
    }
    //取消标记
    else if(map[row][col].blockState==BS_MARKED       //若当前方块处于已标记或错误标记状态
            || map[row][col].blockState==BS_MARKED_ERROR)
    {
        map[row][col].blockState=BS_CLOSE;             //修改当前方块的状态为未打开
        gameState=GS_PLAYING;                          //修改当前游戏状态为游戏进行中
        restBombNum++;                                 //当前剩余雷数加一
    }
}

//按当前地图大小重新开始游戏
void Block::Restart()
{
    Create(this->mRow,this->mCol,this->totalBombNum);
}
