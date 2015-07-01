#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dlfcn.h>
#include "test.h"

#define PROCESSNUM 5

void ChildProcess(int sockfd);
void (* pfunc)();

int main()
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd <= 0) {
		printf("sockfd error, err: %s\n", strerror(errno));
	}

	int iREUSEADDR = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &iREUSEADDR, sizeof(iREUSEADDR));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(22222);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = bind(sockfd, (sockaddr *)&addr, sizeof(addr));
	if (ret == -1) {
		printf("bind error, err: %s\n", strerror(errno));
	}

	ret = listen(sockfd, 32);
	if (ret == -1) {
		printf("listen error, err: %s\n", strerror(errno));
	}

	int childs = 0;

	while (1) 
	{
		if (childs < PROCESSNUM) 
		{
			childs++;
			pid_t pid;
			pid = fork();
			if (pid == 0) {
				printf("child pro, pid: %d, pare: %d\n", getpid(), getppid());
				ChildProcess(sockfd);
			}
			else {
				printf("parent pro, pid: %d, pare: %d, child: %d\n", getpid(), getppid(), pid);
			}
		}
		else {
			pid_t pid;
			int status;
			if ((pid = waitpid(-1, &status, WUNTRACED)) != -1)                                                                                                                                                     
			{
				printf("sub-process %d exit %d.\n", pid, status);                                                                                                                                              
				--childs;

				struct timespec ts;            
				ts.tv_sec = 0;
				ts.tv_nsec = 100 * 1000 * 1000;
				nanosleep(&ts, NULL);    
			}
			else
			{
			}
		}
		printf("over pid: %d\n", getpid());
	}
}

void ChildProcess(int sockfd)
{
	int ret = 0;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr);
	char buf[1024] = {0};
	int client_fd;
	int len = 0;

	while (1) {
		len = 0;
		client_fd = accept(sockfd, (struct sockaddr *)&addr, &addrlen); 
		if (client_fd == -1) {
			printf("accept error, err: %s\n", strerror(errno));
		}
		else {
			void *handle = dlopen("xxxx/test.so", RTLD_LAZY);
			if (handle) {
				printf("dlopen succ\n");
			}

			char *error;
			dlerror();

			pfunc = (void (*)())dlsym(handle, "func");
			//pfunc p = (pfunc)dlsym(handle, "func");
			if ((error = dlerror()) != NULL)
			{
				printf("dlsym failed, err: %s\n", error);
				break;
			}
			else {
				printf("dlsym succ\n");
				pfunc();
			}

			printf("accept succ, ip: %s:%d, pid: %d\n", inet_ntoa(addr.sin_addr), addr.sin_port, getpid());
			int i = 0;
			while (1) {
				memset(buf, 0 ,1024);
				ret = recv(client_fd, buf, 4096, 0);
				if (ret == -1) {
					printf("recv error, err: %s\n", strerror(errno));
				}
				else if (ret == 0) {
					printf("recv data over, len: %d, quit\n", len);
					break;
				}
				else {
					len += ret;
				//	printf("recv data: %s, len: %d, %d\n", buf, ret, ++i);
				}
			}
			close(client_fd);
		}
	}
}
