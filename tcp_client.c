#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define _SERVER_PORT_ 8080
#define _SERVER_IP_ "192.168.136.129"

static void usage(const char *proc)
{
	printf("Usage:%s   [ip]\n",proc);
}

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		usage(argv[2]);
		exit(1);
	}
	const char *ip = argv[1];
	int sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock == -1)
	{
		printf("%d:%s\n",errno,strerror(errno));
		exit(1);
	}

	unsigned short port = _SERVER_PORT_;
	struct sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	inet_pton(AF_INET,_SERVER_IP_,&remote.sin_addr);

	int ret  = connect(sock,(struct sockaddr*)&remote,sizeof(remote));

	if(ret < 0)
	{
		perror("connect");
		exit(1);
	}
	printf("connect success...\n");
	
	char buf[1024];
	while(1)
	{
		memset(buf,'\0',sizeof(buf));
		read(0,buf,sizeof(buf));
		buf[strlen(buf)] = '\0';
		if(strncasecmp(buf,"quit",4) == 0)
		{
			write(sock,"quit",4);
			printf("quit!\n");
			break;
		}

		write(sock,buf,sizeof(buf));
		int ret = read(sock,buf,sizeof(buf));
		write(1,buf,ret);
	}
	close(sock);
	return 0;
}

