#include <iostream>
#include <string>
#include <cstring>  // strtok,c_str
#include <sys/wait.h>  // waitpid
#include <unistd.h>  // fork,pipe,read
#include <sys/types.h> //pid_t
#include <stdlib.h> //setenv,getenv
#include <fstream>
//--------------------socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 

using namespace std;

void close_table(int fd_table[1002][2],int position)
{
  close(fd_table[position][0]);
  close(fd_table[position][1]);
}

void init_table(int fd_table[1002][2])
{
 for(int i=0;i<1002;i++)
    {
      fd_table[i][0]=-1;
      fd_table[i][1]=-1;
    }
}

void npshell()
{
    int fd_table[1002][2],jump=0,cur=0,file_name=-1; //table_1001 for file
    
    init_table(fd_table);   //initial table
    setenv("PATH","bin:.",1);

    while(1)
    {

      //------------------------------cin split
      cout<<"% ";
      string input;
      getline(cin,input);
      char inputtemp[15000]={0};
      char *arg[7500];
      //arg[0]=new char(input.length()+1);
      if(input[input.length()-1]=='\r')
      {
        input[input.length()-1]='\0';
      }
      
      strcpy(inputtemp,input.c_str());
      
      char *temp=strtok(inputtemp," ");      // " " -> "\0" 
      int inputnum=0;
      while(temp!=NULL)
      {
        arg[inputnum++]=temp;
        temp=strtok(NULL," ");   
      }

      //----------------------------------type of pipe and commands

      for(int i=0;i<inputnum;i++)
      {
        char *cmd[7500];
        int cmdnum=0,type=0;

        if(!strcmp(arg[i],"exit"))
        {
          exit(0);
        }
        else if(!strcmp(arg[i],"printenv"))
        {
          if(arg[i+1]!=NULL)
            {
              if(getenv(arg[i+1])!=NULL)
                cout<<getenv(arg[i+1])<<endl;
              i++;
            }    
          i++;  
        }
        else if(!strcmp(arg[i],"setenv"))
        {
          setenv(arg[i+1],arg[i+2],1);
          i+=3;
        }

        for(;i<inputnum;i++,cmdnum++)
        {
          if(arg[i][0]=='|')                               //pipe is '|'
          {
            if(arg[i][1]=='\0')
            {
              jump=(1+cur)%1001;
            }
            else
            {
              string substring=arg[i];
              substring=substring.assign(substring,1,substring.length()-1);
              jump=atoi(substring.c_str());
              jump=(jump+cur)%1001;
            }
            type=1;
            break;
          }
          else if(arg[i][0]=='!')                         //pipe is '!'
          {
            string substring=arg[i];
            substring=substring.assign(substring,1,substring.length()-1);
            jump=atoi(substring.c_str());
            jump=(jump+cur)%1001;
            type=2;
            break;
          }
          else if(arg[i][0]=='>')                          //pipe is '>'
          {
            file_name=0;
            type=3;
            break;
          }

          if(file_name==0)
            file_name++;   

          cmd[cmdnum]=arg[i];
        }
        cmd[cmdnum]=NULL;

        //---------------------------------------------pipe

        if(type==1  && fd_table[jump][0]==-1)
        {
          if(pipe(fd_table[jump])==-1)
            exit(1);
        }
        else if(type==2 && fd_table[jump][0]==-1)
        {
          if(pipe(fd_table[jump])==-1)
            exit(1);
        }
        else if(type==3)
        {
          if(pipe(fd_table[1001])==-1)
            exit(1);
        }

        pid_t pid=fork();

        if(pid<0)
        {
          exit(1);
        }
        else if(pid==0)
        {

          if(type==1)
          {
            dup2(fd_table[jump][1],1);
            close_table(fd_table,jump);
          }
          else if(type==2)
          {
            dup2(fd_table[jump][1],1);
            dup2(fd_table[jump][1],2);
            close_table(fd_table,jump);
          }
          else if(type==3)
          {
            dup2(fd_table[1001][1],1);
            close_table(fd_table,1001);
          }

          if(file_name==1)
          {
            dup2(fd_table[1001][0],0);
            close_table(fd_table,1001);
      
            fstream file;
            file.open(cmd[0],ios::out|ios::trunc);
    
            char buff[9999]={0};
            while(read(0,buff,sizeof(buff))>0)
              file<<buff;

            file.close();
            exit(0);
          }
          
          if(fd_table[cur][0]>-1)
          {
            dup2(fd_table[cur][0],0);
            close_table(fd_table,cur);
          }

          if(execvp(cmd[0],cmd)==-1)
          {
            cerr<<"Unknown command: ["<<cmd[0]<<"].\n"; 
            exit(1);
          }
        }
        else
        {
          if(fd_table[cur][0]>-1)
          {
            close_table(fd_table,cur);
            fd_table[cur][0]=-1;
            fd_table[cur][1]=-1;
          }
    
          if(file_name==1)
          {
            close_table(fd_table,1001);
            fd_table[1001][0]=-1;
            fd_table[1001][1]=-1;
            file_name=-1;
          }
          
          int status;
          waitpid(-1, &status, 0);

          if(type!=3)
            cur=(cur+1)%1001;
        }
      }
    }
    
}

int main(int argc,char *argv[])
{
	int sockfd,newsockfd;

	struct sockaddr_in cli_addr,serv_addr;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		exit(1);
	}
  bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

  int flag=1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &flag, sizeof(int)) < 0)
	{
		exit(1);
	}
  if(bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
	{
		exit(1);
	}
	listen(sockfd,30);
  for(;;)
  {
    socklen_t clilen=sizeof(cli_addr);
		newsockfd=accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd<0)
		{
		  exit(1);
		}	

    pid_t pid=fork();
		if(pid<0)
    {
			exit(1);
    }
		else if(pid==0)
		{
			close(sockfd);			
			dup2(newsockfd,0);
			dup2(newsockfd,1);
			dup2(newsockfd,2);
			npshell();
			close(newsockfd);

			exit(0);
		}
		else
    {
			close(newsockfd);	
    }
  }


  return 0;
}

