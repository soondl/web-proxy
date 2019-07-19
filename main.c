//////////////////////////////////////////////////////////////////////////
// File Name : main.c						//
// Date : 2018/05/18							//
// Os : Ubuntu 16.04 LTS 64bits						//
// Author : Lee Soon Hyong						//
// Student ID : 2014722029						//
// -----------------------------------------------------------------	//
// Title : System Programming Assignment #2-3 (proxy server)		//
// Description : main function				//
//////////////////////////////////////////////////////////////////////////

#include "proxy_server.h"
time_t begin, end;
int child_cnt = 0;
int main_pid = 0;
int client_fd_all =0;

//////////////////////////////////////////////////////////////////////////
// SIGINT_handler							//
// =================================================================	//
// Purpose: When ctrl + c is called, write terminate log				//
//////////////////////////////////////////////////////////////////////////
static void SIGINT_handler()
{
	double time_spent;
	pid_t pid;

	pid = getpid();

	if(pid == main_pid){
	time(&end); // server end time
	time_spent = difftime(end, begin);
	write_Terminated_Server(time_spent, child_cnt);
	}
	//logfile에 Termination 정보 write
	exit(0);
}


//////////////////////////////////////////////////////////////////////////
// SIGCHILD_handler							//
// =================================================================	//
// Purpose: to collect child's termination process				//
//////////////////////////////////////////////////////////////////////////
static void SIGCHLD_handler()
{
    pid_t pid;
    int status;
    while( (pid = waitpid(-1, &status, WNOHANG)) > 0);
}

//////////////////////////////////////////////////////////////////////////
// SIGALRM_handler							//
// =================================================================	//
// Purpose: when proxy_server couldn't get response, terminate sub process//
//////////////////////////////////////////////////////////////////////////
static void SIGALRM_handler()
{
	char buf[BUFFSIZE];

	sprintf(buf, "%s", "not respond\n");
	printf("응답없음\n");
	write(client_fd_all, buf, strlen(buf));
    exit(0);
}

//////////////////////////////////////////////////////////////////////////
//  int main()											//
// =================================================================	//
// Purpose: work as a proxy server						//
//			get request message from client				//
//			send message 'Hit' or 'Miss' to client						//		
//////////////////////////////////////////////////////////////////////////
int main(){
    struct sockaddr_in server_addr, client_addr;
    int socket_fd, client_fd;
    int len;
    pid_t pid;
	double time_spent;
	struct in_addr addr;
	int opt;

	pid = getpid();
	main_pid = pid;
	time(&begin); // server start time
    if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        printf("Server: Can't open stream socket.");
        return 0;
	}

	opt = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// bind()에 의해 생기는 TIME_WAIT 현상을 막기 위함
	
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);
	// setting the server_addr

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Server: Can't bind local address.\n");
        return 0;
    }
	// 생성 된 socket에 서버 주소를 binding

    listen(socket_fd, 5);
    signal(SIGCHLD, (void *)SIGCHLD_handler); // 자식 프로세스 종료시, handler 함수 호출
	signal(SIGINT, (void *)SIGINT_handler); // ctrl+c 로 서버 종료시, handler 호출
	

    while(1){
        bzero((char*)&client_addr, sizeof(client_addr));
        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
		client_fd_all = client_fd;
		// wait for client connect

        if(client_fd < 0){
            printf("Server: accept failed.\n");
            return 0;
        }

		addr.s_addr = client_addr.sin_addr.s_addr;
       // printf("[%s : %d] client was connected.\n", inet_ntoa(addr), client_addr.sin_port);
        pid=fork(); // fork child process 

        if(pid == -1) {
            close(client_fd);
            close(socket_fd);
            continue;
        }

		child_cnt++; // 생성 된 child process의 수
        if(pid == 0){
			struct in_addr inet_client_address;
			struct sockaddr_in web_server_addr;
			int web_socket_fd;

			char buf[BUFFSIZE];
			char tmp[BUFFSIZE] = {0, };

			char url[BUFFSIZE];
			char host[BUFFSIZE];
			char *IPAddr;

			signal(SIGALRM, (void *)SIGALRM_handler);
			read(client_fd, buf, BUFFSIZE); 
			// read request message from client
			
			strcpy(tmp, buf);

			// puts("=========================================");
			// printf("Request from [%s : %d]\n", inet_ntoa(addr), client_addr.sin_port);
			// puts(buf);  
			// puts("===========================================\n");
			// // print request message to console

			get_url_host(tmp, url, host);
			// parsing host from request message

			IPAddr = getIPAddr(host);

			if((web_socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
      		 	printf("Server: Can't open stream socket.");
        		return 0;
			}
			// web server와 통신을 위한 sockets 생성

			bzero( (char*)&web_server_addr, sizeof(web_server_addr) );
			web_server_addr.sin_family = AF_INET;
			web_server_addr.sin_addr.s_addr = inet_addr(IPAddr);
			web_server_addr.sin_port = htons(80);
			// web_server_addr setting

			if( connect(web_socket_fd, (struct sockaddr*)&web_server_addr, sizeof(web_server_addr)) < 0 ){
				printf("can't connect.\n");
				return -1;
			}

			proxy(url, buf, client_fd, web_socket_fd);

		//	printf("[%s : %d] client was disconnected\n", inet_ntoa(addr)
		//			, client_addr.sin_port);
			
			close(client_fd);
			// sub process 와 client 연결 끊기
			close(web_socket_fd);
			return 0; // child process end
		}

		close(client_fd);
		// parent process 와 client 연결 끊기
	}
	close(socket_fd);
	// main preocess의 socket close
	return 0;
}