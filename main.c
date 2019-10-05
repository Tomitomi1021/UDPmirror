#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>

#include"datablock.h"

/*
	UDPmirror
		特定ポート宛のUDPパケットを特定アドレスの特定ポートに転送するプログラム。
	書式
		UDPmirror -s 待機ポート -d 転送先アドレス 転送先ポート
*/

struct ARGS{
	int sport;
	int dport;
	char* daddr;
};

struct ARGS readArgs(int argc,char* argv[]){
	struct ARGS args;
	args.sport=1234;
	args.dport=1235;
	args.daddr="127.0.0.1";

	for(int i=1;i<argc;){
		if(!strcmp(argv[i],"-s")){
			args.sport=atoi(argv[i+1]);
			i+=1;
		}else if(!strcmp(argv[i],"-d")){
			args.dport=atoi(argv[i+2]);
			args.daddr=argv[i+1];
			i+=3;
		}else{
			i+=1;
		}
	}
	return args;
}

int recvSocket_open(int port){
	struct sockaddr_in addr;
	int recvSocket;
	int res;

	res = socket(AF_INET,SOCK_DGRAM,0);
	if(res == -1){
		return -1;
	}else{
		recvSocket = res;
	}
	
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	res=bind(recvSocket,(struct sockaddr*)&addr,sizeof(addr));
	if(res == -1){
		return -2;
	}

	return recvSocket;
}

DataBlock* recvSocket_recv(int sock){
	int size;
	DataBlock* mes;
	mes=DataBlock_new(1024);
	size = recv(sock,mes->data,mes->size,0);
	if(size<0){
		return 0;
	}
	mes->size=size;
	return mes;
}

int sendSocket_open(){
	struct sockaddr_in addr;
	int sendSocket;
	int res;

	res = socket(AF_INET,SOCK_DGRAM,0);
	if(res==-1){
		return -1;
	}else{
		sendSocket = res;
	}
	return sendSocket;
}

int sendSocket_send(int sock,char* daddr,int dport,DataBlock* mes){
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(dport);
	addr.sin_addr.s_addr = inet_addr(daddr);

	sendto(sock,mes->data,mes->size,0,(struct sockaddr*)&addr,sizeof(addr));
}

int main(int argc,char* argv[]){
	struct ARGS args=readArgs(argc,argv);
	int recvSocket,sendSocket;
	int res;
	DataBlock* mes; 

	printf("UDP mirror\n");
	printf("localhost:%d -> %s:%d\n",
			args.sport,args.daddr,args.dport);
	
	printf("recvSocket opening...");
	res = recvSocket_open(args.sport);
	if(res<0){
		char* errmes=strerror(errno);
		printf("ERROR:%s",errmes);
		return -1;
	}else{
		printf("success\n");
		recvSocket = res;
	}

	printf("sendSocket opening...");
	res = sendSocket_open();
	if(res<0){
		char* errmes=strerror(errno);
		printf("ERROR:%s",errmes);
		return -1;
	}else{
		printf("SUCCESS\n");
		sendSocket = res;
	}

	printf("listening on %d ...\n",args.sport);
	while(1){
		mes = recvSocket_recv(recvSocket);
		if(mes==0){
			char* errmes=strerror(errno);
			printf("ERROR:%s",errmes);
			return -1;
		}
		printf("MessageReceived(%dbyte)\n",mes->size);
		res = sendSocket_send(sendSocket,args.daddr,args.dport,mes);
		DataBlock_del(mes);
		printf("mirroring done\n");
	}
	return 0;
}
