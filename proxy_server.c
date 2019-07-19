#include "proxy_server.h"

//////////////////////////////////////////////////////////////////////////
//  sha1_hash								//
// =================================================================	//
// Input: char* -> input_url,						//
// Output: char* -> hashed_url 						//
// Purpose: change url using sha1 hash 					//
//////////////////////////////////////////////////////////////////////////
char *sha1_hash(char *input_url, char *hashed_url) {
	unsigned char hashed_160bits[20];
	char hashed_hex[41];
	int i;

	SHA1((unsigned char*)input_url, strlen(input_url), hashed_160bits);

	for(i=0; i<sizeof(hashed_160bits); i++)
		sprintf(hashed_hex + i*2, "%02x", hashed_160bits[i]);

	strcpy(hashed_url, hashed_hex);

	return hashed_url;
}

//////////////////////////////////////////////////////////////////////////
//  getHomeDir								//
// =================================================================	//
// Input: none								//
// Output: char* -> path of home directory				//
// Purpose: get home directory						//
//////////////////////////////////////////////////////////////////////////
char *getHomeDir(char* home){
	struct passwd *usr_info = getpwuid(getuid());
	strcpy(home, usr_info->pw_dir);

	return home;
}

//////////////////////////////////////////////////////////////////////////
//  fileExist								//
// =================================================================	//
// Input: char* -> path of directory I want to check			//
//	  	  char* -> name of file I want to find			// 
// Output: int - 1 file Exist						//
//  		 0 not Exist						//
// Purpose: check whether file exists or not				//
//////////////////////////////////////////////////////////////////////////
int fileExist(char* dir_path, char* file_name){
	struct dirent *pFile;
	DIR *pDir;
	
	pDir = opendir(dir_path);

	if(pDir == NULL) 
		return 0;
	
	for(pFile=readdir(pDir); pFile; pFile=readdir(pDir)){
		if(strcmp(file_name, pFile->d_name)==0){
			closedir(pDir);
			return 1;
		}
	}
	closedir(pDir);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//  append_path								//
// =================================================================	//
// Input: char* -> source path which is appended by dst			//
//	  char* -> path which is append to src				// 
// Output: char* -> new file_name					//
// Purpose: append dst to source					//
//////////////////////////////////////////////////////////////////////////
char* append_path(char *src, char *dst, char *file_name){
	strcpy(dst, src); 
	strcat(dst, "/");
	strcat(dst, file_name);
	
	return file_name;
}

//////////////////////////////////////////////////////////////////////////
//  checkHit								//
// =================================================================	//
// Input: char* -> root path which is ~/cache/				//
//	  char* -> dir_path which is ~/cache/a23/			//
//	  char* -> front 3 letters of hashed_url			//
//	  char* -> hashed_url except 3 letters				//
// Output: int - 1 HIT							//
//	       - 0 MISS							//			
// Purpose: check whether HIT or MISS					//
//////////////////////////////////////////////////////////////////////////
int checkHit(char* root_path, char* dir_path, char* dir_name, char* file_name){
	if(fileExist(root_path, dir_name) == 1){ // dir exist
		if(fileExist(dir_path, file_name) == 1) 
			return 1; // dir and file exist == HIT;
	}

	else return 0; // only dir exist == MISS;
}

//////////////////////////////////////////////////////////////////////////
//  get_time 								//
// =================================================================	//
// Input: char* -> get string of time info in this allocation		//
//	 								//
// Output: char* - return string of time				//			
// Purpose: make string of time info					//
//////////////////////////////////////////////////////////////////////////
char* get_time(char* str_time){
	time_t now;
	struct tm *ltp;
	char str[100];

	time(&now);
	ltp = localtime(&now);

	sprintf(str_time, "%d/%d/%d, %d:%d:%d", ltp->tm_year+1900, ltp->tm_mon+1, ltp->tm_mday, 
				  ltp->tm_hour, ltp->tm_min, ltp->tm_sec);

	return str_time;
}

//////////////////////////////////////////////////////////////////////////
//  get_log_path 							//
// =================================================================	//
// Input: char* -> get path of logfile.txt in this allocation		//
//	 								//
// Output: char* - return string of path of logfile.txt			//			
// Purpose: make string of path of logfile.txt				//
//////////////////////////////////////////////////////////////////////////
char* get_log_path(char* log_path){
	char home_dir[100];
	char buf[100];

	getHomeDir(home_dir);
	append_path(home_dir, buf, "logfile");
	mkdir(buf, 0777);
	append_path(buf, log_path, "logfile.txt"); // root_path = ~/logfile/logfile.txt
}


//////////////////////////////////////////////////////////////////////////
// void *thr_fn								//
// =================================================================	//
// Input: char* -> file_path, input_url, HorM				//			//
//																//
// Purpose: wrtie log in logfile.txt					//
//////////////////////////////////////////////////////////////////////////
void *thr_fn(void* buf){
	printf("*PID# %d create the *TID# %lu.\n", getpid(), pthread_self());

	char home_dir[BUFFSIZE]; // ~
	char log_path[BUFFSIZE]; // ~/logfile/logfile.txt
	char content[BUFFSIZE]; 
	char str_time[100];
	char *dir_file;
	char file_path[BUFFSIZE];
	char input_url[BUFFSIZE];
	char HorM[BUFFSIZE];
	FILE* fp;

	char *tok;
	tok = strtok(buf, " ");
	strcpy(file_path, tok);
	tok = strtok(NULL, " ");
	strcpy(input_url, tok);
	tok = strtok(NULL, " ");
	strcpy(HorM, tok);

	dir_file = &file_path[12];
	get_log_path(log_path);

	fp = fopen(log_path, "a");

	if(strcmp(HorM, "HIT") == 0){ //Hit
		sprintf(content, "[Hit] ServerPID : %d | %s - [%s]\n", getpid(), dir_file, get_time(str_time));
		fwrite(content, 1, strlen(content), fp);
		//[Hit]dir_name/file_name - [Time]
		sprintf(content, "[Hit] %s\n", input_url);
		fwrite(content, 1, strlen(content), fp);
		//[Hit]URL_name
		if(fp != NULL) fclose(fp);
	}

	else if(strcmp(HorM, "MISS") == 0) {// MISS
		sprintf(content, "[Miss] ServerPID : %d | %s - [%s]\n", getpid(), input_url, get_time(str_time));
		//[Miss] URL - [Time]
		fwrite(content, 1, strlen(content), fp);
		if(fp != NULL) fclose(fp);
	}
	printf("*TID# %lu is exited.\n", pthread_self());
}

//////////////////////////////////////////////////////////////////////////
// writeLog								//
// =================================================================	//
// Input: char* -> path of file in cache dir				//
// 	  char* -> input url						//
// 	  char* -> string of "HIT" or "MISS"				//
// Output: int - 1 write succeed					//
//	       - 0 write failed 					//			
// Purpose: wrtie log in logfile.txt					//
//////////////////////////////////////////////////////////////////////////
int writeLog(char* file_path, char*input_url, char* HorM){
	sem_t *mysem;
	pthread_t tid;
	char buf[BUFFSIZE];
	int err;
	void *tret;
	
	strcat(buf, file_path);
	strcat(buf, " ");
	strcat(buf, input_url);
	strcat(buf, " ");
	strcat(buf, HorM);
	// 인자들을 buf에 " "로 구분하여 저장

	if((mysem = sem_open("mysem", O_CREAT, 0777, 1)) == NULL) { // Open semaphore
		perror("Sem Open Error");
		return 1;
	}
	
	printf("*PID# %d is waiting for the semaphore.\n", getpid());
	sem_wait(mysem); // p function of semaphore

	printf("*PID# %d is in the critical zone.\n", getpid());

	err = pthread_create(&tid, NULL, thr_fn, (void*)buf); // thread 생성, thr_fn에서 log에 접근하는 일을 수행

	if( err != 0 ){
		printf("pthread_create() error.\n");
		return 0;
	}

	pthread_join(tid, &tret); // thread 정보를 회수

	sleep(1);

	sem_post(mysem); // v function of semaphore
	printf("*PID# %d exited the critical zone.\n", getpid());	
}

//////////////////////////////////////////////////////////////////////////
// write_Terminated_Server						//
// =================================================================	//
// Input: double -> execution time of the program			//
// 	  int -> number of created child process 			//
// 									//
// Purpose: write Server_Terminate information in logfile.txt		//
//////////////////////////////////////////////////////////////////////////
void write_Terminated_Server(double time_spent, int child_cnt) {
	char log_path[100];
	char content[100];
	FILE* fp;

	get_log_path(log_path);

	fp = fopen(log_path, "a");

	sprintf(content, "**SERVER** [Terminated] run time:%.0f sec. #sub process: %d\n", time_spent, child_cnt);
	fwrite(content, 1, strlen(content), fp);
	fclose(fp);
}

//////////////////////////////////////////////////////////////////////////
// proxy							//
// =================================================================	//
// Purpose: this is a function for Sub process				//
//	    (SHA-1 function (input_url to hashed_url))			//
//	    (Check: HIT or MISS)								//
//	    (HIT => 1. read response message from cache			//
//				2. send it to client)						//
//		(MISS => 1. write request message to server			//
//				 2. read response message from server		//
//				 3. make cache file							//
//				 4. send message to client)					//
//		(Manipulate cache directory)										//
//////////////////////////////////////////////////////////////////////////
int proxy(char* input_url, char* req_msg, int client_fd, int web_socket_fd) {
	char hashed_url[100];
	char cache_path[100];
	char dir_path[100];
	char file_path[100];
	char dir_name[4]; // front 3 letters of hashed_url
	char *file_name; 
	double time_spent;
	char buf[BUFFSIZE];
	char *tok;
	
	char response_message[BUFFSIZE] = {0, };
	char res_msg_server[BUFFSIZE] = {0, };
	
	pid_t pid;
	int cache_fd;
	int len;

	getHomeDir(cache_path); // dir = ~
	strcat(cache_path, "/cache"); // dir = ~/cache
	// cache_path : path of cache directory

	pid = getpid();

	sha1_hash(input_url, hashed_url);

	strncpy(dir_name, hashed_url, 3);
	dir_name[3] = '\0'; 
	file_name = &hashed_url[3];  
	// dir_name : hashed directory name
	// file_name : hashed file name

	umask(000);
	mkdir(cache_path, 0777); // root_path == ~/cache/

	append_path(cache_path, dir_path, dir_name);
	append_path(dir_path, file_path, file_name);

	if(checkHit(cache_path, dir_path, dir_name, file_name) == 1){
		writeLog(file_path, input_url, "HIT");

		cache_fd=open(file_path, O_RDWR, 0666); 

		while ((len = read(cache_fd, response_message, sizeof(response_message))) > 0){
			write(client_fd, response_message, len);
			bzero((char*)&response_message, sizeof(response_message));
			// send response to client
		}

		close(cache_fd);	
	}
	else {
		writeLog(file_path, input_url, "MISS");

		write(web_socket_fd, req_msg, strlen(req_msg)); // write request message to server

		alarm(10);
		while ((len = read(web_socket_fd, res_msg_server, sizeof(res_msg_server))) > 0){ // get response from server
			alarm(0); // alarm 종료
			mkdir(dir_path, 0777);
			cache_fd = open(file_path, O_CREAT | O_RDWR | O_APPEND, 0666); // make cache file
			write(cache_fd, res_msg_server, len); // store response in cache
			write(client_fd, res_msg_server, len); // send response to client

			bzero((char*)&res_msg_server, sizeof(res_msg_server));
			close(cache_fd);
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// getIPAddr															//
// =================================================================	//
// Input: char* -> Host address	from url								//
// Output: char* -> 32bit dotted decimal IP address						//		
//																		//
// Purpose: convert Host url to 32bit dotted decimal IP address			//
//////////////////////////////////////////////////////////////////////////
char *getIPAddr(char *addr)
{
	struct hostent* hent;
	int len = strlen(addr);
	char* haddr;

	if ( (hent = (struct hostent*)gethostbyname(addr)) != NULL)
	{	
		haddr=inet_ntoa(*(struct in_addr*)hent->h_addr_list[0]);
	}
	
	return haddr;
} 

//////////////////////////////////////////////////////////////////////////
// get_url_host															//
// =================================================================	//
// Input: char* -> request http message									//
//		  char* -> url from request is stored in this buffer			//
//		  char* -> Host from request is stored in this buffer			//			
//																		//
// Purpose: parsing url and host name from request message				//
//////////////////////////////////////////////////////////////////////////
char *get_url_host(char *req, char* url, char *host){
	char method[20] = {0, };
	char *tok = NULL;
	char buf[BUFFSIZE];

	tok = strtok(req , " "); // tok == "GET"
	strcpy(method, tok);
	if(strcmp(method, "GET") == 0){
		tok = strtok(NULL, " "); // tok = url
		strcpy(url, tok); 
		
		strcpy(buf, tok);
		tok = strtok(buf, "/");
		tok = strtok(NULL, "/");
		strcpy(host, tok);
	}

	return host;
}