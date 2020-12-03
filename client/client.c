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

//����״̬
enum bool{false, true} connectState=false;
enum state{STOP, RUN, BLOCK};
volatile enum state messageState,receiverState,mainState;
volatile int id;
//�������ݰ���ʽ��ʹ����������
struct data{
    char time[20];
    int targetID;
    int contentLength;
    char packageType;
    char actionType;
    char body[];
}model;

//��ز���
char targetIp[17];
u_short targetPort;
SOCKET serverSock;
struct sockaddr_in sockAddr;
char responseBuffer[MAXWORD] = {0};
char messageBuffer[MAXWORD] = {0};
char requestBuffer[MAXWORD] = {0};
const char* menu = "[1] ���ӷ�����\n[2] �Ͽ�����������\n[3] ��ȡʱ��\n[4] ��ȡ����\n"
                   "[5] ��ȡ�ͻ����б�\n[6] ������Ϣ\n[7] �˳�����\n\n";

//volatile int tempCounter = -1;

void packData(char type,char action){
    memset(requestBuffer,0,sizeof(requestBuffer));
    //�������ݰ�ͷ����Ϣ ����ʱ�䡢���ݰ�������ݰ��������������С��ͨ�ſͻ��˱��
    SYSTEMTIME sys;
    GetLocalTime(&sys);
    sprintf(requestBuffer,"%4d/%02d/%02d %02d:%02d:%02d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond);
    int length = (int)strlen(messageBuffer);
    strcpy(&requestBuffer[CONTENT_LENGTH_BIT], (char*)&length);
    strcpy(&requestBuffer[SEND_MSG_TARGET_BIT], (char*)&id);
    requestBuffer[PACKAGE_TYPE_BIT] = type;
    requestBuffer[ACTION_TYPE_BIT] = action;
    //���������
    strcpy(&requestBuffer[CONTENT_BIT], messageBuffer);
    send(serverSock, requestBuffer, CONTENT_BIT + length + sizeof(char), 0);
    memset(requestBuffer,0,sizeof(requestBuffer));
}

struct data* unpackData(){
    //����������С
    int contentSize = *(int*)&responseBuffer[CONTENT_LENGTH_BIT];
    //Ϊ������������ڴ�
    struct data* unpacked = (struct data*)malloc(sizeof(model)+contentSize+1);
    memset((void*)unpacked,0,sizeof(model)+contentSize+1);
    //�������ݰ�ͷ��
    memcpy((void*)unpacked,(void*)responseBuffer,sizeof(model));
    //�������ݰ���
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
        while(messageState == BLOCK); //�����ȴ�
    }
    _endthread();
}

void closeSocket(){
    if (connectState){
        messageBuffer[0] = '\0';
        packData(PACKAGE_TYPE_GET,ACTION_TYPE_CLOSE);
        connectState = false;
        receiverState = STOP;
        printf("��ǰ�����ѱ���ֹ����лʹ�á�\n");
        closesocket(serverSock);
    }
}

void message(){
    messageState = RUN;
    while (messageState != STOP){
        if (messageState == BLOCK){
            struct data* unpacked = unpackData();
            //�Զ��崦��ʽ
            switch (unpacked->packageType) {
                case PACKAGE_TYPE_RESPONSE://������Ӧ��
                    if (unpacked->actionType == ACTION_TYPE_TIME){
                        printf("��������ǰʱ��Ϊ : %s",unpacked->time);
                    } else if ( unpacked->actionType == ACTION_TYPE_CONNECT_SUCCESS){
                        connectState = true;
                        printf("%s",unpacked->body);
                    } else {
                        printf("%s",unpacked->body);
                    }
                    break;
                case PACKAGE_TYPE_ORDER://����ָʾ��
                    if (unpacked->actionType == ACTION_TYPE_CONNECT_FAIL){
                        closeSocket();
                        printf("\n[δ����] ������������\n");
                    } else if (unpacked->actionType == ACTION_TYPE_SEND_MESSAGE){
                        printf("[�յ������ͻ��˵���Ϣ]\n�Է��ͻ��˱�ţ�%d\n����ʱ�䣺%s",unpacked->targetID,unpacked->time);
                        printf("%s",unpacked->body);
                        printf("\n[������] ������������\n");
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
        //�ر��׽���
        closeSocket();
    }
    //�����׽���
    serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
    sockAddr.sin_family = PF_INET; //IPv4
    printf("��������Ҫ���ӵ�Ŀ��IP��˿� (Ŀǰֻ������ 127.0.0.1 2943)\n");
    scanf("%s%hu", targetIp, &targetPort);
    sockAddr.sin_addr.s_addr = inet_addr(targetIp);
    sockAddr.sin_port = htons(targetPort);
    if (connect(serverSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) == -1){
        printf("����ʧ�ܣ����������Ƿ�����\n");
        while (mainState == BLOCK) mainState = RUN;
    } else {
        _beginthread(receiver,0,NULL);
    }
}

void sendMessage(){
    fflush(stdin);
    printf("�����뷢�͸�[%d]�ſͻ��˵���Ϣ����Ҫ����ʱ���������λس���\n",id);
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
    //��ʼ��DLL
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //��ӡ�˵�
    printf("%s", menu);

    //����Ϣ�����̵߳ķ�ʽʵ���̼߳�ͨ��
    _beginthread(message,0,NULL);
    //ѭ�������û�����
    mainState = RUN;
    while (mainState != STOP){
        int op;
        //��ʾ����״̬
        if (connectState){
            printf("\n[������] ������������\n");
        } else {
            printf("\n[δ����] ������������\n");
        }
        //�û������ж�
        fflush(stdin);
        scanf("%d",&op);
        if (!connectState && op >= 3 && op <= 6){
            printf("�������ӷ�������\n");
            continue;
        }
        switch (op) {
            case 1: //����
                mainState = BLOCK;
                connectServer();
                break;
            case 2: //�Ͽ�����
                closeSocket();
                break;
            case 3: //��ȡʱ��
                mainState = BLOCK;
//                for (int i = 0 ; i <100 ; i++){
                    messageBuffer[0] = '\0';
                    packData(PACKAGE_TYPE_GET,ACTION_TYPE_TIME);
//                }
//                Sleep(1000);
//                printf("���յ���%d��ʱ����Ӧ\n",tempCounter);
//                tempCounter = 0;
                break;
            case 4: //��ȡ����
                mainState = BLOCK;
                messageBuffer[0] = '\0';
                packData(PACKAGE_TYPE_GET,ACTION_TYPE_NAME);
                break;
            case 5: //��ȡ�ͻ����б�
                mainState = BLOCK;
                messageBuffer[0] = '\0';
                packData(PACKAGE_TYPE_GET,ACTION_TYPE_CLIENT_LIST);
                break;
            case 6: //������Ϣ
                mainState = BLOCK;
                printf("������ͨѶĿ���ID��� (ʹ��[5]�Ź��ܽ��в�ѯ)\n");
                scanf("%d",&id);
                sendMessage();
                break;
            case 7: //�˳�
                mainState = STOP;
                closeSocket();
                break;
            default:
                printf("���������Ч\n");
                break;
        }
        while(mainState == BLOCK);//�����ȴ�
    }
    //֪ͨ���ȴ����߳̽���
    while(messageState!=STOP || receiverState!=STOP){
        messageState = STOP;
        receiverState = STOP;
    }
    //��ֹʹ�� DLL
    closesocket(serverSock);
    WSACleanup();

    system("pause");
    return 0;
}



