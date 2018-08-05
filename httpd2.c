#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>

#define MAX 1024
#define HOME_PAGE "index.html"
void Usage(char *arg)
{
	printf("Usage %s : port\n", arg);
}

int startup(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
	    exit(2);
	}

    int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);
	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
	{
           perror("bind");
		   exit(3);
	}
	if(listen(sock, 5) < 0)
	{
	    perror("listen");
		exit(4);
	}
    return sock;
}
int get_line(int sock, char buf[], int size)
{
    int i = 0;
	char c = 'A';
	while(i < size - 1 && c != '\n' )
	{
		 recv(sock, &c, 1, 0);
	     if(c == '\r')
		 {
			recv(sock, &c, 1, 2);

			if(c == '\n')
			{
		     	recv(sock, &c, 1, 0);
			}
			else
			{
			   	buf[i++] = '\n';
                break;
			}
		 }
         buf[i++] = c;
	}
	buf[i] = '\0';
	return i;
}
void clear_head(int sock)
{
   char line[MAX];
   do
   {
      get_line(sock, line, MAX);
      printf("---line:%s", line);
   }while(strcmp(line, "\n"));
  
}


int response(int sock, char* path, int size)
{
   clear_head(sock);
   // char buf[1024] ;
   // read(sock, &buf, sizeof(buf));	
	int fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		return 404;
	}
	char* msg = "HTTP/1.0 200 OK\n\r\n";
	write(sock, msg, strlen(msg));
	sendfile(sock, fd, NULL, size);
    //fail
	close(fd);
     return 200;	

}


int exe_cgi(int sock, char* path,char* method, char* query_string)
{
    char line[MAX];
    char content_length[MAX/32];
   //printf("method:%s \n", method);
    if(strcasecmp(method,"GET") == 0)
	{
         clear_head(sock);	
	}
	else if(strcasecmp(method,"POST") == 0)
	{
         do
		 {
		     get_line(sock, line, sizeof(line));
           //  printf("---*line:%s", line);
			 if(strncmp(line, "Content-Length: ", 16) == 0)
			 {
                 strcpy(content_length,line+16);
			 }
             
		 }while(strcmp(line, "\n") != 0); 
	}
	else
	{
	    return 404;
	}
 //    printf("%s\t%s\t%s\t\n", method, query_string, content_length);
   //
   int input[2];
   int output[2];

     
   pipe(input);
   pipe(output);
   pid_t id = fork();

   if(id < 0)
   {
   	  return 404;
   }
   else if(id == 0)
   {//child
        close(input[1]);
	    close(output[0]);	
      
	    char METHOD[MAX/32];
		char QUERY_STRING[MAX];
        char CONTENT_LENGTH[MAX/32];
        sprintf(METHOD, "METHOD=%s", method);
        sprintf(QUERY_STRING, "QUERY_STRING=%s", query_string);
        sprintf(CONTENT_LENGTH, "CONTENT_LENGTH=%s", content_length);

		putenv(METHOD);
		putenv(QUERY_STRING);
		putenv(CONTENT_LENGTH);

//	    printf("%s\t%s\t%s\n", METHOD, QUERY_STRING, CONTENT_LENGTH);
//		printf("path:%s\n", path);
//      printf("%s\t%s\t%s\n", getenv("METHOD"), getenv("QUERY_STRING")\
						,getenv("CONTENT_LENGTH"));

		dup2( input[0], 0);
		dup2( output[1], 1);
		execl(path, path, NULL); 


   }
   else
   {
      close(input[0]);
	  close(output[1]);
      //
	  int size = atoi(content_length);
      int i = 0;
	  char c;
      for(; i < size; ++i)
	  {
	      recv(sock, &c, 1, 0);
		  write(input[1], &c, 1);
	  }
      
	  char* msg = "HTTP/1.0 200 OK\n\r\n";
	  send(sock, msg, strlen(msg), 0);
	  while(1)
	  {
	    if( read(output[0], &c, 1) <= 0)
		{
		 	break;
		}
		 send(sock, &c, 1, 0);
	  }
      waitpid(id, NULL, 0);

     //father
   }
}

void Ret404(int sock)
{
	clear_head(sock);
    printf("clear_head\n");
    int fd = open("./wwwroot/ret404.html", O_RDONLY);

    char *msg = "HTTP/1.0 404 OK\n\n";
    struct stat st;
    stat("./wwwroor/ret404.html", &st);
    int size = st.st_size;
	write(sock, msg, strlen(msg));
    sendfile(sock, fd, NULL,size);
    close(fd);	
}
void respondErr(int sock, int err)
{
   switch(err)
   {
      case 404:
			  Ret404(sock);
			  break;
	  case 301:
			//  Ret301();
			  break;
      default:
            // Ret404(sock);
			break;
   };
}

void * hander(void* arg)
{
	int sock = (int)arg;
	char line[MAX];
    char method[MAX/32];
	char url[MAX/8];
	int cgi = 0;
	int errCode = 0;
	char c;
    char* query_string;
#ifdef Debug
	do{
	   get_line(sock, line, MAX);
	   printf("%s", line);
	}while(line[0] != '\n');
#else
    get_line(sock, line, MAX);
    printf("getline:%s\n", line);
	int i = 0;
	int j = 0;
	while(i < sizeof(method)-1 && j < sizeof(line) && !isspace(line[j]))
	{
	    method[i] = line[j];
		i++,j++;
	}
    method[i] = '\0';
	while(j < sizeof(line) && isspace(line[j]))
	{
	    j++;
	}
	i = 0;
    while(i < sizeof(url)-1 && j < sizeof(line) && !isspace(line[j]))
	{
	   url[i] = line[j];
	   i++, j++;
	}
    url[i] = '\0';
	if(strcasecmp(method, "POST") == 0)
	{
	    cgi = 1;
	}	
    else if(strcasecmp(method, "GET") == 0)
	{
		query_string = url;
	     while(*query_string)
		 {
		    if(*query_string == '?')
			{
				*query_string = '\0';
			    query_string++;
				cgi = 1;
				break;
			}
			query_string++;
		 }
	}
	else
	{
		errCode = 404;
		goto end;
	}
	//method url query_string
    char path[MAX];
	sprintf(path, "wwwroot%s", url);
   
   	//last char
//	i = 0;
//    while(path[i])
//	{
//	   c = path[i];
//	   i++;
//	}	
	if(url[strlen(url)-1]  == '/')
	{
	   strcat(path,HOME_PAGE);
	}
    struct stat st;
	if(stat(path, &st) < 0)
	{
	   errCode=404;
	   printf("???\n");
	   goto end;
	}
	else
	{
		if(S_ISREG(st.st_mode))
		{
	    	if(S_ISDIR(st.st_mode))
			{
				strcat(path, HOME_PAGE);
			}
		    else if( (st.st_mode & S_IXUSR)  || (st.st_mode & S_IXGRP )\
						|| (st.st_mode & S_IXOTH))
			{
		 	    cgi = 1;
			}
		}
		else
		{
			 	errCode = 404;
				goto end;
		}
   }	
  	int size = st.st_size;
	
   if(cgi == 0)
   {
       printf("path:%s\n", path);	
	   errCode = response(sock, path, size);
   }
   else
   {
      
      errCode =  exe_cgi(sock, path, method, query_string);
   }

        


#endif
end:
	if(errCode != 200)
    {   
	   respondErr(sock, errCode);
	}
	close(sock);
}

int main(int argc, char * argv[])
{
	if(argc != 2)
   {
      Usage(argv[0]);
	  return 1;
   }
   int listen_sock = startup(atoi(argv[1]));
   for(;;)
   {   
	   struct sockaddr_in client;
       socklen_t len = sizeof(client);


       int new_sock =  accept(listen_sock,\
					   (struct sockaddr *)&client, &len);
	   signal(SIGPIPE, SIG_IGN);
	   if(new_sock <  0)
	   {
		  perror("accept");
	      continue;
	   }
       pthread_t id;
	   pthread_create(&id, NULL, hander, (void*)new_sock);
	   pthread_detach(id);
   }


}
