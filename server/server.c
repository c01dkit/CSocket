#include <stdio.h>
#include <winsock2.h>
#include <process.h>

//��ز���
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

//�������ݰ���ʽ��ʹ����������
struct data{
    char time[20];
    int sourceID;
    int contentLength;
    char packageType;
    char actionType;
    char body[];
}model;

//��¼�ͻ�������״̬
struct connection{
    u_short port;
    struct in_addr	ip;
    SOCKET  clientSock;
    int     state;
}clientList[LIST_SIZE];

void packData(char type, char action, SOCKET targetSock){
    memset(responseBuffer,0,sizeof(responseBuffer));
    //�������ݰ�ͷ����Ϣ ����ʱ�䡢���ݰ�������ݰ��������������С
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    sprintf(responseBuffer,"%4d/%02d/%02d %02d:%02d:%02d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond);
    int length = (int)strlen(messageBuffer);
    strcpy(&responseBuffer[CONTENT_LENGTH_BIT], (char*)&length);
    strcpy(&responseBuffer[SEND_MSG_SOURCE_BIT], (char*)&id);
    responseBuffer[PACKAGE_TYPE_BIT] = type;
    responseBuffer[ACTION_TYPE_BIT] = action;
    //���������
    strcpy(&responseBuffer[CONTENT_BIT], messageBuffer);
    send(targetSock, responseBuffer, CONTENT_BIT + length + sizeof(char), 0);
    memset(responseBuffer,0,sizeof(responseBuffer));
}

/** �˺������ڽ��������
 * �������Ҫͬʱ���������� ��Ҫ�ڶ���Ϊ���ǿ��ٿռ�
 * ʹ��ȫ�ֻ������ᵼ�����ݳ�ͻ ��Ȼ����Ϊÿ�����̵߳������建����
 * �����ǵ������ֹ�����ʱ������ָ߲������� ����ѡ���˵��������Խ�ʡ�����ռ�
 */
struct data* unpackData(){
    //����������С
    int contentSize = *(int*)&requestBuffer[CONTENT_LENGTH_BIT];
    //Ϊ������������ڴ�
    struct data* unpacked = (struct data*)malloc(sizeof(model)+contentSize+1);
    //�������ݰ�ͷ��
    memcpy((void*)unpacked,(void*)requestBuffer,sizeof(model));
    //�������ݰ���
    memcpy(unpacked->body, &requestBuffer[CONTENT_BIT], contentSize);
    memset(requestBuffer,0,sizeof(requestBuffer));
    return unpacked;
}
void clientListResponse(int index, SOCKET client){
    int count = 0; //���ڼ���
    char tempBuffer[MAXBYTE] = {0}; //������װ������Ϣ
    memset(messageBuffer,0,MAXWORD);
    for (int check = 0; check < LIST_SIZE; check++){
        if (clientList[check].state){
            count++;
            sprintf(tempBuffer,"[%d] �ͻ��� -- ����%s:%d\n",check,
                    inet_ntoa(clientList[check].ip),clientList[check].port);
            strcat(messageBuffer,tempBuffer);
        }
    }
    sprintf(tempBuffer,"����%d���ͻ������������������е�ǰ�ͻ��˱��Ϊ %d \n",count,index);
    strcat(messageBuffer,tempBuffer);
    packData(PACKAGE_TYPE_RESPONSE,ACTION_TYPE_CLIENT_LIST,client);
    while (handleState[index] == BLOCK) handleState[index] = RUN;
}

void sendMessage(int index, SOCKET client, struct data* unpacked){
    int target = unpacked->sourceID;
    printf("[����] %d ��������Ϣ�� %d �ſͻ���\n",index,target);
    memset(messageBuffer,0,MAXWORD);
    if (target < 0 ||target >= LIST_SIZE){
        sprintf(messageBuffer,"����ʧ�ܣ���Ч��Ŀ��ͻ��˱��\n");
        printf(" ����ʧ�ܣ���Ч��Ŀ��ͻ��˱��\n");
    } else if (clientList[target].state == STOP){
        sprintf(messageBuffer,"����ʧ�ܣ�Ŀ��ͻ���δ������������\n");
        printf( " ����ʧ�ܣ�Ŀ��ͻ���δ������������\n");
    } else {
        strcpy(messageBuffer,unpacked->body); //���ת��������
        id = index; //�������ݷ��ͷ�Ϊ��ǰclient�ı��
        printf("[ָʾ] �� %d �ſͻ��˷�����Ϣ��\n%s",target,unpacked->body);
        packData(PACKAGE_TYPE_ORDER,ACTION_TYPE_SEND_MESSAGE,clientList[target].clientSock);
        sprintf(messageBuffer,"���ͳɹ���\n");
        printf( "[ * ] ���ͳɹ�\n");
    }
    packData(PACKAGE_TYPE_RESPONSE,ACTION_TYPE_SEND_MESSAGE,client);
    while(handleState[index] == BLOCK) handleState[index] = RUN;
}

void handleRequest(void* i){
    int index = *(int*)i;
    SOCKET client = clientList[index].clientSock;
    printf("[ * ] ��ͬ�ͻ��˽����µ����ӣ�ʹ�����Ӳ۱��Ϊ %d \n",index);

    handleState[index] = RUN;
    sprintf(messageBuffer,"[HELLO] �����������������˳�ʱ��ʹ������[2]������[7]\n");
    packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_CONNECT_SUCCESS, client);
    while (handleState[index]){
        int a = recv(client, requestBuffer, MAXWORD, 0);
        if(a < 0) break;
        if (handleState[index] == STOP) break;
        struct data* unpacked = unpackData();
        //�������յ��İ�һ�����������������ֻ��⶯������
        switch (unpacked->actionType) {
            case ACTION_TYPE_TIME:
                messageBuffer[0] = '\0';
                printf("[��Ӧ] ��Ӧ���� %d �ſͻ��˵�ʱ������\n",index);
                packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_TIME, client);
                break;
            case ACTION_TYPE_NAME:
                sprintf(messageBuffer,"����������Ϊ : c01dkit\n");
                printf("[��Ӧ] ��Ӧ���� %d �ſͻ��˵���������\n",index);
                packData(PACKAGE_TYPE_RESPONSE, ACTION_TYPE_NAME, client);
                break;
            case ACTION_TYPE_CLOSE:
                handleState[index] = STOP;
                clientList[index].state = STOP;
                printf("[����] �ر� %d �ſͻ��˵�����\n",index);
                break;
            case ACTION_TYPE_CLIENT_LIST:
                handleState[index] = BLOCK;
                printf("[��Ӧ] ��Ӧ���� %d �ſͻ��˵��б�����\n",index);
                clientListResponse(index, client);
                break;
            case ACTION_TYPE_SEND_MESSAGE:
                handleState[index] = BLOCK;
                sendMessage(index, client, unpacked);
                break;
        }
        while (handleState[index] == BLOCK); //�����ȴ�
        free(unpacked);
    }
    _endthread();
}

int main(){

    //��ʼ��
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
    for (int i = 0; i < LIST_SIZE; i++){
        clientList[i].state = STOP;
    }

    //�����׽���
    servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    //���׽���
    struct sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
    sockAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ
    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
    sockAddr.sin_port = htons(2943);  //�˿�
    if (SOCKET_ERROR == bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR))){
        printf("�˿ڰ�ʧ�ܣ�\n");
        closesocket(servSock);
        WSACleanup();
        return 0;
    }
    SOCKADDR clientAddr;
    SOCKET clientSock;
    int nSize = sizeof(SOCKADDR);

    //�������״̬
    listen(servSock, LIST_SIZE);
    printf("[ * ] ��������������127.0.0.1:2943 ��������� %d\n",LIST_SIZE);
    //���տͻ�������
    while (mainState){
        clientSock = accept(servSock, (SOCKADDR*)&clientAddr, &nSize);

        //��С��ת��
        char tempPort[2];
        tempPort[1] = *((char*)&((*(SOCKADDR_IN*)&clientAddr).sin_port));
        tempPort[0] = *((char*)&((*(SOCKADDR_IN*)&clientAddr).sin_port)+1);
        u_short port = *(u_short*)&tempPort;

        printf("[����] �յ�����%s:%d���������� \n",
               inet_ntoa(*(struct in_addr*)&clientAddr.sa_data[2]),port);

        //�������Ӳ�����List �����µ��̴߳�������
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
            sprintf(messageBuffer,"�����ѱ���ֹ\n");
            printf("[ָʾ] �����Ӳ�ȫ��������رղ�ʹ�� %d ����Ϊ�µ�����\n",randomKill);
            packData(PACKAGE_TYPE_ORDER,ACTION_TYPE_CONNECT_FAIL,clientList[randomKill].clientSock);
            handleState[randomKill] = STOP;
            clientList[randomKill].ip = (*(SOCKADDR_IN*)&clientAddr).sin_addr;
            clientList[randomKill].port = port;
            clientList[randomKill].clientSock = clientSock;
            i = randomKill;
        }
        _beginthread(handleRequest,0,(void*)&i);
    }
    //�ر��׽���
    closesocket(servSock);

    //��ֹ DLL ��ʹ��
    WSACleanup();

    return 0;
}