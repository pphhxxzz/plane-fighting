#pragma warning(disable:4244)
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include <string>
#include <easyx.h>
#include <sstream>
#include <algorithm>
#include <conio.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define PLAYERHARM 120
#define PLAYERSPEED 1
#define BOSSHP 2000
#define PI 3.14159265
#define PLAYERBULLETSPEED 4
using namespace std;


// ------------------ 数据设计 ---------------------
// 音效
static int sound = 1;

// 游戏难度
static int difficulty = 1;

static int music_num = 1;   // 记录音乐控制键的点击次数

// 积分:包含具体得分。
static int score = 0;

// 存储三种难度的积分
static int allscore[3] = { 0 };

// 用户昵称
static wchar_t name[20]=L"null";

//// 存活时间
static int Time;

// 位置结构体
/* 使用圓来定义飞机、子弹、道具的位置 */

 struct Rect
{
    float x;
    float y;
    int r;
};


// 玩家战机类
 class Player {
public:
    Rect pos;
    int hp;// 生命值
    int harm;
    float speed;// 速度，有固定初始值，获得道具可以修改
    time_t time1;
    int score;
    int firespeed; //越少越快

    Player(int _posx, int _posy, int _r, int _hp, float _speed, int harm);// 构造函数，初始化pos结构体，生命值，速度,伤害


    /*
    功能：获取键盘输入移动战机
    参数：char
    返回值：void
    */
    void Move(int dir);

    /*
    功能：自动开火，获得道具可以改变子弹速度、伤害
    参数：void
    返回值：void
    */
    void Fire(int);

    /*
    功能：遍历敌机子弹数组，检测是否有碰撞，如果有碰撞，销毁子弹并根据敌机子弹伤害扣除生命值并判定是否死亡
    参数：void
    返回值：void
    */
    void BeHurt();  // 碰撞到敌机子弹

    /*
    功能：遍历道具数组，检测是否有碰撞，如果有碰撞，销毁道具并道具编号判定战机属性应该如何修改
    参数：void
    返回值：void
    */
    void GetItem();

    //判斷有沒有敵機相撞
    void crash();
};

// 敌机类
 class Enemy {
public:
    int hp;     // 血量
    float speed;
    Rect pos;   // 出生位置
    int harm;
    int life;
public:
    Enemy(int x, int r, int life);
};


 //记录游玩信息
 struct PlayerMsg
 {
 public:
     wstring name;// 用户昵称
     int rank;//排名
     int score;// 积分:包含具体得分
     int difficulty;// 游戏难度
     int time;//存活时间

     PlayerMsg(wstring name, int rank, int score, int difficulty, int time) :name(name), rank(rank), score(score), difficulty(difficulty), time(time) {};
     PlayerMsg()
     {
         name = L"null";
         rank = 99;
         score = 5310;
         difficulty = 1;
         time = 0;
     }

     ~PlayerMsg() {}
 };

 class RankList
 {
 public:
     vector<PlayerMsg> m_msg;
     const int MAX_RANK = 8;//存储用户上限


     RankList();
     void ReadRank(PlayerMsg msg);//读取文件数据,存入到vector中
     void SaveMsg(PlayerMsg msg);//保存用户数据到vector中，如果其排名在10名之外，则不会保存成功
     void SaveToRank(PlayerMsg msg);//将vector中的数据写回文件
     vector<PlayerMsg> getRankList();
 };

struct SortPlayerMsg//自定义排序规则。如果得分一样，存活时间长的排名靠前
 {
     bool operator()(const PlayerMsg& msg1, const PlayerMsg& msg2)
     {
        return msg1.score > msg2.score;
     }
 };
// 子弹类
 class Bullet
{
public:
    Rect pos;
    int harm;	// 根据敌机等级判断对玩家造成的伤害或者根据玩家确定对敌机伤害
    float speed;
    int dir; // 子弹移动方向


    // 传入子弹伤害、速度以及敌机或者战机的中点坐标，根据图片宽度和高度确定pos
    Bullet(int x, int y, int harm, float speed);


};

// 道具:结构体包含具体位置坐标，以及对应效果。
 class Items {
public:
    int dir;//方向
    Rect pos;
    int num;	// 功能序号, 根据随机数确定
    float speed;



    // 传入功能序号，随机生成的中点坐标或者被击毁敌机的坐标，根据图片宽度和高度确定pos
    Items(int x, int y);


    /*
    功能：道具按照固定速度和方向前进
    参数：void
    返回值：void
    */
    void Move();

};

 class Boss : public Enemy {
 public:
     float boss_px;         //px用来接收图片的x坐标
     float boss_py;         //py用来接收图片的y坐标
     int wid;        //定义图片的宽
     int hig;        //定义图片的高
     IMAGE bossimage[2];//用来粗存boss图片的掩码图和背景图
     int dirctions[4][2] = { {1,1},{-1,1},{-1,-1},{1,-1} };

     Boss(float px, float py, int radis, int wid, int hig);
     void Move(int dirction);
     //Boss自动发射子弹
     void Fire(int,int,int);
     Rect& getpos() {//返回Boss的当前位置
         return pos;
     }
     int getwid() {//返回boss图片大小的宽
         return this->wid;
     }
     int gethig() {//返回boss图片大小的高
         return this->hig;
     }
 };
 //Boss的子弹类
 class BossBullet {
 public:
     float bossbullet_px;         //px用来接收图片的x坐标
     float bossbullet_py;         //py用来接收图片的y坐标
     float wid;        //定义图片的宽
     float hig;        //定义图片的高
     float rot;        //子弹的发射角度增量统一为15度111
     float bb_dir;     //第i颗子弹的发射角度111
     float dir_x;      //X方向上的增量111
     float dir_y;      //Y方向上的增量111
     int harm;
     bool ifshow;
     int frame;         //被创造出来时的帧数
     Rect bossbullet_pos;

     BossBullet(float index_x, float index_y, int radis, float wid, float hig,int frame);

     void Move1(int dir,int rot)
     {  
         this->rot = rot;//用rot来记录该子弹的每次旋转角度的增量
         this->bb_dir = dir * this->rot;//子弹旋转角度
         this->dir_x = -cos(this->bb_dir * PI / 180.0);
         this->dir_y = -(this->dir_x) * tan(this->bb_dir * PI / 180.0);
         //bossbullet图片移动
         bossbullet_pos.x += this->dir_x;//水平移动速度
         bossbullet_pos.y += this->dir_y;//垂直移动速度
         bossbullet_px = bossbullet_pos.x - (this->wid / 2);//将中心坐标还原到图片的位置
         bossbullet_py = bossbullet_pos.y - (this->hig / 2);//将中心位置还原到图片的位置  
     }
 };
 class Button
 {
 private:
     int x; // 按钮左上角x坐标
     int y; // 按钮左上角y坐标
     int width; // 按钮宽度
     int height; // 按钮高度
     float scale; // 缩放比例，用于实现鼠标悬停效果
     bool isMouseOver; // 表示鼠标是否在按钮上方
     wstring text; // 按钮文本
     function<void()> onClick; // 点击按钮触发的函数

 public:

     Button(int x, int y, int width, int height, const wstring& text, const function<void()>& onClick)
         : x(x), y(y), width(width), height(height), text(text), onClick(onClick), scale(1.0f), isMouseOver(false)
     {
     }

     // 检查鼠标是否在按钮上方
     void checkMouseOver(int mouseX, int mouseY);
     
     // 检查鼠标点击是否在按钮内，并执行函数
     bool checkClick(int mouseX, int mouseY);
     

     // 绘制按钮
     void draw();
     
 };

 // 定义Widget类，表示一个简单的图形用户界面
 class Widget
 {
 private:
     wstring text_name = L"飞机大战"; // 标题文本
     wstring set_name = L"游戏设置"; // 标题文本
     wstring difficulty_name = L"游戏难度"; // 难度文本
     wstring stop_name = L"游戏暂停"; // 暂停文本
     wstring fail_name = L"游戏失败"; // 失败文本
     wstring success_name = L"闯关成功"; // 失败文本
     wstring rank_name = L"排行榜"; // 排行榜文本
     wstring easy_name = L"简单模式排行榜"; // 简单排行榜文本
     wstring medium_name = L"中等模式排行榜"; // 中等排行榜文本
     wstring hard_name = L"地狱模式排行榜"; // 地狱排行榜文本
     wstring author_name = L"制作组信息"; // 制作组文本
     wstring rule_name = L"游戏玩法"; // 游戏玩法文本
     RECT rect_name = { 320, 50, 480, 150 };
     int text_x; // 文字的x坐标
     int text_y; // 文字的y坐标
     int width; // 宽度
     int height; // 高度
     int currentIndex; // 当前页面索引
     vector<IMAGE*> pages; // 存储所有页面的图片指针
     vector<vector<Button*>> buttons; // 存储每个页面上的按钮

     // 添加一个页面
     void addPage(IMAGE* page);


     // 在指定页面上添加一个按钮
     void addButton(int index, Button* button);


     // 设置当前显示的页面索引
     void setCurrentIndex(int index);


     // 处理鼠标点击事件
     void mouseClick(int mouseX, int mouseY);


     // 处理鼠标移动事件
     void mouseMove(int mouseX, int mouseY);


     // 绘制当前页面的内容
     void draw();
 

 public:
     Widget(int width, int height)
         :width(width), height(height), currentIndex(-1)
     {
     }
     ~Widget() {}

     void drawpage(wstring loc, wstring name);
     

     void drawpage(wstring loc, wstring name, int);
     

     // 初始化控件，创建图形环境，设置页面和按钮
     void init();
    

     // 运行，进入消息循环
     void run();
    

     // 关闭
     void close()
     {
         closegraph(); // 关闭图形环境
     }
 };
// 敌机管理数组
static vector<Enemy> AllEnemies;

// 敌机子弹管理数组
static vector<Bullet> EnemyBullets;

// 战机子弹管理数组
static vector<Bullet> MyBullets;

// 道具管理数组
static vector<Items> AllItems;

static vector<BossBullet> Bossbullets;

static vector<pair<string, int>> Rank[3];

static Player myPlayer(375, 500, 25, 100, 2, PLAYERHARM);//數據可改

static Boss *boss=NULL;

static time_t B_start;

static bool flag=false;

static IMAGE* bossbulletimage[2];//用来粗存bossbullet图片的掩码图和背景图





void allShow(Player& myPlayer);

void allMove(Player& myPlayer, int key);

void alltouch(Player& myPlayer);

void EnemyFire(int speedframe);

void allDestroy(Player& myPlayer);

bool ifendgame(Player& myPlayer);



void controlsound();

// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y); //x为载入图片的X坐标，y为Y坐标