//////////////////////////////////////////////////////////////////////////
// File Name : proxy_server.h						//
// Date : 2018/05/18							//
// Os : Ubuntu 16.04 LTS 64bits						//
// Author : Lee Soon Hyong						//
// Student ID : 2014722029						//
// -----------------------------------------------------------------	//
// Title : System Programming Assignment #2-3 (proxy server)		//
// Description : proxy server				//
//////////////////////////////////////////////////////////////////////////
#include <stdio.h> // sprintf()
#include <string.h> // strcpy()
#include <openssl/sha.h> // SHA1()
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define BUFFSIZE	1024
#define PORTNO      38072

//////////////////////////////////////////////////////////////////////////
//  sha1_hash								//
// =================================================================	//
// Input: char* -> input_url,						//
// Output: char* -> hashed_url 						//
// Purpose: change url using sha1 hash 					//
//////////////////////////////////////////////////////////////////////////
char *sha1_hash(char *input_url, char *hashed_url);

//////////////////////////////////////////////////////////////////////////
//  getHomeDir								//
// =================================================================	//
// Input: none								//
// Output: char* -> path of home directory				//
// Purpose: get home directory						//
//////////////////////////////////////////////////////////////////////////
char *getHomeDir(char* home);

//////////////////////////////////////////////////////////////////////////
//  fileExist								//
// =================================================================	//
// Input: char* -> path of directory I want to check			//
//	  	  char* -> name of file I want to find			// 
// Output: int - 1 file Exist						//
//  		 0 not Exist						//
// Purpose: check whether file exists or not				//
//////////////////////////////////////////////////////////////////////////
int fileExist(char* dir_path, char* file_name);

//////////////////////////////////////////////////////////////////////////
//  append_path								//
// =================================================================	//
// Input: char* -> source path which is appended by dst			//
//	  char* -> path which is append to src				// 
// Output: char* -> new file_name					//
// Purpose: append dst to source					//
//////////////////////////////////////////////////////////////////////////
char* append_path(char *src, char *dst, char *file_name);

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
int checkHit(char* root_path, char* dir_path, char* dir_name, char* file_name);

//////////////////////////////////////////////////////////////////////////
//  get_time 								//
// =================================================================	//
// Input: char* -> get string of time info in this allocation		//
//	 								//
// Output: char* - return string of time				//			
// Purpose: make string of time info					//
//////////////////////////////////////////////////////////////////////////
char* get_time(char* str_time);

//////////////////////////////////////////////////////////////////////////
//  get_log_path 							//
// =================================================================	//
// Input: char* -> get path of logfile.txt in this allocation		//
//	 								//
// Output: char* - return string of path of logfile.txt			//			
// Purpose: make string of path of logfile.txt				//
//////////////////////////////////////////////////////////////////////////
char* get_log_path(char* log_path);

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
int writeLog(char* file_path, char*input_url, char* HorM);

//////////////////////////////////////////////////////////////////////////
// write_Terminated							//
// =================================================================	//
// Input: double -> execution time of the program			//
// 	  int -> number of "HIT" during the running time 		//
// 	  int -> number of "MISS" during the running time		//
// 									//
// Purpose: write Terminate information in logfile.txt			//
//////////////////////////////////////////////////////////////////////////
void write_Terminated(double time_spent, int hit_cnt, int miss_cnt);

//////////////////////////////////////////////////////////////////////////
// write_Terminated_Server						//
// =================================================================	//
// Input: double -> execution time of the program			//
// 	  int -> number of created child process 			//
// 									//
// Purpose: write Server_Terminate information in logfile.txt		//
//////////////////////////////////////////////////////////////////////////
void write_Terminated_Server(double time_spent, int child_cnt);

//////////////////////////////////////////////////////////////////////////
// assignment1_2							//
// =================================================================	//
// Purpose: this is a function for Sub process				//
//	    (SHA-1 function (input_url to hashed_url))			//
//	    (Check: HIT or MISS)					//
//	    (Manipulate cache directory)				//
//	    (logging)							//
//////////////////////////////////////////////////////////////////////////
int proxy(char* input_url, char* req_msg, int client_fd, int web_socket_fd);

//////////////////////////////////////////////////////////////////////////
// getIPAddr															//
// =================================================================	//
// Input: char* -> Host address	from url								//
// Output: char* -> 32bit dotted decimal IP address						//		
//																		//
// Purpose: convert Host url to 32bit dotted decimal IP address			//
//////////////////////////////////////////////////////////////////////////
char *getIPAddr(char *addr);

//////////////////////////////////////////////////////////////////////////
// get_url_host															//
// =================================================================	//
// Input: char* -> request http message									//
//		  char* -> url from request is stored in this buffer			//
//		  char* -> Host from request is stored in this buffer			//			
//																		//
// Purpose: parsing url and host name from request message				//
//////////////////////////////////////////////////////////////////////////
char *get_url_host(char *req, char *url, char *host);