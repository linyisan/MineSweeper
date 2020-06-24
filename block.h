#ifndef Block_H
#define Block_H

#include "stdlib.h"
#include <QSound>

// ---------------- 全局定义变量 ---------------- //
//游戏状态
enum GameState
{
    GS_PLAYING, //游戏进行中
    GS_WIN,     //游戏胜利
    GS_DEAD,    //游戏失败
    GS_FAULT    //错误标记，可作为游戏胜利的判断条件
};

//方块状态
enum BlockState
{
    BS_OPEN,         //未翻开
    BS_CLOSE,        //翻开
    BS_MARKED,       //已标记
    BS_MARKED_ERROR, //错误标记
    BS_BOMB_BLAST,   //雷爆炸状态
    BS_BOMB_LIVE     //未标记的雷，
};

//方块属性
struct BombBlock
{
    BlockState blockState;  //当前状态
    int value;             //雷数量 0~8 -1表示雷
};
//雷图
class Block
{
public:
    Block();
    ~Block();
    void Create(int row=9,int col=9,int bombNum=10);       //创建指定一定大小的雷图
    void OpenBlock(int,int);        //打开方块
    void MarkBomb(int,int);         //标记方块
    void CheckGame();               //根据当前游戏状态进行相应处理
    void Restart();                 //按当前地图大小重新开始游戏
public:
    int mRow;                //雷图行数
    int mCol;                //雷图列数
    int totalBombNum;       //地雷总数
    int restBombNum;        //剩余地雷数量
    int mTime;               //当局已进行游戏时间
    GameState gameState;    //当前游戏状态
    BombBlock **map;              //雷图指针
    QSound *audioClick;         //点击声效指针
    QSound *audioBomb;          //雷爆炸声效指针
    QSound *audioOpenzero;      //打开空白方块声效指针
};

#endif // BLOCK_H
