#include <stdio.h>
#include <winsock2.h>
#include <process.h>

//相关参数
#define SEND_MSG_SOURCE_BIT 20
#define CONTENT_LENGTH_BIT 24
#define PACKAGE_TYPE_BIT 28
#define ACTION_TYPE_BIT 29
#define CONTENT_BIT 30
#define LIST_SIZE 3
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
enum state{STOP, RUN, BLOCK};
volatile enum state handleState[LIST_SIZE],mainState=RUN;
volatile int id;
char requestBuffer[MAXWORD] = {0};
char messageBuffer[MAXWORD] = {0};
char responseBuffer[MAXWORD] = {0};
SOCKET servSock;

//定义数据包格式，使用柔性数组
struct data{
    char time[20];
    int sourceID;
    int contentLength;
    char packageType;
    char actionType;
    char body[];
}model;

//记录客户端连接状态
struct connection{
    u_short port;
    struct in_addr	ip;
    SOCKET  clientSock;
    int     state;
}clientList[LIST_SIZE];

void packData(char type, char action, SOCKET targetSock){
    memset(responseBuffer,0,sizeof(responseBuffer));
    //设置数据包头部信息 包括时间、数据包类别、数据包动作、数据体大小
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    sprintf(responseBuffer,"%4d/%02d/%02d %02d:%02d:%02d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond);
    int length = (int)strlen(messageBuffer);
    strcpy(&responseBuffer[CONTENT_LENGTH_BIT], (char*)&length);
    strcpy(&responseBuffer[SEND_MSG_SOURCE_BIT], (char*)&id);
    responseBuffer[PACKAGE_TYPE_BIT] = type;
    responseBuffer[ACTION_TYPE_BIT] = action;
    //填充数据体
    strcpy(&responseBuffer[CONTENT_BIT], messageBuffer);
    send(targetSock, responseBuffer, CONTENT_BIT + length + sizeof(char), 0);
    memset(responseBuffer,0,sizeof(responseBuffer));
}

/** 此函数用于解析请求包
 * 服务端需要同时保存多个请求 需要在堆上为它们开辟空间
 * 使用全局缓冲区会导致数据冲突 虽然可以为每个子线程单独定义缓冲区
 * 但考虑到单机手工操作时不会出现高并发问题 所以选择了单缓冲区以节省大量空间
 */
struct data* unpackData(){
    //获得数据体大小
    int contentSize = *(int*)&requestBuffer[CONTENT_LENGTH_BIT];
    //为柔性数组分配内存
    struct data* unpacked = (struct data*)malloc(sizeof(model)+contentSize+1);
    //保存数据包头部
    memcpy((void*)unpacked,(void*)requestBuffer,sizeof(model));
    //保存数据包体
    memcpy(unpacked->body, &requestBuffer[CONTENT_BIT], contentSize);
    memset(requestBuffer,0,sizeof(requestBuffer));
    return unpacked;
}
void clientListResponse(int index, SOCKET client){
    int count = 0; //用于计数
    char tempBuffer[MAXBYTE] = {0}; //用于组装单条消息
    memset(messageBuffer,0,MAXWORD);
    for (int check = 0; check < LIST_SIZE; check++){
        if (clientList[check].state){
            count++;
            sprintf(tempBuffer,"[%d] 客户端 -- 来自%s:%d\n",check,
                    inet_ntoa(clientList[check].ip),clientList[check].port);
            strcat(messageBuffer,tempBuffer);
        }
    }
    sprintf(tempBuffer,"共有%d个客户端与服务端相连，其中当前客户端编号为 %d \n",count,index);
    strcat(messageBuffer,tempBuffer);
    packData(PACKAGE_TYPE_RESPONSE,ACTION_TYPE_CLIENT_LIST,client);
    while (handleState[index] == BLOCK) handleState[index] = RUN;
}

void sendMessage(int index, SOCKET client, struct data* unpacked){
    int target = unpacked->sourceID;
    printf("[请求] %d 请求发送消息至 %d 号客户端\n",index,target);
    memset(messageBuffer,0,MAXWORD);
    if (target < 0 ||target >= LIST_SIZE){
        sprintf(messageBuffer,"发送失败：无效的目标客户端编号\n");
        printf(" 发送失败：无效的目标客户端编号\n");
    } else if (clientList[target].state == STOP){
        sprintf(messageBuffer,"发送失败：目标客户端未连接至服务器\n");
        printf( " 发送失败：目标客户端未连接至服务器\n");
    } else {
        strcpy(messageBuffer,unpacked->body); //填充转发的数据
        id = index; //设置数据发送方为当前client的编号
        printf("[指示] 向 %d 号客户端发送消息：\n%s",target,unpacked->body);
        packData(PACKAGE_TYPE_ORDER,ACTION_TYPE_SEND_MESSAGE,clientList[target].clientSock);
        sprintf(messageBuffer,"发送成功！\n");
        printf( "[ * ] 发送成功\n");
    }
    packData(PACKAGE_TYPE_RESPONSE,ACTION_TYPE_SEND_MESSAGE,client);
    while(handleState[index] == BLOCK) handleState[index] = RUN;
}

void handleRequest(void* i){
    int index = *(int*)i;
    SOCKET client = clientList[index].clientSock;
    printf("[ * ] 已同客户端建立新的连接，使用连接槽编号为 %d \n",index);

    handleState[index] = RUN;
    sprintf(messageBuffer,"[HELLO] 已连接至服务器。退出时请使用命令[2]或命令[7]\n");
    packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_CONNECT_SUCCESS, client);
    while (handleState[index]){
        int a = recv(client, requestBuffer, MAXWORD, 0);
        if(a < 0) break;
        if (handleState[index] == STOP) break;
        struct data* unpacked = unpackData();
        //服务器收到的包一定都是请求包，所以只检测动作类型
        switch (unpacked->actionType) {
            case ACTION_TYPE_TIME:
                messageBuffer[0] = '\0';
                printf("[响应] 响应来自 %d 号客户端的时间请求\n",index);
                packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_TIME, client);
                break;
            case ACTION_TYPE_NAME:
                sprintf(messageBuffer,"服务器名称为 : c01dkit\n");
                printf("[响应] 响应来自 %d 号客户端的名称请求\n",index);
                packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_NAME, client);
                break;
            case ACTION_TYPE_CLOSE:
                handleState[index] = STOP;
                clientList[index].state = STOP;
                printf("[请求] 关闭 %d 号客户端的连接\n",index);
                break;
            case ACTION_TYPE_CLIENT_LIST:
                handleState[index] = BLOCK;
                printf("[响应] 响应来自 %d 号客户端的列表请求\n",index);
                clientListResponse(index, client);
                break;
            case ACTION_TYPE_SEND_MESSAGE:
                handleState[index] = BLOCK;
                sendMessage(index, client, unpacked);
                break;
        }
        while (handleState[index] == BLOCK); //自旋等待
        free(unpacked);
    }
    _endthread();
}

int main(){

    //初始化
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
    for (int i = 0; i < LIST_SIZE; i++){
        clientList[i].state = STOP;
    }

    //创建套接字
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //绑定套接字
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
    sockAddr.sin_family = PF_INET;  //使用IPv4地址
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    sockAddr.sin_port = htons(2943);  //端口
    if (SOCKET_ERROR == bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR))){
        printf("端口绑定失败！\n");
        closesocket(servSock);
        WSACleanup();
        return 0;
    }
    SOCKADDR clientAddr;
    SOCKET clientSock;
    int nSize = sizeof(SOCKADDR);

    //进入监听状态
    listen(servSock, LIST_SIZE);
    printf("[ * ] 服务器已启动在127.0.0.1:2943 最大连接数 %d\n",LIST_SIZE);
    //接收客户端请求
    while (mainState){
        clientSock = accept(servSock, (SOCKADDR*)&clientAddr, &nSize);

        //大小端转化
        char tempPort[2];
        tempPort[1] = *((char*)&((*(SOCKADDR_IN*)&clientAddr).sin_port));
        tempPort[0] = *((char*)&((*(SOCKADDR_IN*)&clientAddr).sin_port)+1);
        u_short port = *(u_short*)&tempPort;

        printf("[请求] 收到来自%s:%d的连接请求 \n",
               inet_ntoa(*(struct in_addr*)&clientAddr.sa_data[2]),port);

        //建立连接并加入List 启动新的线程处理请求
        int i;
        for (i = 0; i < LIST_SIZE; i++){
            if (clientList[i].state == STOP) {
                clientList[i].ip = (*(SOCKADDR_IN*)&clientAddr).sin_addr;
                clientList[i].port = port;
                clientList[i].state = RUN;
                clientList[i].clientSock = clientSock;
                break;
            }
        }
        if (i == LIST_SIZE){
            int randomKill = rand() % LIST_SIZE;
            sprintf(messageBuffer,"连接已被终止\n");
            printf("[指示] 因连接槽全满，随机关闭并使用 %d 号作为新的连接\n",randomKill);
            packData(PACKAGE_TYPE_ORDER,ACTION_TYPE_CONNECT_FAIL,clientList[randomKill].clientSock);
            handleState[randomKill] = STOP;
            clientList[randomKill].ip = (*(SOCKADDR_IN*)&clientAddr).sin_addr;
            clientList[randomKill].port = port;
            clientList[randomKill].clientSock = clientSock;
            i = randomKill;
        }
        _beginthread(handleRequest,0,(void*)&i);
    }
    //关闭套接字
    closesocket(servSock);

    //终止 DLL 的使用
    WSACleanup();

    return 0;
}