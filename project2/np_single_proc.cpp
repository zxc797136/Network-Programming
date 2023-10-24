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

int client[30];
char client_name[30][100]={0};
char client_ip[30][100]={0};
char client_port[30][100]={0};
char message[9999]={0};
int fd_table[30][1002][2];  //table_1001 for file
int client_fd_table[30][30][2];
int jump=0;
int cur[30]={0};
int file_name=-1;
int client_out=0,client_in=0;


void broadcast()
{
  for(int broadcast_i=0;broadcast_i<30;broadcast_i++)
  {
    if(client[broadcast_i]>0)
      write(client[broadcast_i],message,strlen(message));
  }
}

void initial_all()
{

  for(int i=0;i<30;i++)
  {
    client[i]=-1;
    for(int j=0;j<1002;j++)
    {
      fd_table[i][j][0]=-1;
      fd_table[i][j][1]=-1;
    }
    for(int j=0;j<30;j++)
    {
      client_fd_table[i][j][0]=-1;
      client_fd_table[i][j][1]=-1;

    }
  }
}



int npshell(int index)
{
    setenv("PATH","bin:.",1);
   
      //------------------------------cin split
     
     // cout<<"% ";
      string input;
      getline(cin,input);
      char inputtemp[15000]={0},tellmessage[15000]={0};
      char *arg[7500];
      //arg[0]=new char(input.length()+1);
      if(input[input.length()-1]=='\r')
      {
        input[input.length()-1]='\0';
      }
      
      strcpy(inputtemp,input.c_str());
      strcpy(tellmessage,input.c_str());
      
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
          memset(message,0,sizeof(message));
          strcpy(message,"*** User '");
          strcat(message,client_name[index]);
          strcat(message,"' left. ***\n\0");

          broadcast();
          
          for(int i=0;i<30;i++)
          {
            client_fd_table[i][index][0]=-1;
            client_fd_table[i][index][1]=-1;
            client_fd_table[index][i][0]=-1;
            client_fd_table[index][i][1]=-1;
          }

          return 1;
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
        else if(!strcmp(arg[i],"who"))
        {
          cout<<"<ID>\t<nickname>\t<IP:port>\t<indicate me>\n";
          for(int k=0;k<30;k++)
          {
            if(client[k]>=0)
            {
              cout<<k+1<<"\t"<<client_name[k]<<"\t"<<client_ip[k]<<":"<<client_port[k];
              if(index==k)
                cout<<"\t<-me";
              cout<<endl;
            }
          }	
          i++;
        }
        else if(!strcmp(arg[i],"tell"))
        {
          if(client[atoi(arg[i+1])-1]<0)
          {
            cout<<"*** Error: user #"<<arg[i+1]<<" does not exist yet. ***\n";
          }
          else
          {
            char *tell_temp=strtok(tellmessage," "); 
            tell_temp=strtok(NULL," ");
            tell_temp=strtok(NULL,"\0");
            
            memset(message,0,sizeof(message));
            strcpy(message,"*** ");
            strcat(message,client_name[index]);
            strcat(message," told you ***: ");
            strcat(message,tell_temp);
            strcat(message,"\n\0");	
            
            write(client[atoi(arg[i+1])-1],message,strlen(message));
          }
          i=inputnum;
        }
        else if(!strcmp(arg[i],"yell"))
        {
          if(inputnum>1)
          {
            char *tell_temp=strtok(tellmessage," "); 
            tell_temp=strtok(NULL,"\0");
          
            memset(message,0,sizeof(message));
            strcpy(message,"*** ");
            strcat(message,client_name[index]);
            strcat(message," yelled ***: ");
            strcat(message,tell_temp);
            strcat(message,"\n\0");	
            
            broadcast();
          }	
          i=inputnum;
        }
        else if(!strcmp(arg[i],"name"))
        {
          if(arg[i+1]!=NULL)
          {
            int exist_name=0;
            for(int name_i=0;name_i<30;name_i++)
            {
              if(client[name_i]>0 && !strcmp(arg[i+1],client_name[name_i]))
              {
                cout<<"*** User '"<<arg[i+1]<<"' already exists. ***\n";
                exist_name=1;
                break;
              }
            }
            if(!exist_name)
            {
              strcpy(client_name[index],arg[i+1]);
              
              memset(message,0,sizeof(message));
              strcpy(message,"*** User from ");
              strcat(message,client_ip[index]);
              strcat(message,":");
              strcat(message,client_port[index]);
              strcat(message," is named '");
              strcat(message,client_name[index]);
              strcat(message,"'. ***\n\0");	

              broadcast();
            }
          }
          i=inputnum;
        }



        for(;i<inputnum;i++,cmdnum++)
        {
          if(arg[i][0]=='|')                               //pipe is '|'
          {
            if(arg[i][1]=='\0')
            {
              jump=(1+cur[index])%1001;
            }
            else
            {
              string substring=arg[i];
              substring=substring.assign(substring,1,substring.length()-1);
              jump=atoi(substring.c_str());
              jump=(jump+cur[index])%1001;
            }
            type=1;
            break;
          }
          else if(arg[i][0]=='!')                         //pipe is '!'
          {
            string substring=arg[i];
            substring=substring.assign(substring,1,substring.length()-1);
            jump=atoi(substring.c_str());
            jump=(jump+cur[index])%1001;
            type=2;
            break;
          }
          else if(arg[i][0]=='>')                          //pipe is '>'
          {
            if(arg[i][1]=='\0')
            {
              file_name=0;
              type=3;
              break;
            }
            else
            {
              string substring=arg[i];
              substring=substring.assign(substring,1,substring.length()-1);
              client_out=atoi(substring.c_str());
             
              if(client[client_out-1]<0)
              {
                cerr<<"*** Error: user #"<<client_out<<" does not exist yet. ***\n";
              }
              else if(client_fd_table[index][client_out-1][1]>0)
              {
                cerr<<"*** Error: the pipe #"<<client_out<<"->#"<<index<<" does not exist yet. ***\n";
              }

              type=4;
              break;
            }       
          }
          else if(arg[i][0]=='<')                          //pipe is '<'
          {
            string substring=arg[i];
            substring=substring.assign(substring,1,substring.length()-1);
            client_in=atoi(substring.c_str());
     
            if(client[client_in-1]<0)
            {
              cerr<<"*** Error: user #"<<client_in<<" does not exist yet. ***\n";
            }
            else if(client_fd_table[client_in-1][index][0]<0)
            {
              cerr<<"*** Error: the pipe #"<<client_in<<"->#"<<index<<" does not exist yet. ***\n";
            }
        
            type=5;
            break;
                  
          }

          if(file_name==0)
            file_name++;   

          cmd[cmdnum]=arg[i];
        }
        cmd[cmdnum]=NULL;

        //---------------------------------------------pipe

        if(type==1  && fd_table[index][jump][0]==-1)
        {
          if(pipe(fd_table[index][jump])==-1)
            exit(1);
        }
        else if(type==2 && fd_table[index][jump][0]==-1)
        {
          if(pipe(fd_table[index][jump])==-1)
            exit(1);
        }
        else if(type==3)
        {
          if(pipe(fd_table[index][1001])==-1)
            exit(1);
        }
        else if(type==4)
        {
          if(pipe(client_fd_table[index][client_out-1])==-1)
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
            dup2(fd_table[index][jump][1],1);
            close(fd_table[index][jump][0]);
            close(fd_table[index][jump][1]);
          }
          else if(type==2)
          {
            dup2(fd_table[index][jump][1],1);
            dup2(fd_table[index][jump][1],2);
            close(fd_table[index][jump][0]);
            close(fd_table[index][jump][1]);
          }
          else if(type==3)
          {
            dup2(fd_table[index][1001][1],1);
            close(fd_table[index][1001][0]);
            close(fd_table[index][1001][1]);
          }
          else if(type==4)
          {
            memset(message,0,sizeof(message));
            strcpy(message,"*** ");
            strcat(message,client_name[index]);
            strcat(message," (#");
            strcat(message,to_string(index).c_str());
            strcat(message,") just piped '");
            strcat(message,input.c_str());
            strcat(message,"' to ");
            strcat(message,client_name[client_out-1]);
            strcat(message," (#");
            strcat(message,to_string(client_out).c_str());
            strcat(message,") ***\n\0");

            broadcast();

            dup2(client_fd_table[index][client_out-1][1],1);
            close(client_fd_table[index][client_out-1][0]);
            close(client_fd_table[index][client_out-1][1]);	
          }
          else if(type==5)
          {
           
            memset(message,0,sizeof(message));
            strcpy(message,"*** ");
            strcat(message,client_name[index]);
            strcat(message," (#");
            strcat(message,to_string(index).c_str());
            strcat(message,") just received from ");
            strcat(message,client_name[client_in-1]);
            strcat(message," (#");
            strcat(message,to_string(client_in).c_str());
            strcat(message,") by '");
            strcat(message,input.c_str());
            strcat(message,"' ***\n\0");

            broadcast();
            
            dup2(client_fd_table[client_in-1][index][0],0);
            close(client_fd_table[client_in-1][index][0]);
            close(client_fd_table[client_in-1][index][1]);
          }
          



          if(file_name==1)
          {
            dup2(fd_table[index][1001][0],0);
            close(fd_table[index][1001][0]);
            close(fd_table[index][1001][1]);
      
            fstream file;
            file.open(cmd[0],ios::out|ios::trunc);
    
            char buff[9999]={0};
            while(read(0,buff,sizeof(buff))>0)
              file<<buff;

            file.close();
            exit(0);
          }
          
          if(fd_table[index][cur[index]][0]>-1)
          {
            dup2(fd_table[index][cur[index]][0],0);
            close(fd_table[index][cur[index]][0]);
            close(fd_table[index][cur[index]][1]);
          }

        
          if(execvp(cmd[0],cmd)==-1)
          {
            cerr<<"Unknown command: ["<<cmd[0]<<"].\n"; 
            exit(1);
          }
        }
        else
        {
          if(fd_table[index][cur[index]][0]>-1)
          {
            close(fd_table[index][cur[index]][0]);
            close(fd_table[index][cur[index]][1]);
            fd_table[index][cur[index]][0]=-1;
            fd_table[index][cur[index]][1]=-1;
          }
          
          if(file_name==1)
          {
            close(fd_table[index][1001][0]);
            close(fd_table[index][1001][1]);
            fd_table[index][1001][0]=-1;
            fd_table[index][1001][1]=-1;
            file_name=-1;
          }
          if(type==4)
          {
           // close(client_fd_table[index][client_out-1][0]);
            close(client_fd_table[index][client_out-1][1]);
			    //	client_fd_table[index][client_out-1][0]=-1;
            client_fd_table[index][client_out-1][1]=-1;
            client_out=0;
          }
          if(type==5)
          {
            close(client_fd_table[client_in-1][index][0]);
           // close(client_fd_table[client_in-1][index][1]);
				    client_fd_table[client_in-1][index][0]=-1;
           // client_fd_table[client_in-1][index][1]=-1;
            client_in=0;
          }
          
          int status;
          waitpid(-1, &status, 0);

         

          if(type!=3)
            cur[index]=(cur[index]+1)%1001;
        }
       
      }
  write(client[index],"% ",2);   
  return 0;
  
}

int main(int argc,char *argv[])
{
  initial_all();
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

  fd_set afds,rfds;
  //int nfds;
  
  FD_ZERO(&afds); 
  FD_SET(sockfd,&afds); 
 // nfds=getdtablesize();    
  int maxfd=sockfd;      //maxfd     http://blog.yslin.tw/2011/06/c-socket-server-sample.html
  
  
  for(;;)
  {
    rfds=afds;
    if((select(maxfd+1,&rfds,NULL,NULL,NULL))<0)
    {
      exit(1);
    }
    
    if(FD_ISSET(sockfd,&rfds))
    {
      socklen_t clilen=sizeof(cli_addr);
      newsockfd=accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
      if(newsockfd<0)
      {
        exit(1);
      }	

      int index=0;
      for(index=0;index<30;index++)
      {
        if(client[index]<0)
        {
          client[index]=newsockfd;
          //cout<<newsockfd<<endl;
          break;
        }
      }

      if(index==30)
      {
        close(newsockfd);
        continue;
      } 
     

      strcpy(client_name[index],"(no name)");
      char client_addr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(cli_addr.sin_addr), client_addr, INET_ADDRSTRLEN);
			strcpy(client_ip[index],client_addr);
			sprintf(client_port[index],"%d",ntohs(cli_addr.sin_port));
 
      char welcome[] =  "****************************************\n"
                        "** Welcome to the information server. **\n"
                        "****************************************\n";
	           
	    write(client[index],welcome,strlen(welcome));

      memset(message,0,sizeof(message));
      strcpy(message,"*** User '");
			strcat(message,client_name[index]);
			strcat(message,"' entered from ");
			strcat(message,client_ip[index]);
			strcat(message,":");
			strcat(message,client_port[index]);
			strcat(message,". ***\n\0");

      broadcast();

      write(client[index],"% ",2);
      
      if(newsockfd>maxfd)
        maxfd=newsockfd;

 
      FD_SET(newsockfd,&afds);
      
      
    }
    for(int i=0;i<=29;i++)
    { 
      if(FD_ISSET(client[i],&rfds)&&client[i]!=-1)
      {  
        dup2(client[i], 0);
        dup2(client[i], 1);
        dup2(client[i], 2);
      
        if(!npshell(i))
          continue;
        
        close(client[i]); 
        FD_CLR(client[i], &afds); 
        client[i] = -1;
        
      }
    }
  }
  return 0;
}
  