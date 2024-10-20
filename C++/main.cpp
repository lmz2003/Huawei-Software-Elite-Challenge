#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

struct Robot
{
    int x, y, goods;   //机器人坐标和是否携带货物
    int status;		//机器人状态  恢复还是正常运行
    int mbx, mby;  
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];

struct Berth
{
    int x;
    int y;
    int transport_time;
    int loading_speed;
    Berth(){}
    
    Berth(int x, int y, int transport_time, int loading_speed) {
        this -> x = x;
        this -> y = y;
        this -> transport_time = transport_time;
        this -> loading_speed = loading_speed;
    }
}berth[berth_num + 10];

struct Boat
{
    int num, pos, status;
}boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
void Init()  //判题器初始化
{
    for(int i = 1; i <= n; i ++)   //读入地图信息
        scanf("%s", ch[i] + 1);
    for(int i = 0; i < berth_num; i ++)  //读入泊位信息
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d", &berth[id].x, &berth[id].y, &berth[id].transport_time, &berth[id].loading_speed);
    }
    scanf("%d", &boat_capacity); //读入船的容量
    char okk[100];
    scanf("%s", okk);  //表示判题器输入完成
    printf("OK\n");
    fflush(stdout);
}

int Input()   //每一帧判题器的输入
{
    scanf("%d%d", &id, &money);  //帧序号  当前金钱数
    int num;
    scanf("%d", &num);       //新增货物的数量
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);  //新增货物的数量坐标和价值
    }
    for(int i = 0; i < robot_num; i ++)   
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);  //机器人是否携带货物 坐标  状态
    }
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);   //轮船的状态和目标泊位
    char okk[100];
    scanf("%s", okk);
    return id;  //返回当前帧数
}

int main()
{
    Init();
    for(int zhen = 1; zhen <= 15000; zhen ++)
    {
        int id = Input();
        for(int i = 0; i < robot_num; i ++)
            printf("move %d %d\n", i, rand() % 4);
        puts("OK");
        fflush(stdout);
    }

    return 0;
}
