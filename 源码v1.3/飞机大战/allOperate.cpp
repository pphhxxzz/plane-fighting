#pragma once
#include "allClass.h"
#include "tools.hpp"

PlayerMsg m_msg;
RankList* m_ranklist = new RankList();


RankList::RankList()
{
	if (!this->m_msg.empty()) this->m_msg.clear();
}

void RankList::ReadRank(PlayerMsg msg)
{
	wifstream infile;
	wstring m_rankfile;

	switch (msg.difficulty)
	{
	case 1:
		m_rankfile = L"LeaderBoard_1.txt";
		break;
	case 2:
		m_rankfile = L"LeaderBoard_2.txt";
		break;
	case 3:
		m_rankfile = L"LeaderBoard_3.txt";
		break;
	}

	infile.open(m_rankfile, ios::in);
	if (!infile)
	{
		//如果文件不存在，则创建
		ofstream os;
		os.open(m_rankfile);//默认会创建
		if (!os) exit(0);//如果创建失败，结束程序
		os.close();
	}
	else
	{
		wstring line;
		wstringstream stream;
		PlayerMsg msg;

		while (getline(infile, line))
		{
			stream.clear();
			stream.str(line); //将字符串流中的内容转换为字符串
			stream >> msg.rank >> msg.name >> msg.score >> msg.time >> msg.difficulty;
			m_msg.push_back(msg);
		}
		sort(m_msg.begin(), m_msg.end(), SortPlayerMsg());
	}
	infile.close();
}

void RankList::SaveMsg(PlayerMsg msg)
{
	m_msg.push_back(msg);
	sort(m_msg.begin(), m_msg.end(), SortPlayerMsg());
	if (m_msg.size() > this->MAX_RANK) m_msg.pop_back();

	vector<PlayerMsg>::iterator it = m_msg.begin();

	//修改rank
	for (int i = 0; i < m_msg.size(); i++, it++) it->rank = i + 1;
}

vector<PlayerMsg> RankList::getRankList()
{
	return this->m_msg;
}

void RankList::SaveToRank(PlayerMsg msg)
{
	wofstream outfile;
	wstring m_rankfile;

	switch (msg.difficulty)
	{
	case 1:
		m_rankfile = L"LeaderBoard_1.txt";
		break;
	case 2:
		m_rankfile = L"LeaderBoard_2.txt";
		break;
	case 3:
		m_rankfile = L"LeaderBoard_3.txt";
		break;
	}

	outfile.open(m_rankfile, ios::out); //每次写文件都重新写一遍
	if (!outfile) return; //如果失败，返回
	for (int i = 0; i < m_msg.size(); ++i)
	{
		outfile << m_msg[i].rank << " " << m_msg[i].name << ' ' << m_msg[i].score << " " << m_msg[i].time << " " << m_msg[i].difficulty << endl;
	}
	outfile.close();
}


//玩家--------------------------------------------------------------------------
//初始化 已測試 數據不建議改
Player::Player(int _posx, int _posy, int _r, int _hp, float _speed, int _harm)
{
	this->pos.x = _posx;
	this->pos.y = _posy;
	this->pos.r = _r;
	this->hp = _hp;
	this->speed = _speed;
	this->harm = _harm;
	this->time1 = time(nullptr);
	score = 0;
	firespeed = 300;
}

//移動 已測試 數據不建議改
void Player::Move(int dir)
{
	if (GetAsyncKeyState(VK_UP))
	{
		if (this->pos.y - 25 >= 0) this->pos.y -= this->speed;//不能出邊框
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		if (this->pos.y + 25 <= 640) this->pos.y += this->speed;

	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		if (this->pos.x - 25 >= 0) this->pos.x -= this->speed;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		if (this->pos.x + 25 <= 800) this->pos.x += this->speed;
	}
}

//發射子彈 已測試 y那裡有減40為了子彈剛好從戰機頭生成
void Player::Fire(int speedframe)
{
	if (!speedframe)
	{
		Bullet b(this->pos.x, this->pos.y - 40, this->harm, PLAYERBULLETSPEED);//減40是為了子彈在戰機前面生成
		MyBullets.push_back(b);

	}
}

//被打中 已測試 數據不建議改
void Player::BeHurt()
{
	auto it = EnemyBullets.begin();
	while (it < EnemyBullets.end())
	{
		//勾股定理 判斷距離
		if ((this->pos.x - it->pos.x) * (this->pos.x - it->pos.x) + (this->pos.y - it->pos.y) * (this->pos.y - it->pos.y) <= (this->pos.r + it->pos.r) * (this->pos.r + it->pos.r))
		{
			//扣血 加清除子彈
			this->hp -= it->harm;
			EnemyBullets.erase(it);
			break;
		}
		it++;
	}
}

//獲得道具 數據可改  不同的道具屬性
void Player::GetItem()
{
	auto it = AllItems.begin();
	while (it < AllItems.end())
	{
		//勾股定理 判斷距離
		if ((this->pos.x - it->pos.x) * (this->pos.x - it->pos.x) + (this->pos.y - it->pos.y) * (this->pos.y - it->pos.y) <= (this->pos.r + it->pos.r) * (this->pos.r + it->pos.r))
		{
			settextcolor(YELLOW);
			settextstyle(80, 0, _T("Consolas"));
			//獲得效果 然後消毀道具
			switch (it->num)
			{
			case 0://加血
				outtextxy(320, 250, L"hp++");
				Sleep(500);
				this->hp += 50;
				break;
			case 1:
				outtextxy(300, 250, L"harm++");
				Sleep(500);
				this->harm += 30;
				break;
			case 2:
				outtextxy(200, 250, L"firespeed++");
				Sleep(500);
				this->firespeed = this->firespeed * 0.85;
				break;
			}
			it = AllItems.erase(it);
		}
		else it++;

	}
}


void Player::crash()
{
	auto it = AllEnemies.begin();
	while (it < AllEnemies.end())
	{
		//勾股定理判斷距離
		if ((this->pos.x - it->pos.x) * (this->pos.x - it->pos.x) + (this->pos.y - it->pos.y) * (this->pos.y - it->pos.y) <= (this->pos.r + it->pos.r) * (this->pos.r + it->pos.r))
		{
			this->hp -= it->harm * 2;
			AllEnemies.erase(it);
			//cout << "玩家被敵機撞到\n";
			break;
		}
		it++;
	}
}

//玩家--------------------------------------------------------------------------

//敵機--------------------------------------------------------------------------
Enemy::Enemy(int x, int r, int _life)
{
	this->pos.x = x;
	this->pos.y = -r;
	this->pos.r = r;
	this->hp = 100+(difficulty -1)*50;//可改
	this->life = _life;//生成时刻
	this->speed = 0.5;
	this->harm = 15*difficulty;//寫創建敵機時要加在敵機的數組中
}


//敵機--------------------------------------------------------------------------

//道具--------------------------------------------------------------------------
Items::Items(int x = 0, int y = 0) {
	dir = 1;//默认向下
	if (x == 0 && y == 0) {
		pos.r = 20;
		pos.x = 80 + rand() % 640;
		pos.y = 160 + rand() % 320;
	}
	else {
		pos.r = 20;
		pos.x = x;
		pos.y = y;
	}
	num = rand() % 3;//0增加生命力 1增加攻击力 2增加攻击速度
	speed = 1;
}
void Items::Move() {

}
//道具--------------------------------------------------------------------------

//boss--------------------------------------------------------------------------
Boss::Boss(float px, float py, int radis, int wid, int hig) :Enemy(300, 100, 0) {
	//Boss出生坐标固定在y轴中点，以菱形轨迹(一个角立着的正方形)移动，使用loadiamge(pos.x + dir, pos.y + dir)和loadiamge(pos.x - dir, pos.y + dir)和loadiamge(pos.x - dir, pos.y - dir)和loadiamge(pos.x + dir, pos.y - dir)变换轨迹，每次变换轨迹都将dir置零
	boss_px = px;
	boss_py = py;//定义boss图片初始位置在窗口中心
	this->wid = wid;//初始化Boss图片的宽、高
	this->hig = hig;
	this->pos = { boss_px + this->wid / 2,boss_py + this->hig / 2,radis };//boss结构体记录图片的中心位置,半径为110
	loadimage(bossimage + 0, _T("./Boss/boss2掩码图.jpg"), wid, hig);//将初始化大小的boss掩码图片导入到内存中
	loadimage(bossimage + 1, _T("./Boss/boss2背景图.jpg"), wid, hig);//将初始化大小的boss背景图片导入到内存中
	hp = BOSSHP+1500 * (difficulty-1);

}
void Boss::Move(int dirction) {
	pos.x += 1.4 * speed * dirctions[dirction][0];//水平移动速度
	pos.y += 0.15 * speed * dirctions[dirction][1];//垂直移动速度
	boss_px = pos.x - (this->wid / 2);//将中心坐标还原到图片的位置
	boss_py = pos.y - (this->hig / 2);//将中心位置还原到图片的位置
}

void Boss::Fire(int speedframe, int num, int frame) {
	if (!speedframe)
	{
		for (int i = 0; i < num; ++i) Bossbullets.push_back(BossBullet(this->boss_px + 75, this->boss_py + 50, 12.5, 60, 60, frame));
	}
}
//boss子弹--------------------------------------------------------------------

BossBullet::BossBullet(float index_x, float index_y, int radis, float wid, float hig, int _frame) {

	ifshow = true;
	frame = _frame;
	harm = 10*difficulty;
	bossbullet_px = index_x;
	bossbullet_py = index_y;//定义bossbullet图片初始位置在窗口中心
	this->wid = wid;//初始化Bossbullet图片的宽、高
	this->hig = hig;
	this->bossbullet_pos = { bossbullet_px + this->wid / 2,bossbullet_py + this->hig / 2,radis };//bossbullet结构体记录图片的中心位置,半径为110
}

//子彈--------------------------------------------------------------------------
Bullet::Bullet(int x, int y, int harm, float speed)
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.r = 15;
	this->harm = harm;
	this->speed = speed;
}

//上面是成員函數實現


void Createenemy(int speedframe, int frame)
{
	if (!speedframe)
	{
		int x;
		x = rand() % 761 + 20;
		for (auto& enemy : AllEnemies) {
			if (enemy.pos.x - x < 30 && enemy.pos.x - x > -30) return;
		}
		for (auto& item : AllItems) {
			if (item.pos.x - x < 30 && item.pos.x - x > -30) return;
		}
		Enemy e(x, 20, frame);
		AllEnemies.push_back(e);
	}
}
void Createitem(int speedframe)
{

	if (!speedframe)
	{
		int x;
		x = rand() % 761 + 20;
		for (auto& item : AllItems) {
			if (item.pos.x - x < 30 && item.pos.x - x > -30) return;
		}
		for (auto& enemy : AllEnemies) {
			if (enemy.pos.x - x < 30 && enemy.pos.x - x > -30) return;
		}
		Items e(x, 20);
		AllItems.push_back(e);
	}
}

//包括戰機1 敵機1 boss 玩家子彈1 敵機子彈1 道具 (不斷運行)
void allShow(Player& myPlayer)
{
	//显示血量和得分
	settextstyle(25, 0, L"华文彩云");
	wstring str = to_wstring(myPlayer.hp);
	settextcolor(RED);
	outtextxy(60, 600, str.data());
	str = to_wstring(myPlayer.score);
	outtextxy(90, 35, str.data());

	setlinecolor(RED);
	//战机------------------------------
	IMAGE imgOfplayer;//图片对象
	loadimage(&imgOfplayer, _T("./resource/icon/plane.png"), 50, 50);
	drawImg(myPlayer.pos.x - 25, myPlayer.pos.y - 25, &imgOfplayer);
	circle(myPlayer.pos.x, myPlayer.pos.y, myPlayer.pos.r);//打印战机碰撞体积
	//敌机---------------------------------
	//创建图片对象
	IMAGE imgOfEnemy;
	loadimage(&imgOfEnemy, _T("./resource/icon/enemy.png"), 40, 40);
	//打印敌机
	auto it1 = AllEnemies.begin();
	settextstyle(15, 0, L"华文彩云");
	while (it1 < AllEnemies.end())
	{
		drawImg(it1->pos.x - it1->pos.r, it1->pos.y - it1->pos.r, &imgOfEnemy);
		str = to_wstring(it1->hp);
		outtextxy(it1->pos.x - 10, it1->pos.y - 35, str.data());
		circle(it1->pos.x, it1->pos.y, it1->pos.r);
		it1++;
	}
	//玩家子弹---------------------------------
	IMAGE imgOfPlayerBullet;
	loadimage(&imgOfPlayerBullet, _T("./resource/icon/plane_bullet.png"), 30, 30);
	//打印玩家子弹
	auto it2 = MyBullets.begin();
	while (it2 != MyBullets.end())
	{
		drawImg(it2->pos.x - it2->pos.r, it2->pos.y - it2->pos.r, &imgOfPlayerBullet);
		circle(it2->pos.x, it2->pos.y, it2->pos.r);
		it2++;
	}
	//敌机子弹---------------------------------
	IMAGE imgOfEnemyBullet;
	loadimage(&imgOfEnemyBullet, _T("./resource/icon/enemy_bullet.png"), 30, 30);
	//打印敌机子弹
	auto it3 = EnemyBullets.begin();
	//cout << EnemyBullets.size() << endl;
	while (it3 != EnemyBullets.end() && EnemyBullets.size())
	{
		drawImg(it3->pos.x - it3->pos.r, it3->pos.y - it3->pos.r, &imgOfEnemyBullet);
		circle(it3->pos.x, it3->pos.y, it3->pos.r);
		it3++;
	}
	//道具--------------------------------------
	IMAGE imgOfItemBullet;
	loadimage(&imgOfItemBullet, _T("./resource/icon/item.png"), 30, 30);
	//打印道具
	auto it4 = AllItems.begin();
	while (it4 < AllItems.end())
	{
		drawImg(it4->pos.x - it4->pos.r, it4->pos.y - it4->pos.r, &imgOfItemBullet);
		circle(it4->pos.x - 5, it4->pos.y - 5, it4->pos.r);
		it4++;
	}
	//Boss--------------------------------------
	if (boss)
	{
		settextstyle(40, 0, _T("Consolas"));
		if (boss)str = to_wstring(boss->hp), outtextxy(boss->boss_px + boss->wid / 2 - 35, boss->boss_py - 25, str.data());
		putimage(boss->boss_px, boss->boss_py, &(boss->bossimage[0]), SRCAND);
		putimage(boss->boss_px, boss->boss_py, &(boss->bossimage[1]), SRCPAINT);
		for (auto& bossbullet : Bossbullets) {
			if (bossbullet.ifshow)
			{
				putimage(bossbullet.bossbullet_px, bossbullet.bossbullet_py, bossbulletimage[0], SRCAND);
				putimage(bossbullet.bossbullet_px, bossbullet.bossbullet_py, bossbulletimage[1], SRCPAINT);
			}

		}
	}
}

//包括 玩家1 敵機1 boss 玩家子彈1 敵機子彈1 道具 的移動 (不斷運行)
void allMove(Player& myPlayer, int key)
{
	//玩家------------------------------------------------
	myPlayer.Move(key); //移动方向
	//敵機------------------------------------------------
	//遍歷敵機數組移動
	auto it1 = AllEnemies.begin();
	while (it1 < AllEnemies.end())
	{
		it1->pos.y += it1->speed;
		it1++;
	}
	//玩家子彈------------------------------------------------
	//遍歷玩家子彈數組移動
	auto it2 = MyBullets.begin();
	while (it2 < MyBullets.end())
	{
		it2->pos.y -= it2->speed;
		it2++;
	}
	//敵方子彈------------------------------------------------
	//遍歷敵方子彈數組移動
	auto it3 = EnemyBullets.begin();
	while (it3 < EnemyBullets.end())
	{
		it3->pos.y += it3->speed;
		it3++;
	}
	//道具====================================================
	for (auto& item : AllItems) {
		item.pos.y += item.speed;
	}
	//Boss===================================================
}

//包括 玩家(子彈1和道具1和敵機1) 敵機(子彈)1 boss(子彈)(不斷運行)
void alltouch(Player& myPlayer)
{
	//玩家(子彈和道具)------------------------------

	myPlayer.BeHurt();//判斷有沒有被打到
	myPlayer.GetItem();//判斷有沒有遇到道具
	myPlayer.crash();//判斷有沒有和敵機相撞
	//敵機(子彈)-------------------------------------
	//敵機不用它的成員函數的原因是因為簡化不了多了 最多也只能簡化一下勾股定理那裡
	//但那樣又要有返回值不然不知道要不要消毀子彈
	auto itOfEnemy = AllEnemies.begin();
	auto itOfMyBullet = MyBullets.begin();
	//遍歷整個敵機類
	while (itOfEnemy < AllEnemies.end())
	{
		//每一個敵機遍歷一次玩家子彈類看有沒有碰撞
		while (itOfMyBullet < MyBullets.end())
		{
			//勾股定理 判斷距離
			if ((itOfEnemy->pos.x - itOfMyBullet->pos.x) * (itOfEnemy->pos.x - itOfMyBullet->pos.x) + (itOfEnemy->pos.y - itOfMyBullet->pos.y) * (itOfEnemy->pos.y - itOfMyBullet->pos.y) <= (itOfEnemy->pos.r + itOfMyBullet->pos.r) * (itOfEnemy->pos.r + itOfMyBullet->pos.r))
			{

				itOfEnemy->hp -= itOfMyBullet->harm;
				//cout << itOfEnemy->hp<<' ';
				MyBullets.erase(itOfMyBullet);
				//cout << "敵機被子彈打中\n";
				break;
			}
			itOfMyBullet++;
		}
		itOfMyBullet = MyBullets.begin();
		itOfEnemy++;
	}
}

//包括 玩家1 敵機1 boss (不斷運行)
void EnemyFire(int frame)
{
	auto it = AllEnemies.begin();
	//遍歷整個敵機數組
	while (it != AllEnemies.end())
	{
		if (!((frame - it->life) % 550))
		{
			Bullet b(it->pos.x, it->pos.y + 35, it->harm, it->speed + 0.5);
			EnemyBullets.push_back(b);
		}
		it++;
	}
}

//包括 玩家(好像在另一個函數有實現)0 敵機1 boss 玩家子彈 敵方子彈(不斷運行)
void allDestroy(Player& myPlayer)
{
	//敵機----------------------------------------

	for (auto it = AllEnemies.begin(); it != AllEnemies.end() && AllEnemies.size();)
	{
		if (it->hp <= 0 || it->pos.y - 20 >= 640)
		{
			if (it->hp <= 0)myPlayer.score += 20;
			it = AllEnemies.erase(it);
		}
		else it++;
	}

	//玩家子彈----------------------------------------

	for (auto it = MyBullets.begin(); it != MyBullets.end() && MyBullets.size();)
	{
		if (it->pos.y + 15 <= 0)
		{

			it = MyBullets.erase(it);
		}
		else it++;
	}


	////敵機子彈----------------------------------------

	for (auto it = EnemyBullets.begin(); it != EnemyBullets.end() && EnemyBullets.size();)
	{
		if (it->pos.y + 15 >= 640)
		{
			it = EnemyBullets.erase(it);
		}
		else it++;
	}

	////道具--------------------------------------------


	for (auto it = AllItems.begin(); it != AllItems.end() && AllItems.size();)
	{
		if (it->pos.y + 15 >= 640)
		{
			it = AllItems.erase(it);
		}
		else it++;
	}

}
//判断游戏是否结束------------------------------------
bool ifendgame(Player& player)
{
	if (player.hp <= 0)
	{
		return true;
	}

	return false;
}

int lasthurtframe=0;

void Bossbullettouch(int frame) {

	for (auto it = Bossbullets.begin(); it != Bossbullets.end() && Bossbullets.size();)
	{
		if (sqrt(((it->bossbullet_px + it->wid/2) - myPlayer.pos.x) * ((it->bossbullet_px + it->wid/2) - myPlayer.pos.x) + ((it->bossbullet_py + it->wid/2) - myPlayer.pos.y) * ((it->bossbullet_py + it->wid/2) - myPlayer.pos.y)) <= abs((it->wid/2-5 + myPlayer.pos.r)))
		{
			myPlayer.hp -= it->harm;
			it = Bossbullets.erase(it);
		}
		else it++;
	}
	for (auto it = MyBullets.begin(); it != MyBullets.end() && MyBullets.size();) {
		if (sqrt((it->pos.x - boss->pos.x) * (it->pos.x - boss->pos.x) + (it->pos.y - boss->pos.y) * (it->pos.y - boss->pos.y)) <= abs((65 + boss->pos.r))) {
			boss->hp -= myPlayer.harm;
			if (boss->hp <= 0)myPlayer.score += 400;
			it = MyBullets.erase(it);
		}
		else it++;
	}
	if (frame - lasthurtframe>600 && sqrt((myPlayer.pos.x - boss->pos.x) * (myPlayer.pos.x - boss->pos.x) + (myPlayer.pos.y - boss->pos.y) * (myPlayer.pos.y - boss->pos.y)) <= abs(myPlayer.pos.r + 71)) {
		myPlayer.hp -= 25;
		lasthurtframe = frame;
	}
}
void Bossbulletdestory(int frame) {
	for (auto it = Bossbullets.begin(); it != Bossbullets.end() && Bossbullets.size();)
	{
		if (frame - it->frame > 2000)
			it = Bossbullets.erase(it);
		else it++;
	}
}


// 控制音效
void controlsound()
{
	++music_num;
	if (music_num % 2 == 1)
	{
		mciSendString(L"play ./music/game_music.mp3 repeat", NULL, 0, NULL);
	}
	else
	{
		mciSendString(L"pause ./music/game_music.mp3", NULL, 0, NULL);
	}
}

// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

void Button::checkMouseOver(int mouseX, int mouseY)
{
	isMouseOver = (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);

	if (isMouseOver) {
		scale = 0.9f; // 鼠标悬停时缩放按钮
	}
	else {
		scale = 1.0f; // 恢复按钮原始大小
	}
}

bool Button::checkClick(int mouseX, int mouseY)
{
	if (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height)
	{
		onClick(); // 执行按钮点击时的函数
		isMouseOver = false;
		scale = 1.0f;
		return true;
	}
	return false;
}

void Button::draw()
{
	int scaledWidth = width * scale; // 缩放后的按钮宽度
	int scaledHeight = height * scale; // 缩放后的按钮高度
	int scaledX = x + (width - scaledWidth) / 2; // 缩放后的按钮x坐标
	int scaledY = y + (height - scaledHeight) / 2; // 缩放后的按钮y坐标

	if (isMouseOver)
	{
		setlinecolor(RGB(0, 120, 215)); // 鼠标悬停时按钮边框颜色
		setfillcolor(RGB(229, 241, 251)); // 鼠标悬停时按钮填充颜色

	}
	else
	{
		setlinecolor(RGB(255, 144, 126)); // 按钮边框颜色
		setfillcolor(RGB(225, 225, 225)); // 按钮填充颜色
	}

	setlinestyle(PS_DASH | PS_ENDCAP_FLAT, 3);  // 设置线条宽度和样式
	fillroundrect(scaledX, scaledY, scaledX + scaledWidth, scaledY + scaledHeight, 50, 50); // 绘制按钮
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(25 * scale, 0, _T("华文彩云")); // 设置文本大小和字体
	//居中显示按钮文本
	int textX = scaledX + (scaledWidth - textwidth(text.c_str())) / 2; // 计算文本在按钮中央的x坐标
	int textY = scaledY + (scaledHeight - textheight(L"幼圆")) / 2; // 计算文本在按钮中央的y坐标
	outtextxy(textX, textY, text.c_str()); // 在按钮上绘制文本
}

void Widget::addPage(IMAGE* page)
{
	pages.push_back(page);
	buttons.push_back({});
}

void Widget::addButton(int index, Button* button)
{
	if (index >= 0 && index < buttons.size())
	{
		buttons[index].push_back(button);
	}
}

void Widget::setCurrentIndex(int index)
{
	if (index >= 0 && index < pages.size())
	{
		currentIndex = index;
	}
}

void Widget::mouseClick(int mouseX, int mouseY)
{
	if (currentIndex >= 0 && currentIndex < buttons.size())
	{
		for (Button* button : buttons[currentIndex])
		{
			if (button->checkClick(mouseX, mouseY))
			{
				break;
			}
		}
	}
}

void Widget::mouseMove(int mouseX, int mouseY)
{
	if (currentIndex >= 0 && currentIndex < buttons.size())
	{
		for (Button* button : buttons[currentIndex])
		{
			button->checkMouseOver(mouseX, mouseY);
		}
	}
}

void Widget::draw()
{
	BeginBatchDraw();

	putimage(0, 0, pages[currentIndex]); // 在窗口中绘制当前页面的图片
	if (currentIndex >= 0 && currentIndex < pages.size())
	{
		putimage(0, 0, pages[currentIndex]); // 在窗口中绘制当前页面的图片
		if (currentIndex >= 0 && currentIndex < buttons.size())
		{
			for (Button* button : buttons[currentIndex])
			{
				button->draw(); // 绘制当前页面上的所有按钮
			}
		}
	}

	if (currentIndex == 2) {

		allShow(myPlayer);
	}
	EndBatchDraw();
}

void Widget::drawpage(wstring loc, wstring name)
{
	IMAGE* page = new IMAGE(width, height);// 昵称界面的图片指针
	loadimage(page, loc.c_str(), width, height, true);
	putimage(0, 0, page);
	setlinestyle(PS_DASH | PS_ENDCAP_FLAT, 3);  // 设置线条宽度和样式
	// 绘制标题
	fillroundrect(rect_name.left, rect_name.top, rect_name.right, rect_name.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect_name.left + (rect_name.right - rect_name.left - textwidth(name.c_str())) / 2;
	text_y = rect_name.top + (rect_name.bottom - rect_name.top - textheight(name.c_str())) / 2;
	outtextxy(text_x, text_y, name.c_str()); // 在按钮上绘制文本
	getimage(page, 0, 0, width, height); // 将昵称页面的内容保存到图片中
	addPage(page); // 添加昵称页面
}

void Widget::drawpage(wstring loc, wstring name, int)
{
	IMAGE* page = new IMAGE(width, height);
	loadimage(page, loc.c_str(), width, height, true);
	//putimage(0, 0, page);
	// 绘制简单榜标题
	RECT rect = { 220, 50, 580, 150 };
	fillroundrect(rect.left, rect.top, rect.right, rect.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect.left + (rect.right - rect.left - textwidth(name.c_str())) / 2;
	text_y = rect.top + (rect.bottom - rect.top - textheight(name.c_str())) / 2;
	outtextxy(text_x, text_y, name.c_str()); // 在按钮上绘制文本
	fillroundrect(rect.left - 100, rect.top + 120, rect.right + 100, rect.bottom + 370, 50, 50); // 绘制标题
	settextstyle(25, 0, _T("华文琥珀")); // 设置文本大小和字体
	outtextxy(rect.left - 60, rect.top + 130, L"排名"); // 绘制排名
	outtextxy(rect.left + 140, rect.top + 130, L"昵称"); // 绘制昵称
	outtextxy(rect.left + 330, rect.top + 130, L"积分"); // 绘制积分

	int tmp_difficulty = m_msg.difficulty;
	m_msg.difficulty = (name == L"地狱模式排行榜") ? 3 : (name == L"简单模式排行榜" ? 1 : 2);
	m_ranklist->m_msg.clear();
	m_ranklist->ReadRank(m_msg);
	int itl = 200;
	for (auto& msg : m_ranklist->m_msg) {
		wstring text = to_wstring(msg.rank) + L"               " + msg.name;
		outtextxy(rect.left - 60, rect.top + itl, text.data());
		text = to_wstring(msg.score);
		outtextxy(rect.left + 330, rect.top + itl, text.data());
		itl += 30;
	}
	m_msg.difficulty = tmp_difficulty;
	getimage(page, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page); // 添加暂停页面
}

void Widget::init()
{
	initgraph(width, height);
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, NULL, GetSystemMetrics(SM_CXSCREEN) / 3, GetSystemMetrics(SM_CYSCREEN) / 4, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	bossbulletimage[0] = new IMAGE;
	bossbulletimage[1] = new IMAGE;
	loadimage(bossbulletimage[0], _T("./BossBullet/子弹掩码图1.jpg"));//将初始化大小的bossbullet掩码图片导入到内存中
	loadimage(bossbulletimage[1], _T("./BossBullet/子弹背景图1.jpg"));//将初始化大小的bossbullet掩码图片导入到内存中
	BeginBatchDraw(); // 开始批量绘制, 批量绘制应该放在initgraph之后

	// 0. 创建昵称界面
	drawpage(L"./resource/page/name.jpg", text_name);


	// 在昵称页面创建按钮input
	Button* name_input = new Button(320, 200, 160, 60, L"输入游戏昵称", [&]() {
		InputBox(name, 20, L"请输入游戏昵称"); // 点击按钮输入游戏昵称
		});
	addButton(0, name_input); // 将按钮input添加到昵称页面

	// 在昵称页面创建按钮进入游戏
	Button* name_enter = new Button(320, 310, 160, 60, L"进入游戏", [&]() {
		setCurrentIndex(1);
		});
	addButton(0, name_enter); // 将按钮添加到昵称页面

	// 在昵称页面创建按钮退出游戏
	Button* name_exit = new Button(320, 420, 160, 60, L"退出游戏", [&]() {
		closegraph();
	exit(0);
		});
	addButton(0, name_exit); // 将按钮添加到昵称页面

	// 1. 创建开始页面
	drawpage(L"./resource/page/start.jpg", text_name);

	// 在开始页面创建开始游戏按钮
	Button* start_begin = new Button(320, 180, 150, 60, L"开始游戏", [&]() {
		setCurrentIndex(2); // 进入游戏界面
		});
	addButton(1, start_begin); // 将按钮添加到页面

	// 在开始页面创建游戏难度按钮
	Button* start_difficulty = new Button(320, 260, 150, 60, L"游戏难度", [&]() {
		setCurrentIndex(3); // 进入游戏难度界面
		});
	addButton(1, start_difficulty); // 将按钮添加到页面

	// 在开始页面创建排行榜按钮
	Button* start_rank = new Button(320, 340, 150, 60, L"排行榜", [&]() {
		setCurrentIndex(4); // 进入排行榜界面
		});
	addButton(1, start_rank); // 将按钮添加到页面

	// 在开始页面创建游戏设置按钮
	Button* start_set = new Button(320, 420, 150, 60, L"游戏设置", [&]() {
		setCurrentIndex(5); // 进入开始界面的游戏设置界面
		});
	addButton(1, start_set); // 将按钮添加到页面

	// 在开始页面创建退出账号按钮
	Button* start_exit = new Button(320, 500, 150, 60, L"退出账号", [&]() {
		setCurrentIndex(0); // 返回昵称界面
		});
	addButton(1, start_exit); // 将按钮添加到页面

	// 2. 创建游戏页面
	IMAGE* page_game = new IMAGE(width, height);
	loadimage(page_game, L"./resource/page/game.jpg", width + 400, height, true);
	putimage(0, 0, page_game);
	loadimage(page_game, L"./resource/icon/score.png", 40, 60, true);	// 计分板
	drawAlpha(page_game, 15, 15);     // 载入PNG图并去透明部分
	loadimage(page_game, L"./resource/icon/life.png", 40, 40, true);	// 血量
	drawAlpha(page_game, 10, 590);     // 载入PNG图并去透明部分
	fillroundrect(60, 25, 130, 65, 20, 50); // 分数显示
	fillroundrect(50, 590, 120, 630, 20, 50); // 血量显示
	getimage(page_game, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page_game); // 添加游戏页面

	// 在游戏页面创建暂停按钮
	Button* game_stop = new Button(730, 25, 60, 50, L"暂停", [&]() {
		setCurrentIndex(6); // 进入暂停界面
		});
	addButton(2, game_stop); // 将按钮添加到页面

	// 3. 创建游戏难度页面
	drawpage(L"./resource/page/difficulty.jpg", difficulty_name);

	// 在游戏难度页面创建简单按钮
	Button* difficulty_easy = new Button(320, 200, 160, 60, L"简单", [&]() {
		difficulty = 1; // 难度设置为简单
		});
	addButton(3, difficulty_easy); // 将按钮添加到页面
	// 在游戏难度页面创建中等按钮
	Button* difficulty_medium = new Button(320, 310, 160, 60, L"中等", [&]() {
		difficulty = 2; // 难度设置为中等
		});
	addButton(3, difficulty_medium); // 将按钮添加到页面
	// 在游戏难度页面创建地狱按钮
	Button* difficulty_hard = new Button(320, 420, 160, 60, L"地狱", [&]() {
		difficulty = 3; // 难度设置为地狱
		});
	addButton(3, difficulty_hard); // 将按钮添加到页面
	// 在游戏难度页面创建返回按钮
	Button* difficulty_exit = new Button(320, 530, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 返回开始界面
		});
	addButton(3, difficulty_exit); // 将按钮添加到页面

	// 4. 创建排行榜页面
	drawpage(L"./resource/page/rank.jpg", rank_name);

	// 在排行榜页面创建简单按钮
	Button* rank_easy = new Button(320, 200, 160, 60, L"简单榜", [&]() {
		setCurrentIndex(7); // 进入简单排行榜
		});
	addButton(4, rank_easy); // 将按钮添加到页面
	// 在排行榜页面创建中等按钮
	Button* rank_medium = new Button(320, 310, 160, 60, L"中等榜", [&]() {
		setCurrentIndex(8); // 进入中等排行榜
		});
	addButton(4, rank_medium); // 将按钮添加到页面
	// 在排行榜页面创建地狱按钮
	Button* rank_hard = new Button(320, 420, 160, 60, L"地狱榜", [&]() {
		setCurrentIndex(9); // 进入地狱排行榜
		});
	addButton(4, rank_hard); // 将按钮添加到页面
	// 在排行榜页面创建返回按钮
	Button* rank_exit = new Button(320, 530, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 返回开始界面
		});
	addButton(4, rank_exit); // 将按钮添加到页面

	// 5. 游戏设置界面
	drawpage(L"./resource/page/set.jpg", set_name);

	// 在开始界面的设置界面页面创建音效按钮
	Button* setinstart_music = new Button(320, 200, 160, 60, L"打开/关闭音效", [&]() {
		controlsound(); // 控制音效
		});
	addButton(5, setinstart_music); // 将按钮添加到页面
	// 在开始界面的设置界面页面创建游戏玩法按钮
	Button* setinstart_rule = new Button(320, 310, 160, 60, L"游戏玩法", [&]() {
		setCurrentIndex(11); // 进入游戏玩法
		});
	addButton(5, setinstart_rule); // 将按钮添加到页面
	// 在开始界面的设置界面页面创建制作组信息按钮
	Button* setinstart_author = new Button(320, 420, 160, 60, L"制作组信息", [&]() {
		setCurrentIndex(10); // 进入制作组信息界面
		});
	addButton(5, setinstart_author); // 将按钮添加到页面
	// 在开始界面的设置界面页面创建返回按钮
	Button* setinstart_exit = new Button(320, 530, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 返回开始界面
		});
	addButton(5, setinstart_exit); // 将按钮添加到页面

	// 6. 创建暂停界面
	drawpage(L"./resource/page/stop.jpg", stop_name);

	// 在暂停页面创建继续游戏按钮
	Button* stop_continue = new Button(320, 200, 160, 60, L"继续游戏", [&]() {
		setCurrentIndex(2); // 返回游戏界面
		});
	addButton(6, stop_continue); // 将按钮添加到页面
	Button* stop_music = new Button(320, 310, 160, 60, L"打开/关闭音效", [&]() {
		controlsound(); // 设置音效
		});
	addButton(6, stop_music); // 将按钮添加到页面
	// 在暂停界面创建返回开始界面按钮
	Button* stop_exit = new Button(320, 420, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 返回开始界面
		});
	addButton(6, stop_exit); // 将按钮添加到页面

	// 7. 简单榜
	drawpage(L"./resource/page/easy.jpg", easy_name, 0);

	// 在简单榜界面创建返回排行榜界面按钮
	Button* easy_exit = new Button(320, 530, 160, 60, L"返回排行榜", [&]() {
		setCurrentIndex(4); // 返回排行榜界面
		});
	addButton(7, easy_exit); // 将按钮添加到页面

	// 8. 中等榜
	drawpage(L"./resource/page/medium.jpg", medium_name, 0);

	// 在中等榜界面创建返回排行榜界面按钮
	Button* medium_exit = new Button(320, 530, 160, 60, L"返回排行榜", [&]() {
		setCurrentIndex(4); // 返回排行榜界面
		});
	addButton(8, medium_exit); // 将按钮添加到页面

	// 9. 地狱榜
	drawpage(L"./resource/page/hard.jpg", hard_name, 0);

	// 在地狱榜界面创建返回排行榜界面按钮
	Button* hard_exit = new Button(320, 530, 160, 60, L"返回排行榜", [&]() {
		setCurrentIndex(4); // 返回排行榜界面
		});
	addButton(9, medium_exit); // 将按钮添加到页面

	// 10. 制作组
	IMAGE* page_author = new IMAGE(width, height);
	loadimage(page_author, L"./resource/page/author.jpg", width, height, true);
	putimage(0, 0, page_author);
	// 绘制制作组标题
	RECT rect_author = { 220, 50, 580, 150 };
	fillroundrect(rect_author.left, rect_author.top, rect_author.right, rect_author.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect_author.left + (rect_author.right - rect_author.left - textwidth(author_name.c_str())) / 2;
	text_y = rect_author.top + (rect_author.bottom - rect_author.top - textheight(author_name.c_str())) / 2;
	outtextxy(text_x, text_y, author_name.c_str()); // 在按钮上绘制文本
	fillroundrect(rect_author.left - 100, rect_author.top + 120, rect_author.right + 100, rect_author.bottom + 370, 50, 50); // 绘制标题
	settextstyle(28, 0, _T("华文彩云")); // 设置文本大小和字体
	outtextxy(rect_author.left - 40, rect_author.top + 130, L"成员"); // 绘制昵称
	outtextxy(rect_author.left + 220, rect_author.top + 130, L"负责部分"); // 绘制简介
	settextstyle(25, 0, _T("华光行楷_CNKI")); // 设置文本大小和字体
	settextcolor(RGB(49, 128, 87)); // 设置文本颜色
	outtextxy(rect_author.left - 40, rect_author.top + 170, L"邓钦予"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 200, L"何宁"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 230, L"祖培宏"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 260, L"李丰任"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 290, L"杨永山泰"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 320, L"千反田"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 350, L"苏子乔"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 380, L"沈兴满"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 410, L"阿聪"); // 绘制昵称
	outtextxy(rect_author.left - 40, rect_author.top + 440, L"岳永"); // 绘制昵称
	outtextxy(rect_author.left + 160, rect_author.top + 170, L"游戏界面设计"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 200, L"游戏BOSS生成和运行逻辑"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 230, L"游戏功能流程图整理"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 260, L"子弹设计和代码整合"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 290, L"子弹设计和代码整合"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 320, L"敌机生成和运行逻辑"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 350, L"数据保存加载和排行榜制作"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 380, L"游戏音效设置"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 410, L"玩家战机生成和运行逻辑"); // 绘制简介
	outtextxy(rect_author.left + 160, rect_author.top + 440, L"道具生成和运行逻辑"); // 绘制简介
	getimage(page_author, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page_author); // 添加制作组信息页面
	// 在制作组界面创建返回按钮
	Button* author_exit = new Button(320, 540, 160, 60, L"返回设置", [&]() {
		setCurrentIndex(5); // 返回设置界面
		});
	addButton(10, author_exit); // 将按钮添加到页面

	// 11. 游戏玩法说明
	IMAGE* page_rule = new IMAGE(width, height);
	loadimage(page_rule, L"./resource/page/rule.jpg", width, height, true);
	putimage(0, 0, page_rule);
	// 绘制玩法标题
	RECT rect_rule = { 220, 50, 580, 150 };
	fillroundrect(rect_rule.left, rect_rule.top, rect_rule.right, rect_rule.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect_rule.left + (rect_rule.right - rect_rule.left - textwidth(rule_name.c_str())) / 2;
	text_y = rect_rule.top + (rect_rule.bottom - rect_rule.top - textheight(rule_name.c_str())) / 2;
	outtextxy(text_x, text_y, rule_name.c_str()); // 在按钮上绘制文本
	fillroundrect(rect_rule.left - 80, rect_rule.top + 120, rect_rule.right + 80, rect_rule.bottom + 370, 50, 50); // 绘制标题
	settextstyle(25, 0, _T("华文琥珀")); // 设置文本大小和字体
	settextcolor(RGB(49, 128, 87)); // 设置文本颜色为黑色
	outtextxy(rect_rule.left - 40, rect_rule.top + 140, L"↑↓←→控制战机移动");
	outtextxy(rect_rule.left - 40, rect_rule.top + 180, L"首页可以设置游戏难度");
	outtextxy(rect_rule.left - 40, rect_rule.top + 220, L"昵称页面可以设置玩家昵称");
	outtextxy(rect_rule.left - 40, rect_rule.top + 260, L"战机自动发射子弹");
	outtextxy(rect_rule.left - 40, rect_rule.top + 300, L"击毁敌机分数增加, 被敌机击中生命值减少");
	outtextxy(rect_rule.left - 40, rect_rule.top + 340, L"击毁BOSS游戏胜利, 生命值小于0游戏结束");
	outtextxy(rect_rule.left - 40, rect_rule.top + 380, L"获得道具可以随机增加生命或子弹伤害");
	outtextxy(rect_rule.left - 40, rect_rule.top + 420, L"游戏界面右上角可以暂停游戏");
	getimage(page_rule, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page_rule); // 添加玩法信息页面
	// 在玩法界面创建返回按钮
	Button* rule_exit = new Button(320, 540, 160, 60, L"返回设置", [&]() {
		setCurrentIndex(5); // 返回设置界面
		});
	addButton(11, rule_exit); // 将按钮添加到页面

	// 12. 失败界面
	IMAGE* page_fail = new IMAGE(width, height);
	loadimage(page_fail, L"./resource/page/fail.jpg", width, height, true);
	putimage(0, 0, page_fail);
	// 绘制玩法标题
	RECT rect_fail = { 220, 50, 580, 150 };
	fillroundrect(rect_fail.left, rect_fail.top, rect_fail.right, rect_fail.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect_fail.left + (rect_fail.right - rect_fail.left - textwidth(fail_name.c_str())) / 2;
	text_y = rect_fail.top + (rect_fail.bottom - rect_fail.top - textheight(fail_name.c_str())) / 2;
	outtextxy(text_x, text_y, fail_name.c_str()); // 在按钮上绘制文本
	fillroundrect(rect_fail.left, rect_fail.top + 120, rect_fail.right, rect_fail.bottom + 60, 50, 50); // 绘制安慰
	settextstyle(25, 0, _T("华文琥珀")); // 设置文本大小和字体
	outtextxy(rect_fail.left + 90, rect_fail.top + 130, L"别走，决战到天亮！"); // 绘制失败安慰
	getimage(page_fail, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page_fail); // 添加失败页面
	// 在失败页面创建音效设置按钮
	Button* fail_music = new Button(320, 250, 160, 60, L"打开/关闭音效", [&]() {
		controlsound(); // 音效设置
		});
	addButton(12, fail_music); // 将按钮添加到页面
	// 在失败页面创建再次挑战按钮
	Button* fail_restart = new Button(320, 360, 160, 60, L"再次挑战", [&]() {
		setCurrentIndex(2); // 初始化数据并返回游戏界面
		});
	addButton(12, fail_restart); // 将按钮添加到页面
	// 在失败页面创建返回按钮
	Button* fail_exit = new Button(320, 470, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 初始化数据并返回游戏界面
		});
	addButton(12, fail_exit); // 将按钮添加到页面

	// 13. 成功界面
	IMAGE* page_success = new IMAGE(width, height);
	loadimage(page_success, L"./resource/page/success.jpg", width, height, true);
	putimage(0, 0, page_success);

	//绘制成功标题
	RECT rect_success = { 220, 50, 580, 150 };
	fillroundrect(rect_success.left, rect_success.top, rect_success.right, rect_success.bottom, 50, 50); // 绘制标题
	settextcolor(GREEN); // 设置文本颜色为绿色
	setbkmode(TRANSPARENT); // 设置文本背景透明
	settextstyle(35, 0, _T("华文琥珀")); // 设置文本大小和字体
	text_x = rect_success.left + (rect_success.right - rect_success.left - textwidth(success_name.c_str())) / 2;
	text_y = rect_success.top + (rect_success.bottom - rect_success.top - textheight(success_name.c_str())) / 2;
	outtextxy(text_x, text_y, success_name.c_str()); // 在按钮上绘制文本
	fillroundrect(rect_success.left, rect_success.top + 120, rect_success.right, rect_success.bottom + 60, 50, 50); // 绘制安慰
	settextstyle(25, 0, _T("华文琥珀")); // 设置文本大小和字体
	outtextxy(rect_success.left + 120, rect_success.top + 130, L"牛哇牛哇！"); // 绘制通关提示
	getimage(page_success, 0, 0, width, height); // 将页面的内容保存到图片中
	addPage(page_success); // 添加成功页面
	// 在成功页面创建重新挑战按钮
	Button* success_restart = new Button(320, 400, 160, 60, L"重新挑战", [&]() {
		setCurrentIndex(2);
		});
	addButton(13, success_restart); // 将按钮添加到页面
	// 在成功页面创建返回按钮
	Button* success_exit = new Button(320, 470, 160, 60, L"返回开始界面", [&]() {
		setCurrentIndex(1); // 初始化数据并返回游戏界面
		});
	addButton(13, success_exit); // 将按钮添加到页面

	setCurrentIndex(0); // 设置初始显示页面
	FlushBatchDraw(); // 将缓冲区内容显示在屏幕上
}

void Widget::run()
{
	//鼠标消息
	ExMessage msg;
	//游戏变量
	time_t start = time(NULL);//计时
	int num = 4+6*difficulty;//Boss子弹数量
	int speed = 500 - 100 * difficulty;//敌机生成速度
	int frame = 0;//帧率
	int bossframe = 0;//Boss出现时的帧率
	while (true)
	{
		//变量更新
		int lastIndex = currentIndex;

		//input
		if (peekmessage(&msg)) // 检查是否有消息
		{
			int mouseX = msg.x; // 获取鼠标x坐标
			int mouseY = msg.y; // 获取鼠标y坐标

			switch (msg.message)
			{
			case WM_LBUTTONDOWN: // 鼠标左键按下事件
				mouseClick(mouseX, mouseY); // 处理鼠标点击事件
				break;
			case WM_MOUSEMOVE: // 鼠标移动事件
				mouseMove(mouseX, mouseY); // 处理鼠标移动事件
				break;
			}
		}
		//reset 退出游戏界面的重置操作
		if ((lastIndex != 2 && currentIndex == 2 && lastIndex != 6) || ((lastIndex == 12 || lastIndex == 13) && currentIndex == 1)) {
			num = 4 + 6 * difficulty;
			start = time(NULL);
			B_start = time(nullptr);
			if (boss) {
				delete boss;
				boss = NULL;
			}
			frame = 0;
			AllEnemies.clear();
			EnemyBullets.clear();
			Bossbullets.clear();
			AllItems.clear();
			myPlayer.pos.x = 375, myPlayer.pos.y = 500, myPlayer.pos.r = 25, myPlayer.hp = 100, myPlayer.speed = PLAYERSPEED, myPlayer.harm = PLAYERHARM;
			myPlayer.score = 0;
			flag = false;
			speed = 500 - 100 * difficulty;//重置敌机生成速度
		}

		//游戏界面逻辑操作
		if (currentIndex == 2) {
			if (!(frame % 400))myPlayer.score += 1;
			if (flag)//boss战
			{
				//玩家开火
				myPlayer.Fire(frame % myPlayer.firespeed);
				//boss开火
				boss->Fire(frame % (500-50*difficulty), num, frame);
				//道具生成
				Createitem(frame % 800);
				//处理移动
				int key = _kbhit();
				allMove(myPlayer, key);
				//处理boss移动
				boss->Move(((frame - bossframe) / 400) % 4);
				//处理boss子弹移动
				for (int i = 0; i < Bossbullets.size(); ++i)
				{
					Bossbullets[i].Move1(i, 360 / num);
				}
				//处理碰撞
				Bossbullettouch(frame);
				myPlayer.GetItem();
				//销毁
				Bossbulletdestory(frame);
				allDestroy(myPlayer);

			}
			else {
				//敌机与道具生成
				Createenemy(frame % speed, frame);
				Createitem(frame % 700);
				//玩家与敌机开火
				myPlayer.Fire(frame % myPlayer.firespeed);
				EnemyFire(frame % 500);
				//处理所有移动
				int key = _kbhit();
				allMove(myPlayer, key); 
				//处理碰撞
				alltouch(myPlayer);
				//处理销毁
				allDestroy(myPlayer);

				//标记指定帧后boss生成-----------

				if (frame >= 20000 && !boss) {
					flag = true;
					boss = new Boss(280, 85, 60, 260, 143);
					bossframe = frame;
					//清除界面
					AllEnemies.clear();
					EnemyBullets.clear();
					settextstyle(120, 0, _T("Consolas"));
					outtextxy(260, 250, L"BOSS!!");
					Sleep(1500);
				}
			}
			//更新帧数
			frame++;
		}
		//处理判断
		if (boss && boss->hp <= 0) {//胜利
			setCurrentIndex(13);
		}
		if (ifendgame(myPlayer) && lastIndex != 12)//失败
		{
			setCurrentIndex(12);
		}
		//保存记录
		if (lastIndex == 2 && (currentIndex == 13 || currentIndex == 12)) {
			m_msg.name = name, m_msg.score = myPlayer.score, m_msg.time = time(nullptr) - start, m_msg.difficulty = difficulty;
			m_ranklist->SaveMsg(m_msg);
			m_ranklist->SaveToRank(m_msg);
		}
		//output
		draw(); // 绘制当前页面内容
	}
}