#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
#include<sys/wait.h>
#include<pthread.h>

#define PORT 8080 

#define _BACK_LOG_ 10

int startup(const char *ip,unsigned short port)
{
	int listen_sock = socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock == -1)
	{
		printf("%d:%s\n",errno,strerror(errno));
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if((setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))) <0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if(bind(listen_sock,(struct sockaddr*)&local,sizeof(local))==-1)
	{
		printf("%d:%s\n",errno,strerror(errno));
		exit(1);
	}

	if(listen(listen_sock,_BACK_LOG_)==-1)
	{
		printf("%d:%s\n",errno,strerror(errno));
		exit(1);
	}
	return listen_sock;
}

static void wait_child(int signo)
{
	pid_t exit_id;
	while((exit_id = waitpid(-1,NULL,WNOHANG))>0)
	{
		printf("child exited,pid is : %d\n",exit_id);
	}
}

void *thread_run(void *arg)
{
	printf("create new thread,thread_id is : %u\n",pthread_self());
	int new_client = (int)arg;
	char buf[1024];
	while(1)
	{
		memset(buf,'\0',sizeof(buf));
		ssize_t _size = read(new_client,buf,sizeof(buf)-1);
		if(_size < 0)
		{
			perror("read");
			break;
		}
		else if(_size == 0)
		{
			printf("client release!\n");
			break;
		}
		else
		{
			buf[_size] = '\0';
			printf("client:->%s\n",buf);
		}

	}
	close(new_client);
}

void usage(char *proc)
{
	printf("Usage : %s [ip] [port]\n",proc);
}

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		usage(argv[3]);
		exit(1);
	}
	signal(SIGCHLD,wait_child);
	char *ip = argv[1];
	int port = atoi(argv[2]);
	struct sockaddr_in client_info;
	socklen_t client_len = sizeof(client_info);
	int listen_sock = startup(ip,port);

	int new_client = accept(listen_sock,(struct sockaddr*)&client_info,&client_len);
	while(1)
	{
		if(new_client < 0)
		{
			perror("accept");
			continue;
		}
		printf("get a new connect...\n");
#ifdef _FUN1_
		char buf[1024];
		printf("usage fun1...\n");
		while(1)
		{
			memset(buf,'\0',sizeof(buf));
			ssize_t _size = read(new_client,buf,sizeof(buf));
			if(_size < 0)
			{
				perror("read");
				break;
			}
			else
			{
				if(!strncasecmp(buf,"quit",4))
				{
					printf("client release!\n");
					close(listen_sock);
					continue;
				}
				buf[_size] = '\0';
				write(1,buf,sizeof(buf));
				int i = 0;
				for(;i<sizeof(buf);i++)
					buf[i] = toupper(buf[i]);
				write(new_client,buf,sizeof(buf));
			}
		}
		close(listen_sock);
#endif
	}
	return 0;
}
