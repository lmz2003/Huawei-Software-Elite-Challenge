#include <bits/stdc++.h>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;


typedef struct Open_list Open_list, * pOpen_list;
typedef struct Node1
{
    //父节点
    pOpen_list  pFather;
    float G;
    float H;
    //F值
    float F;
    //x,y坐标
    int x;
    int y;
}Node1, * pNode1;

typedef struct Open_list
{
    struct Open_list * next;
    struct Node1 Node1;
}Open_list, * pOpen_list;

//---------------------------------------------------------------------------//
//相关函数
float my_abs(int x);
//距离函数（给两个坐标(x1,y1)和（x2，y2））
float my_distance(int x1, int y1, int x2, int y2);
//添加链表
void list_add_tail(pOpen_list my_list, pOpen_list add_Node1);
//遍历链表,返回与tmpY和tmpY匹配的节点指针
pOpen_list list_browse(pOpen_list my_list, int tmpX, int tmpY);
//判断链表中是不是有某个节点（通过坐标来确定），有的话返回0，没有返回1
int judge_Node1_exist(pOpen_list mylist, int x, int y);
//删除链表中某个节点，通过坐标删除,并且返回这个删除的节点指针，方便加入到close list
pOpen_list list_delete_point(pOpen_list my_list, int tmpX, int tmpY);
//找到链表中最小的f值的函数,输入链表，返回最小f的节点
pOpen_list find_min_f(pOpen_list my_list);
//打印open list中的各个节点坐标以及F值
void msg_open_list(pOpen_list my_list);
//打印父节点坐标函数
void printf_father_Node1(pOpen_list my_list,vector<pair<int, int>> &path);

//---------------------------------------------------------------------------//

struct Robot
{
    int x, y, goods;   //机器人坐标和是否携带货物
    int status;        //机器人状态  恢复还是正常运行
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
    int isboatcoming;  	//泊位上是否即将有船进入
    int hasgoods;   //泊位上货物数量 
    double berth_advantage;
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
    int boatgoods;  //船上货物数量
}boat[10];

int money, boat_capacity, id;
char ch[N][N];
int gds[N][N];
int dir[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; //四个方向


void AStar(int START_X,int START_Y,int END_X,int END_Y,vector<pair<int, int>> &path){
    //************************************************************************//
    //2.把起点放入open list

    //创建一个指针pO指向open list和一个指针pC指向close list，注意现在open list是pO指向的那块内存
    pOpen_list pO = (pOpen_list)malloc(sizeof(Open_list));
    pO->next = NULL;
    //pO = NULL;  千万不可以，NULL不可以访问的，除非pO赋值
    pOpen_list pC = (pOpen_list)malloc(sizeof(Open_list));
    pC->next = NULL;

    //创建起始节点并初始化，创建一个目标节点
    pOpen_list start = (pOpen_list)malloc(sizeof(Open_list));
    start->next = NULL;
    pOpen_list end = (pOpen_list)malloc(sizeof(Open_list));
    end->next = NULL;

    start->Node1.pFather = NULL;
    start->Node1.x = START_X;
    start->Node1.y = START_Y;
    start->Node1.G = 0;
    start->Node1.H = (my_abs(END_X - START_X) + my_abs(END_Y - START_Y));
    start->Node1.F = start->Node1.G + start->Node1.H;

    end->Node1.pFather = NULL;
    end->Node1.x = END_X;
    end->Node1.y = END_Y;

    //起始节点加入到open list
    list_add_tail(pO, start);


    int i,j;    //计数来遍历

    //************************************************************************//
    int cir = 1;
    while(cir)
    {
        //printf("-------------------begin while--------------\n");
        //寻找最小的F值节点，记为pCurrent（第一次循环也就是起点）
        //pOpen_list pCurrent = (pOpen_list)malloc(sizeof(Open_list));
//        msg_open_list(pO);	//现在open list中有的节点
        pOpen_list pCurrent = find_min_f(pO);  //有可能找不到

        //*********************************************************************//

        //把当前点从open list中移除（通过坐标），加入到close list，记为p
        pOpen_list p = list_delete_point(pO, pCurrent->Node1.x, pCurrent->Node1.y);
        list_add_tail(pC, p);
//        printf("core is (%d, %d)\n", p->Node1.x, p->Node1.y);
//
//        printf("now ,the open list is as follow\n");
//        msg_open_list(pO);
        //printf("------------------------begin for  for------------------\n");

        //*********************************************************************//

        //还要考虑是不是障碍物，有没有在close list或者open list,考虑是不是边界
        //这块应该是遍历当前节点(p->Node1.x, p->Node1.y)的四周，并且都加入open list
        for(i = -1; i < 2; i++)
        {
            for(j = -1; j < 2; j++)
            {
                if((j==-1&&i==0)||(j==1&&i==0)||(j==0&&i==-1)||(j==0&&i==1)){
                    if((p->Node1.x + i < 1) || (p->Node1.x + i > 200) || (p->Node1.y + j < 1) || (p->Node1.y + j > 200))	//超过边界了，跳过这次循环
                        continue;

                    if(judge_Node1_exist(pO, (p->Node1.x + i), (p->Node1.y + j)))    //不是open list里面的节点
                    {
                        if("#" == ch[p->Node1.x + i][p->Node1.y + j] || "*" == ch[p->Node1.x + i][p->Node1.y + j])   //不可到达
                        {
                            continue;
                        }
                        else if(!(judge_Node1_exist(pC, (p->Node1.x + i), (p->Node1.y + j))))   //在close list中
                        {
                            continue;
                        }
                        else if(((p->Node1.x + i)==END_X) && ((p->Node1.y+j)==END_Y))  //是目标节点，初始化
                        {
//                            printf("hahahahhaha,zhaodaole\n");
                            end->Node1.pFather = p;		//当前节点设为end 的父节点
                            cir = 0;	//跳出循环标志，注意跳出的是for
                            break;
                        }
                        else    //不在open list中的普通节点，加入进去初始化并设好父节点
                        {
                            pOpen_list pTmp = (pOpen_list)malloc(sizeof(Open_list));
                            pTmp->next =NULL;

                            pTmp->Node1.pFather = p;  //父节点为当前节点
                            //节点坐标
                            pTmp->Node1.x = p->Node1.x + i;
                            pTmp->Node1.y = p->Node1.y + j;
                            //节点G.H.F值       G值怎么算好(找到父节点和现在节点的坐标了)
                            pTmp->Node1.G = my_distance(pTmp->Node1.x, pTmp->Node1.y, START_X, START_Y);   //初始节点到其实际距离
                            pTmp->Node1.H = (my_abs(END_X - pTmp->Node1.x) + my_abs(END_Y - pTmp->Node1.y));
                            pTmp->Node1.F = pTmp->Node1.G + pTmp->Node1.H;

                            //加入到open list
                            list_add_tail(pO, pTmp);
//                            msg_open_list(pO);
                        }
                    }
                    else    //在open list中了
                    {
                        //首先根据坐标找到他的指针
                        pOpen_list pTmp = list_browse(pO, (p->Node1.x + i), (p->Node1.y + j));
                        //定义好两个的G值(一个是原本的G值，一个是通过当前节点到的G值)
                        //所以用核心节点的G值加上当前节点到核心节点的值
                        float currentG = p->Node1.G + my_distance(pTmp->Node1.x, pTmp->Node1.y, p->Node1.x, p->Node1.y);
                        float pastG = pTmp->Node1.G;

                        if(currentG < pastG)    //当前更优
                        {
                            pTmp->Node1.pFather = p; //更换父节点
                            //注意，更改F值和G值一定
                            pTmp->Node1.G = currentG;
                            pTmp->Node1.F = pTmp->Node1.G + pTmp->Node1.H;
                        }
                    }
                }
            }
            if(cir == 0)	//跳出外层的for循环
                break;
        }
    }

    printf_father_Node1(end,path);

}



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
        robot[i].status = sts;            //把机器人的状态赋值`
    }
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);   //轮船的状态和目标泊位
    char okk[100];
    scanf("%s", okk);
    return id;  //返回当前帧数
}


// 更新机器人和船舶状态和位置
void Update()
{
	
	//更新船舶状态
	for(int i=0;i<5;i++){
		if(boat[i].status == 0) continue;		//船处于运输状态 
		
		else if((boat[i].status == 1 &&boat[i].pos == -1) ||  boat[i].status == 2){    //船位于虚拟点，准备前往泊位   或船在泊位外等待
			int sumgoods = 0;
			int sumtimes = 0;
			int sumspeed = 0;
			int advantage = 0;
			int berth_target = 0;								//根据泊位现有货物、运输时间、装载速度来为泊位划分优先级berth_advantage
			for(int j=0;j<berth_num;j++){
				sumgoods += berth[j].hasgoods;
				sumtimes += berth[j].transport_time;
				sumspeed += berth[j].loading_speed;
			}
			for(int j=0;j<berth_num;j++){
				berth[j].berth_advantage = (double)berth[j].hasgoods/sumgoods - (double)berth[j].transport_time/sumtimes + (double)berth[j].loading_speed/sumspeed;
			}
			

			for(int j=0;j<berth_num;j++){
				if(berth[j].berth_advantage>advantage && berth[j].isboatcoming == 0){   //船根据泊位优先级和该泊位此时是否有船来    来选择目标泊位
					advantage = berth[j].berth_advantage;
					berth_target = j;
				}
			}
			printf("ship %d %d\n",i,berth_target);			//发出指令
			
		}
		
		else if(boat[i].status == 1 && boat[i].pos != -1){		//正常运行状态(即装货状态或运输完成状态) 
			
			berth[boat[i].pos].isboatcoming = 1;  //表示当前泊位即将有船到达
			
			if(boat[i].boatgoods>=boat_capacity) {		//如果船上货物数量等于船的容量，则船运往虚拟点
			
				berth[boat[i].pos].isboatcoming = 0;
				printf("go %d\n",i);     //船货物装满前往虚拟点，该泊位上无船，isboatcoming置为0
				
			}
			
			//如果船容量-船货物<泊位装载速度<泊位剩余货物
			else if(boat_capacity-boat[i].boatgoods < berth[boat[i].pos].loading_speed && berth[boat[i].pos].hasgoods >= berth[boat[i].pos].loading_speed){
				
				boat[i].boatgoods = boat_capacity;
				berth[boat[i].pos].hasgoods -= boat_capacity-boat[i].boatgoods;
				
			}
			
			//如果泊位剩余货物<船容量-船货物<泊位装载速度
			else if(boat_capacity-boat[i].boatgoods < berth[boat[i].pos].loading_speed &&boat_capacity-boat[i].boatgoods > berth[boat[i].pos].hasgoods){
				
				boat[i].boatgoods += berth[boat[i].pos].hasgoods;
				berth[boat[i].pos].hasgoods = 0;
			}
	
			//如果泊位装载速度<船容量-船货物，泊位装载速度<泊位剩余货物
			else if(boat_capacity-boat[i].boatgoods > berth[boat[i].pos].loading_speed && berth[boat[i].pos].hasgoods >= berth[boat[i].pos].loading_speed){
				
				boat[i].boatgoods += berth[boat[i].pos].loading_speed;
				berth[boat[i].pos].hasgoods -= berth[boat[i].pos].loading_speed;
			}
			//如果船容量-船货物>泊位装载速度，泊位装载速度>泊位剩余货物
			else if(boat_capacity-boat[i].boatgoods > berth[boat[i].pos].loading_speed && berth[boat[i].pos].hasgoods < berth[boat[i].pos].loading_speed){
				
				boat[i].boatgoods += berth[boat[i].pos].hasgoods;
				berth[boat[i].pos].hasgoods = 0;
			}
		}
	}
	
	
	//更新机器人状态
	
	//机器人到泊位
    for(int i = 0; i < robot_num; i ++)
    {
        if(!robot[i].status)
            continue;
        if(robot[i].goods)
        {	
        	int distance=99999;
        	int target = 0;
        	for(int k=0;k<berth_num;k++){				//先算直线距离，找出离机器人最近的泊位
        		if(abs(berth[k].x-robot[i].x)+abs(berth[k].y-robot[i].y)<distance){
        			distance = abs(berth[k].x-robot[i].x)+abs(berth[k].y-robot[i].y);
        			target = k;
				}
			}
			
			if(robot[i].x >= berth[target].x && robot[i].x <= berth[target].x+3 && robot[i].y >= berth[target].y && robot[i].y <= berth[target].y+3) {   
				robot[i].goods = 0;			//如果机器人坐标在泊位坐标范围，则机器人卸除货物，更新机器人goods状态
				berth[target].hasgoods++;		//泊位上货物数量增加
				printf("pull %d",i);
			}
			vector<pair<int, int>> path;
            AStar(robot[i].x, robot[i].y, berth[target].x, berth[target].y, path);   //算出机器人与泊位之间的最短路径
            
            if(path[0].first-robot[i].x == 1) printf("move %d 0\n",i);			//更新机器人的坐标  输出指令
            else if(path[0].first-robot[i].x == -1) printf("move %d 1\n",i);
            else if(path[0].second-robot[i].y == -1) printf("move %d 2\n",i);
            else if(path[0].second-robot[i].y == 1) printf("move %d 3\n",i);
//            robot[i].x = path[0].first;
//            robot[i].y = path[0].second;
            
            
        }
        else     //机器人与货物之间  还未修改
        {
            //待填充
        }
    }
}

int main()
{
    Init();
    int total_frames = 15000; // 总共运行的帧数
    int current_frame = 0;
    int r=0;
    int b=0;
    //寻找初始地图中机器人的位置
    for(int i=1;i<=n;i++){
    	for(int j=1;j<=n;j++){
    		if(ch[i][j] == 'A') {
    			robot[r].x = i;
    			robot[r].y = j;
    			r++;
			}
//			if(ch[i][j] == 'B' &&ch[i+3][j+3] == 'B'){
//				berth[b].x = i;
//				berth[b].y = j;
//				b++;
//			}
		}
	}
	for(int i=0;i<5;i++){			//每艘船上的货物初始化为零
		boat[i].boatgoods = 0;
	}
	
	for(int i=0;i<berth_num;i++){		//每个泊位上的货物初始化为零
		berth[i].hasgoods = 0;
	}

    while(current_frame < total_frames)
    {
        int num = Input();
        
        if(num == -1)
            break;
        Update();
        printf("OK");
        
    }

    return 0;
}


//尾部插入链表
void list_add_tail(pOpen_list my_list, pOpen_list add_Node1)
{
    pOpen_list tmp = my_list;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = add_Node1;
    add_Node1->next = NULL;
}

//删除链表中某个节点，通过坐标删除,并且返回这个删除的节点指针，方便free
pOpen_list list_delete_point(pOpen_list my_list, int x, int y)
{

    while (my_list->next != NULL)
    {
        if((my_list->next->Node1.x == x) && (my_list->next->Node1.y == y))  //找到删除节点
        {
            pOpen_list tmp = my_list->next;
            //这个节点不是最后一个节点
            if(my_list->next->next != NULL)
            {
                my_list->next = my_list->next->next;

                tmp->next = NULL;
            }
            else //最后一个节点
            {
                my_list->next = NULL;

                tmp->next = NULL;
            }
            return tmp;
        }
        my_list = my_list->next;
    }

    return NULL;
}

//遍历链表,返回与p->Node1.y和p->Node1.y匹配的节点指针
pOpen_list list_browse(pOpen_list my_list, int x, int y)
{

    while (my_list->next != NULL)
    {
        if((my_list->next->Node1.x == x) && (my_list->next->Node1.y == y))
        {
            return my_list->next;
        }
        my_list = my_list->next;
    }

    return NULL;
}

//判断链表中是不是有某个节点（通过坐标来确定），有的话返回0，没有返回1
int judge_Node1_exist(pOpen_list mylist, int x, int y)
{
    while(mylist->next != NULL)
    {
        if((mylist->next->Node1.x == x) && (mylist->next->Node1.y == y))	//在open list
        {
            return 0;
        }
        mylist = mylist->next;
    }

    return 1;
}

//找到链表中最小的f值的函数,输入链表，返回最小f的节点
pOpen_list find_min_f(pOpen_list my_list)
{
    //定义一个临时变量tmpf为第二个节点的F值，挨个比下去
    int tmpf = my_list->next->Node1.F;
    pOpen_list tmpp = my_list->next;

    while(my_list->next != NULL)
    {

        if(tmpf > my_list->next->Node1.F)
        {
            tmpf = my_list->next->Node1.F;
            tmpp = my_list->next;       //用一个循环就可以找到，注意！！！
        }
        my_list = my_list->next;
    }

    //找到了F值即为tmpf，怎么找到对应的节点，为什么不跟着定义一个临时节点呢
    return tmpp;
}

//打印open list中的各个节点坐标以及F值
void msg_open_list(pOpen_list my_list)
{
    while(my_list->next != NULL)
    {
        int x = my_list->next->Node1.x;
        int y = my_list->next->Node1.y;

        float f = my_list->next->Node1.F;
        printf("is (%d, %d).   F = %f\n", x, y, f);

        my_list = my_list->next;
    }
}

//打印父节点坐标函数
void printf_father_Node1(pOpen_list my_list,vector<pair<int, int>> &path)
{
    while(my_list->Node1.pFather->Node1.pFather != NULL)
    {
//        printf("(%d, %d)\n", my_list->Node1.x, my_list->Node1.y);
        my_list = my_list->Node1.pFather;
    }
    path.emplace_back(my_list->Node1.x,my_list->Node1.y);
//    printf("track end\n");
}

//绝对值函数（我的）
float my_abs(int x)
{
    if(x < 0)
    {
        return (float)(-x);
    }
    else
    {
        return (float)(x);
    }
}

//距离函数（给两个坐标(x1,y1)和（x2，y2））
float my_distance(int x1, int y1, int x2, int y2)
{
    return sqrt(  (my_abs(x1-x2)*my_abs(x1-x2)) + (my_abs(y1-y2)*my_abs(y1-y2)) );
}

