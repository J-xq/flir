/*
 * ============================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/01/2018 07:32:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Fang Yuan (yfang@nju.edu.cn)
 *   Organization:  nju
 *
 * ============================================================================
 */

#include "serial_flir.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT         9999
#define MAXLINE      4096

int network_init()
{
	int listenfd,fd;
	struct sockaddr_in addr,cliaddr;
	int sin_size;
	int on = 1;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	//
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(listenfd,(struct sockaddr *)&addr, sizeof(addr));
	listen(listenfd, 2);
	sin_size = sizeof(cliaddr);
	fd = accept(listenfd,(struct sockaddr *)&cliaddr, &sin_size);
	return fd;
}


/* snapshot camera, send to client */
void timer_handler(int sig_num)
{

}

/* 
* ===  FUNCTION  =============================================================
*         Name:  main
*  Description:  
* ============================================================================
*/

int main(int argc, char *argv[])
{
	int connfd;
	char line[MAXLINE];
	int n;
    struct itimerval ival;
    struct sigaction act;

    act.sa_handler = timer_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGALRM, &act, NULL);

    ival.it_interval.tv_sec = 3;
    ival.it_interval.tv_usec = 0;
    ival.it_value.tv_sec = 3;
    ival.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &ival, NULL);

	connfd = network_init();
	for(;;) {
		read(connfd, line, 1);
		write(connfd, line, MAXLINE);
	}

	return 0;
}

