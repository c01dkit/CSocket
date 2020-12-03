#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>

#define SEND_MSG_TARGET_BIT 20
#define CONTENT_LENGTH_BIT 24
#define PACKAGE_TYPE_BIT 28
#define ACTION_TYPE_BIT 29
#define CONTENT_BIT 30
#define PACKAGE_TYPE_GET 'G'
#define PACKAGE_TYPE_RESPONSE 'R'
#define PACKAGE_TYPE_ORDER 'O'
#define ACTION_TYPE_TIME 'T'
#define ACTION_TYPE_NAME 'N'
#define ACTION_TYPE_CLIENT_LIST 'L'
#define ACTION_TYPE_CONNECT_SUCCESS 'S'
#define ACTION_TYPE_CONNECT_FAIL 'F'
#define ACTION_TYPE_SEND_MESSAGE 'M'
#define ACTION_TYPE_CLOSE 'C'

//定义状态
enum bool{false, true} connectState=false;
enum state{STOP, RUN, BLOCK};
volatile enum state messageState,receiverState,mainState;
volatile int id;
//定义数据包格式，使用柔性数组
struct data{
    char time[20];
    int targetID;
    int contentLength;
    char packageType;
    char actionType;
    char body[];
}model;

//相关参数
char targetIp[17];
u_short targetPort;
SOCKET serverSock;
struct sockaddr_in sockAddr;
char responseBuffer[MAXWORD] = {0};
char messageBuffer[MAXWORD] = {0};
char requestBuffer[MAXWORD] = {0};
const char* menu = "[1] 连接服务器\n[2] 断开服务器连接\n[3] 获取时间\n[4] 获取名字\n"
                   "[5] 获取客户端列表\n[6] 发送消息\n[7] 退出程序\n\n";

//volatile int tempCounter = -1;

void packData(char type,char action){
    memset(requestBuffer,0,sizeof(requestBuffer));
    //设置数据包头部信息 包括时间、数据包类别、数据包动作、数据体大小、通信客户端编号
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    sprintf(requestBuffer,"%4d/%02d/%02d %02d:%02d:%02d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond);
    int length = (int)strlen(messageBuffer);
    strcpy(&requestBuffer[CONTENT_LENGTH_BIT], (char*)&length);
    strcpy(&requestBuffer[SEND_MSG_TARGET_BIT], (char*)&id);
    requestBuffer[PACKAGE_TYPE_BIT] = type;
    requestBuffer[ACTION_TYPE_BIT] = action;
    //填充数据体
    strcpy(&requestBuffer[CONTENT_BIT], messageBuffer);
    send(serverSock, requestBuffer, CONTENT_BIT + length + sizeof(char), 0);
    memset(requestBuffer,0,sizeof(requestBuffer));
}

struct data* unpackData(){
    //获得数据体大小
    int contentSize = *(int*)&responseBuffer[CONTENT_LENGTH_BIT];
    //为柔性数组分配内存
    struct data* unpacked = (struct data*)malloc(sizeof(model)+contentSize+1);
    memset((void*)unpacked,0,sizeof(model)+contentSize+1);
    //保存数据包头部
    memcpy((void*)unpacked,(void*)responseBuffer,sizeof(model));
    //保存数据包体
    memcpy(unpacked->body, &responseBuffer[CONTENT_BIT], contentSize);
    memset(responseBuffer,0,sizeof(responseBuffer));
    return unpacked;
}

void receiver(){
    receiverState = RUN;
    while(receiverState != STOP){
        recv(serverSock, responseBuffer, MAXWORD, 0);
//        tempCounter++;
        messageState = BLOCK;
        while(messageState == BLOCK); //自旋等待
    }
    _endthread();
}

void closeSocket(){
    if (connectState){
        messageBuffer[0] = '\0';
        packData(PACKAGE_TYPE_GET,ACTION_TYPE_CLOSE);
        connectState = false;
        receiverState = STOP;
        printf("当前连接已被终止，感谢使用。\n");
        closesocket(serverSock);
    }
}

void message(){
    messageState = RUN;
    while (messageState != STOP){
        if (messageState == BLOCK){
            struct data* unpacked = unpackData();
            //自定义处理方式
            switch (unpacked->packageType) {
                case PACKAGE_TYPE_RESPONSE://处理响应包
                    if (unpacked->actionType == ACTION_TYPE_TIME){
                        printf("服务器当前时间为 : %s",unpacked->time);
                    } else if ( unpacked->actionType == ACTION_TYPE_CONNECT_SUCCESS){
                        connectState = true;
                        printf("%s",unpacked->body);
                    } else {
                        printf("%s",unpacked->body);
                    }
                    break;
                case PACKAGE_TYPE_ORDER://处理指示包
                    if (unpacked->actionType == ACTION_TYPE_CONNECT_FAIL){
                        closeSocket();
                        printf("\n[未连接] 请输入操作编号\n");
                    } else if (unpacked->actionType == ACTION_TYPE_SEND_MESSAGE){
                        printf("[收到其他客户端的消息]\n对方客户端编号：%d\n发送时间：%s",unpacked->targetID,unpacked->time);
                        printf("%s",unpacked->body);
                        printf("\n[已连接] 请输入操作编号\n");
                    }
                    break;
            }
            free(unpacked);
            while (messageState == BLOCK) messageState = RUN;
            while (mainState == BLOCK) mainState = RUN;
        }
    }
    _endthread();
}



void connectServer(){
    if (connectState){
        //关闭套接字
        closeSocket();
    }
    //创建套接字
    serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET; //IPv4
    printf("请输入需要连接的目标IP与端口 (目前只能连接 127.0.0.1 2943)\n");
    scanf("%s%hu", targetIp, &targetPort);
    sockAddr.sin_addr.s_addr = inet_addr(targetIp);
    sockAddr.sin_port = htons(targetPort);
    if (connect(serverSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1){
        printf("连接失败，请检查输入是否有误\n");
        while (mainState == BLOCK) mainState = RUN;
    } else {
        _beginthread(receiver,0,NULL);
    }
}

void sendMessage(){
    fflush(stdin);
    printf("请输入发送给[%d]号客户端的信息，需要结束时请连按两次回车。\n",id);
    char ch;
    int index = 0;
    enum bool over = false;
    while(true){
        ch = (char)getchar();
        if (ch == '\n'){
            if (over) break;
            else over = true;
        } else over = false;
        messageBuffer[index++] = ch;
    }
    messageBuffer[index] = '\0';
    packData(PACKAGE_TYPE_GET,ACTION_TYPE_SEND_MESSAGE);
}
int main(){
    //初始化DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //打印菜单
    printf("%s", menu);

    //以消息处理线程的方式实现线程间通信
    _beginthread(message,0,NULL);
    //循环接收用户操作
    mainState = RUN;
    while (mainState != STOP){
        int op;
        //显示连接状态
        if (connectState){
            printf("\n[已连接] 请输入操作编号\n");
        } else {
            printf("\n[未连接] 请输入操作编号\n");
        }
        //用户操作判断
        fflush(stdin);
        scanf("%d",&op);
        if (!connectState && op >= 3 && op <= 6){
            printf("请先连接服务器！\n");
            continue;
        }
        switch (op) {
            case 1: //连接
                mainState = BLOCK;
                connectServer();
                break;
            case 2: //断开连接
                closeSocket();
                break;
            case 3: //获取时间
                mainState = BLOCK;
//                for (int i = 0 ; i <100 ; i++){
                    messageBuffer[0] = '\0';
                    packData(PACKAGE_TYPE_GET,ACTION_TYPE_TIME);
//                }
//                Sleep(1000);
//                printf("共收到了%d个时间响应\n",tempCounter);
//                tempCounter = 0;
                break;
            case 4: //获取名字
                mainState = BLOCK;
                messageBuffer[0] = '\0';
                packData(PACKAGE_TYPE_GET,ACTION_TYPE_NAME);
                break;
            case 5: //获取客户端列表
                mainState = BLOCK;
                messageBuffer[0] = '\0';
                packData(PACKAGE_TYPE_GET,ACTION_TYPE_CLIENT_LIST);
                break;
            case 6: //发送消息
                mainState = BLOCK;
                printf("请输入通讯目标的ID编号 (使用[5]号功能进行查询)\n");
                scanf("%d",&id);
                sendMessage();
                break;
            case 7: //退出
                mainState = STOP;
                closeSocket();
                break;
            default:
                printf("操作编号无效\n");
                break;
        }
        while(mainState == BLOCK);//自旋等待
    }
    //通知并等待子线程结束
    while(messageState!=STOP || receiverState!=STOP){
        messageState = STOP;
        receiverState = STOP;
    }
    //终止使用 DLL
    closesocket(serverSock);
    WSACleanup();

    system("pause");
    return 0;
}



