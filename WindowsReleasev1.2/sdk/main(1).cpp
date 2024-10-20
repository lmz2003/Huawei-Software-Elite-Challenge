#pragma GCC optimize(2)

#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
using namespace std;

const int n = 200;
const int robot_num = 10;
const int berth_num = 10;
const int N = 210;

int money, boat_capacity, id;
char ch[N][N];


struct Robot
{
    int x, y, goods;   //机器人坐标和是否携带货物
    int status;        //机器人状态  恢复还是正常运行
	pair<int, int> now_good;
    Robot() {}
    Robot(int startX, int startY) {
        x = startX;
        y = startY;
    }
}robot[robot_num + 10];

//货物数据结构
struct Goods{
    //货物的位置信息
    int x=-1;

    int y=-1;
    //货物的价值
    int value = 0;
    // 记录它的帧数
    int startid = 0;
    bool operator==(const Goods& other) const {
    	return x == other.x && y == other.y;
    }
};
vector<Goods> goods;
// 对机器人的目标进行初始化
int dis_cost[20];
int gds[200][200];// 目标的货物
// 下一步的点对
pair<int,int> x_y_target[10];
// 经过调试是可以使用的
//void traverse_goods_list2delete(vector<Goods> &good, int id){
//    // 使用引用遍历向量中的元素
//    for(auto it = good.begin(); it != good.end();){
//        // 获取当前元素的引用
//        auto& g = *it;
//        // 如果当前的时间小于 id，那么就将其删除
//        if(g.id + 950 < id){
//            // 通过 erase 方法删除元素，并更新迭代器
//            it = good.erase(it);
//        } else {
//            // 如果不删除当前元素，则迭代器向后移动
//            ++it;
//        }
//    }
//}

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
    int arrival_time;
}boat[10];

int dx[4] = {1, -1, 0, 0};
int dy[4] = {0, 0, 1, -1};



void Init()  //判题器初始化
{
    for(int i = 0; i < n; i ++) {		  //读入地图信息
	  scanf("%s", ch[i]);
	  
    }
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

    scanf("%d%d", &id, &money);  //帧序号  当前金钱
    //对每一帧的货品进行错误性消除
    int num;

    scanf("%d", &num);       //新增货物的数量
    for(int i = 1; i <= num; i ++)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);  //新增货物的数量坐标和价值
        Goods new_goods;
        new_goods.x = x;
        new_goods.y = y;
        new_goods.value = val;
        new_goods.startid = id;
        // 将商品插入到vector数组中去。
        goods.push_back(new_goods);
    }
    // 记录机器人的状态信息
    for(int i = 0; i < robot_num; i ++)
    {
        int sts;
        scanf("%d%d%d%d", &robot[i].goods, &robot[i].x, &robot[i].y, &sts);
        //机器人是否携带货物 坐标  状态
        robot[i].status = sts;                                                    //把机器人的状态赋值`
    }
    // 记录轮船的状态和位置信息
    for(int i = 0; i < 5; i ++)
        scanf("%d%d\n", &boat[i].status, &boat[i].pos);   //轮船的状态和目标泊位
    char okk[100];

    scanf("%s", okk);
    return id;  //返回当前帧数
}

int dist[N][N];

void bfs(pair<int, int> start)
{
    for(int i = 0; i < n; i ++)      //初始化dist数组  距离
        for(int j = 0; j < n; j ++)
            dist[i][j] = 999999;
    dist[start.first][start.second] = 0;  //将起点的dist置为0
    queue<pair<int, int>> q;
    q.push(start);
    while (!q.empty())   //得到整个地图距离起点的距离
    {
        pair<int,int> cur = q.front();  //当前节点
        int x = cur.first;
        int y = cur.second;
        q.pop();
        for (int i = 0; i < 4; i++)
        {
            int newX = x + dx[i];
            int newY = y + dy[i];
            if (newX < 0 || newX >= n || newY < 0 || newY >= n || ch[newX][newY] == '#' || ch[newX][newY] == '*'|| dist[newX][newY] != 999999)
                continue;
            dist[newX][newY] = dist[x][y] + 1; //距离加一
            q.push(make_pair(newX, newY));
//            ch[newX][newY] = '*';
        }
    }

}

void update_Boat()
{
    //更新船舶状态
    for(int i=0;i<5;i++){

        if(boat[i].status == 0||boat[i].status == 2) continue;		//船处于运输状态
        if(boat[i].status == 1 && boat[i].pos == -1){    //船位于虚拟点，准备前往泊位   或船在泊位外等待
            int maxgoods = 0;
            int berth_target = 0;								//找出当前货物数量最多的泊位
            for(int j=0;j<berth_num;j++){
                if(berth[j].hasgoods >= maxgoods && berth[j].isboatcoming == 0){
                    maxgoods = berth[j].hasgoods;
                    berth_target = j;
                }
            }
			berth[berth_target].isboatcoming = 1;  //表示当前泊位即将有船到达
            printf("ship %d %d\n",i,berth_target);			//发出指令

        }
        else if(boat[i].status == 1 && boat[i].pos != -1){		//正常运行状态(即装货状态或运输完成状态)

            if(boat[i].arrival_time == 0){  //记录船刚到泊位的id
                boat[i].arrival_time = id;
            }
			
			int goods_get = (id - boat[i].arrival_time) * berth[boat[i].pos].loading_speed;
			
            if(goods_get >= berth[boat[i].pos].hasgoods ) {		//如果船上货物数量等于船的容量，则船运往虚拟点
				
				
				if(berth[boat[i].pos].hasgoods + boat[i].boatgoods< boat_capacity-12 && id < 13000){

                    boat[i].boatgoods += berth[boat[i].pos].hasgoods;
					berth[boat[i].pos].hasgoods = 0;
	                boat[i].arrival_time = 0;
	                berth[boat[i].pos].isboatcoming = 0;

	                int maxgoods = 0;
		            int berth_target = 0;								//找出当前货物数量最多的泊位
		            for(int j=0;j<berth_num;j++){
		                if(berth[j].hasgoods >= maxgoods && berth[j].isboatcoming == 0){
		                    maxgoods = berth[j].hasgoods;
		                    berth_target = j;
		                }
		            }
					berth[berth_target].isboatcoming = 1;  //表示当前泊位即将有船到达
		            printf("ship %d %d\n",i,berth_target);			//发出指令
		            
		            
				}
				else if(id >= 13000 && id + berth[boat[i].pos].transport_time <= 14990)
				{
					continue;
				}
				else {
                    printf("go %d\n", i);
                    boat[i].arrival_time = 0;
                    berth[boat[i].pos].isboatcoming = 0;
                    boat[i].boatgoods = 0;
                    berth[boat[i].pos].hasgoods = 0;
                }
            }
            else if(goods_get + boat[i].boatgoods >= boat_capacity){
					
				berth[boat[i].pos].hasgoods = berth[boat[i].pos].hasgoods - goods_get;
	            boat[i].arrival_time = 0;
	            berth[boat[i].pos].isboatcoming = 0;
	            boat[i].boatgoods = 0;
	            printf("go %d\n",i);
			}
			else if(id + berth[boat[i].pos].transport_time > 14990){
		        berth[boat[i].pos].hasgoods = 0;
		        boat[i].arrival_time = 0;
		        berth[boat[i].pos].isboatcoming = 0;
		        printf("go %d\n",i);     //船货物前往虚拟点，该泊位上无船，isboatcoming置为0，该泊位上的货物清零
		    }
        }
    }
}
pair<int,int> now_position[10];  	//机器人现在的位置
pair<int,int> next_position[10];          //机器人下一个位置
int wait[10] ={0,0,0,0,0,0,0,0,0,0};

void update_Robot()
{
    for(int i = 0;i < robot_num;i++)
        ch[now_position[i].first][now_position[i].second] = '.';
    //更新机器人状态
    for(int i = 0;i < 20;i++) dis_cost[i] = 899999;
    //机器人到泊位
    for(int i = 0; i < robot_num; i ++)
    {	
    	now_position[i].first = robot[i].x;
    	now_position[i].second = robot[i].y;
    	
    	ch[now_position[i].first][now_position[i].second] = '#';  //将当前机器人位置设为障碍
    	
        if(!robot[i].status)
            continue;

        //cerr<<"__________now robot "<<i<<endl;

        if(robot[i].goods)
        {
            int distance = 999999;
            pair<int,int> start = make_pair(robot[i].x,robot[i].y);
            bfs(start);

            int target = -1;

            for(int k = 0;k < berth_num;k++)
            {
                if(id > (15000- 2*berth[k].transport_time - 300) && berth[k].isboatcoming == 0) continue;
				
				int dist_to_berth = dist[berth[k].x][berth[k].y];   //机器人到泊位的距离
	            if(dist_to_berth < distance){
	            	
                    distance = dist_to_berth;			//找出该机器人到那个泊位最近
                    target = k;
            	}
			}
				
            if(distance == 999999){			//如果没找到泊位，则跳过该机器人
                continue;
            }

            if(robot[i].x >= berth[target].x && robot[i].x <= berth[target].x+3 && robot[i].y >= berth[target].y &&robot[i].y <= berth[target].y+3) {		
                berth[target].hasgoods++;		//泊位上货物数量增加
                printf("pull %d\n",i);			//如果机器人坐标在泊位坐标范围，则机器人卸除货物，更新机器人goods状态
                continue;
            }

            pair<int, int> next_point = {berth[target].x,berth[target].y};  //目标泊位的坐标

            while(dist[next_point.first][next_point.second] != 1){   //从目标泊位开始回溯路径
                for(int i = 0; i < 4; i++){
                    int newX = next_point.first + dx[i];
                    int newY = next_point.second + dy[i];
                    if(dist[newX][newY] == dist[next_point.first][next_point.second] - 1){
                        next_point = {newX, newY};				//next_point是下一步要走的位置
                        break;
                    }
                }
            }
            
            next_position[i] = next_point;
            ch[next_point.first][next_point.second] = '#'; //将机器人即将要走的位置也设为障碍
            int flag = 0;
            for(int j = 0;j < robot_num;j++)			//判断当前机器人下一步要走的位置有无其他机器人
            {
                if(j == i) continue;
                if(next_position[j] == next_point || now_position[i] == next_point){
                    next_point = {robot[j].x, robot[j].y};
                    flag = 1;
                    break;
                }
            }
            if(flag == 1) {
            	wait[i]++;
            	continue;
			}
			if(wait[i]>8){
				srand(time(nullptr));
				while(1){
					int dir = rand()%4;
					if(robot[i].x + dx[dir] < 0 || robot[i].x + dx[dir] >= n || robot[i].y + dy[dir] < 0 || robot[i].y + dy[dir] >= n || ch[robot[i].x + dx[dir]][robot[i].y + dy[dir]] == '#' || ch[robot[i].x + dx[dir]][robot[i].y + dy[dir]] == '*'){
						if(dir == 0) printf("move %d 3\n",i);			//更新机器人的坐标  输出指令
			            else if(dir == 1) printf("move %d 2\n",i);
			            else if(dir == 3) printf("move %d 1\n",i);
			            else if(dir == 2) printf("move %d 0\n",i);
			            wait[i] = 0;
			            break;
					}
				}
			}
//            ch[next_point.first][next_point.second] = '#';

//            robot_position[i] = next_point;

//			now_pos[i] = next_point;
			
            if(next_point.first-robot[i].x == 1) printf("move %d 3\n",i);			//更新机器人的坐标  输出指令
            else if(next_point.first-robot[i].x == -1) printf("move %d 2\n",i);
            else if(next_point.second-robot[i].y == -1) printf("move %d 1\n",i);
            else if(next_point.second-robot[i].y == 1) printf("move %d 0\n",i);

        }
            
        else{
            pair<int,int> start = make_pair(robot[i].x,robot[i].y);
            bfs(start);
			double dis_min = 999999;
            double dcost = 999999;
            Goods target_good[10];
			Goods old_good[10];
//			double pd = 9999999;
            for(auto good:goods){
            	//cerr<<good.x<<' '<<good.y<<' '<<dist[good.x][good.y]<<endl;
                if(good.x == -1 && good.y == -1) continue;
                if(gds[good.x][good.y] != i && gds[good.x][good.y] != -1) continue;
                if(id < 12000) dcost = 2*dist[good.x][good.y]-good.value - 0.08*(id - good.startid);
				else dcost = 2*dist[good.x][good.y]-good.value;
                if(dcost < dis_min){
                	//cerr<<good.x<<' '<<good.y<<' '<<"distance: "<<dcost<<endl;
                    dis_min = dcost;
                    x_y_target[i].first = good.x;
                    x_y_target[i].second = good.y;
                    target_good[i] = good;
                }
            }
            //cerr<<"target: "<<x_y_target[i].first<<' '<<x_y_target[i].second<<endl;
            if(dis_min >= 800000){
                continue;
            }
            gds[target_good[i].x][target_good[i].y] =i;
            
			if(old_good[i].x != target_good[i].x || old_good[i].y != target_good[i].y){
				gds[old_good[i].x][old_good[i].y] = -1;
			}	
			old_good[i] = target_good[i];
			
			
            

            //cerr<<"robot: "<<robot[i].x<<" "<<robot[i].y<<endl;
			//cerr<<"good: "<<x_y_target[i].first<<" "<<x_y_target[i].second<<endl;
            if(robot[i].x == x_y_target[i].first && robot[i].y == x_y_target[i].second){
                printf("get %d\n",i);
                gds[x_y_target[i].first][x_y_target[i].second] = 0;
                goods.erase(remove(goods.begin(), goods.end(), target_good[i]), goods.end());
                continue;
            }

            pair<int, int> next_point = {target_good[i].x,target_good[i].y};

            while(dist[next_point.first][next_point.second] != 1){        //目标货物回溯路径
                for(int i = 0; i < 4; i++){
                    int newX = next_point.first + dx[i];
                    int newY = next_point.second + dy[i];
                    if(dist[newX][newY] == dist[next_point.first][next_point.second] - 1){
                        next_point = {newX, newY};
                        break;
                    }
                }
            }
            
            //cerr<<"next"<<next_point.first<<' '<<next_point.second<<endl;
            
            next_position[i] = next_point;
            ch[next_point.first][next_point.second] = '#'; //将机器人即将要走的位置也设为障碍
            
            int flag = 0;
            
            for(int j = 0;j < robot_num;j++)			//判断当前机器人下一步要走的位置有无其他机器人
            {
                if(j == i) continue;
                if(next_position[j] == next_point || now_position[i] == next_point){
                    next_point = {robot[j].x, robot[j].y};
                    flag = 1;
                    break;
                }
            }
            if(flag == 1) {
            	wait[i]++;
            	continue;
			}
			
			if(wait[i]>8){
				
				while(1){
					int dir = rand()%4;
					if(robot[i].x + dx[dir] < 0 || robot[i].x + dx[dir] >= n || robot[i].y + dy[dir] < 0 || robot[i].y + dy[dir] >= n || ch[robot[i].x + dx[dir]][robot[i].y + dy[dir]] == '#' || ch[robot[i].x + dx[dir]][robot[i].y + dy[dir]] == '*'){
						if(dir == 0) printf("move %d 3\n",i);			//更新机器人的坐标  输出指令
			            else if(dir == 1) printf("move %d 2\n",i);
			            else if(dir == 3) printf("move %d 1\n",i);
			            else if(dir == 2) printf("move %d 0\n",i);
			            wait[i] = 0;
			            break;
					}
				}
			}
            
            // 代表找到点对，直接走就行
            //0 you 1 zuo 2 shang 3 xia
            //cerr<<next_point.second.first-robot[i].x<<endl;
            if(next_point.first-robot[i].x == 1) printf("move %d 3\n",i);			//更新机器人的坐标  输出指令
            else if(next_point.first-robot[i].x == -1) printf("move %d 2\n",i);
            else if(next_point.second-robot[i].y == -1) printf("move %d 1\n",i);
            else if(next_point.second-robot[i].y == 1) printf("move %d 0\n",i);
        }
    }
}
void Update_Item()
{
	for(auto good:goods)
	{
		if(good.startid + 960 < id) goods.erase(remove(goods.begin(), goods.end(), good), goods.end());
	}
}
// 更新机器人和船舶状态和位置
void Update()
{
    update_Boat();
    update_Robot();
    Update_Item();
}

int main()
{
    Init();
    
    for(int i=0;i<200;i++){
    	for(int j=0;j<200;j++){
    		gds[i][j] = -1;
		}
	}
    
    int total_frames = 15000; // 总共运行的帧数
    int current_frame = 0;

    for(int i=0;i<5;i++){			//每艘船上的货物初始化为零
        boat[i].boatgoods = 0;
        boat[i].arrival_time = 0;   //每艘船到达泊位的开始id设为0
    }

    for(int i=0;i<berth_num;i++){		//每个泊位上的货物初始化为零
        berth[i].hasgoods = 0;
    }
	
    while(current_frame < total_frames)
    {
        id = Input();
        Update();
        puts("OK");
        fflush(stdout);
        current_frame++;
    }

    return 0;
}


//PreliminaryJudge.exe -f 0 -m maps/map1.txt "main.exe"