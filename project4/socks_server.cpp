#include <iostream>
#include <cstdlib>
#include <memory>
#include <utility>
#include <fstream>
#include <regex>
#include <boost/asio.hpp>
#include <sys/wait.h>

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

io_service ioservice;

class Session : public enable_shared_from_this<Session> 
{
    public:
        Session(tcp::socket socket) : socket_(move(socket)) {}

        void start() 
        { 
            do_read(); 
        }

    private:
        void do_read() 
        {                                                                     
            auto self(shared_from_this());

            memset(data_, 0, max_length);
            socket_.async_read_some(boost::asio::buffer(data_, max_length), 
                [this, self](boost::system::error_code ec, size_t length)
                {
                    if(!ec)
                    {
                        ParseSOCK4Request(length);
                        Reply();
                    }
                }); 
        }
        //     socket_.async_read_some(boost::asio::buffer(data_, max_length), 
        //         [this, self](boost::system::error_code ec, size_t length)
        //         {
        //             sscanf(data_, "%s %s %s %s %s", REQUEST_METHOD, REQUEST_URI, SERVER_PROTOCOL, blackhole, HTTP_HOST);
        //             if(!ec)
        //                 do_write(length);
        //         }); 
        // }



        // void do_write(size_t length)
        // {
        //     auto self(shared_from_this());
        //     async_write(socket_, buffer(status_str, strlen(status_str)),
        //         [this, self](boost::system::error_code ec, size_t /*length*/)
        //         {
        //             if(!ec)
        //             {
        //                 strcpy(SERVER_ADDR,socket_.local_endpoint().address().to_string().c_str());
        //                 strcpy(REMOTE_ADDR,socket_.remote_endpoint().address().to_string().c_str());

        //                 sprintf(SERVER_PORT, "%u", socket_.local_endpoint().port());
        //                 sprintf(REMOTE_PORT, "%u", socket_.remote_endpoint().port());

        //                 int flag = 0;
        //                 int query_i = 0;

        //                 for(int i = 0;i < 1000; i++)
        //                 {
        //                     if(REQUEST_URI[i] == '\0' ) 
        //                     {
        //                         break;
        //                     }
        //                     else if(REQUEST_URI[i] == '?') 
        //                     {
        //                         flag = 1;
        //                         continue;
        //                     }

        //                     if(flag)
        //                         QUERY_STRING[query_i++] = REQUEST_URI[i];
        //                 }

        //                 for(int i = 1; i < 100; i++) 
        //                 {
        //                     if(REQUEST_URI[i] == '\0' || REQUEST_URI[i] == '?') 
        //                     {
        //                         break;
        //                     }
        //                     EXEC_FILE[i + 1] = REQUEST_URI[i];
        //                 }

        //                 //----------------------------------------------------------set env
        //                 setenv("REQUEST_METHOD", REQUEST_METHOD, 1);                  
        //                 setenv("REQUEST_URI", REQUEST_URI, 1);
        //                 setenv("QUERY_STRING", QUERY_STRING, 1);
        //                 setenv("SERVER_PROTOCOL", SERVER_PROTOCOL, 1);
        //                 setenv("HTTP_HOST", HTTP_HOST, 1);
        //                 setenv("SERVER_ADDR", SERVER_ADDR, 1);
        //                 setenv("SERVER_PORT", SERVER_PORT, 1);
        //                 setenv("REMOTE_ADDR", REMOTE_ADDR, 1);
        //                 setenv("REMOTE_PORT", REMOTE_PORT, 1);
        //                 setenv("EXEC_FILE", EXEC_FILE, 1);
        //                 //---------------------------------------------------------print env
        //                 cout << "EXEC_FILE: " << getenv("EXEC_FILE") << endl;        
        //                 cout << "REQUEST_METHOD: " << getenv("REQUEST_METHOD") << endl;
        //                 cout << "REQUEST_URI: " << getenv("REQUEST_URI") << endl;
        //                 cout << "QUERY_STRING: " << getenv("QUERY_STRING") << endl;
        //                 cout << "SERVER_PROTOCOL: " << getenv("SERVER_PROTOCOL") << endl;
        //                 cout << "HTTP_HOST: " << getenv("HTTP_HOST") << endl;
        //                 cout << "SERVER_ADDR: " << getenv("SERVER_ADDR") << endl;
        //                 cout << "SERVER_PORT: " << getenv("SERVER_PORT") << endl;
        //                 cout << "REMOTE_ADDR: " << getenv("REMOTE_ADDR") << endl;
        //                 cout << "REMOTE_PORT: " << getenv("REMOTE_PORT") << endl;

        //                 //---------------------------------------------- replace socket to cgi program
        //                 ioservice.notify_fork(io_service::fork_prepare);              //prepare fork
        //                 if (fork() != 0)
        //                 {
        //                     ioservice.notify_fork(io_service::fork_parent);             //parent
        //                     socket_.close();
        //                 }
        //                 else 
        //                 {
        //                     ioservice.notify_fork(io_service::fork_child);               //child
        //                     int sock = socket_.native_handle();                         //Get the native socket representation
        //                     dup2(sock, 0);                                              //stdin
        //                     dup2(sock, 1);                                              //stdout
        //                     socket_.close();

        //                     if(execlp(EXEC_FILE, EXEC_FILE, NULL) < 0) 
        //                     {
        //                         cerr << "error" << endl;
        //                     }
        //                 }
        //                 do_read();
        //             }
        //         });
        // }


        void ParseSOCK4Request(int length)
        {
            unsigned char temp_domain_name[1024];

            D_PORT = to_string((int)(data_[2] < 0 ? (data_[2] + 256) * 256 : data_[2] * 256) +                     //D_PORT
                                (int)(data_[3] < 0 ? data_[3] + 256 : data_[3]));
            D_IP = "";

            for(int i = 4;i < 8;i++)                                                                               //D_IP
            {
                if(i != 4) D_IP += ".";
                int temp = (data_[i] < 0 ? (int)data_[i] + 256 : data_[i]);
                D_IP += to_string(temp);
            }
            bool flag = false;
            int count = 0;

            for(int i = 0; i < (int)length;i++) 
            {
                if (!data_[i])
                {
                    flag = true;
                    count = 0;
                }
                else if (flag)
                {
                    temp_domain_name[count] = data_[i];
                    temp_domain_name[count + 1] = '\0';
                    ++count;
                }	
            }
            DOMAIN_NAME = string((char*)temp_domain_name);
        }
        void Reply()
        {
            string command;
            string reply;
            if(data_[0] != 0x04 || D_IP == "0.0.0.0" || !CheckFireWall(data_[1]))
            {
                reply = "Reject";
                ReplyReject();
            }
            else if(data_[1] == 0x01)                                 //connect mode(CD=0x01)
            {
                command = "CONNECT";
                reply = "Accept";
                ReplyConnect();
            }
            else if(data_[1] == 0x02)                                 //bind mode(CD=0x02)
            {
                command = "BIND";
                reply = "Accept";
                ReplyBind();
            }
            PrintSOCK4Information(socket_.remote_endpoint().address().to_string(),
                            to_string(socket_.remote_endpoint().port()), D_IP, D_PORT, command, reply);
            cout << "----------------------------------------------------" << endl;
        }

        //-------------------------------------------------------------------------------沒有符合允許的位址
        void ReplyReject()                                     
        {
            message[0] = 0;
            message[1] = 0x5B;
            auto self(shared_from_this());
            async_write(socket_, boost::asio::buffer(message, 8),
                        [this, self](boost::system::error_code ec, size_t)
                        {
                            if(ec) return;
                        });
        }
        //-------------------------------------------------------------------------------有符合允許的位址
        void ReplyConnect()                                   
        {
            message[0] = 0;
            message[1] = 0x5A;
            for (int i = 2;i < 8;++i) message[i] = data_[i];

            auto self(shared_from_this());
            async_write(socket_, boost::asio::buffer(message, 8),
                        [this, self](boost::system::error_code ec, size_t)
                        {
                            if(ec) return;

                            web_socket = new tcp::socket(ioservice);
                            tcp::endpoint ed;

                            if (D_IP[0] == '0')
                            {
                                tcp::resolver resolver(ioservice);
                                tcp::resolver::query query(DOMAIN_NAME, D_PORT);
                                tcp::resolver::iterator iter = resolver.resolve(query);
                                ed = *iter;
                            }
                            else
                            {
                                ed = tcp::endpoint(address::from_string(D_IP), atoi((D_PORT).c_str()));
                            }

                            (*web_socket).connect(ed);

                            ReadFromWeb();
                            ReadFromClient();
                        });
        }

        void ReplyBind()
        {
            int temp;

            message[0] = 0;
            message[1] = 0x5A;                                   //90(accept)

            tcp::endpoint ed(address::from_string("0.0.0.0"), 0);

            acceptor = new tcp::acceptor(ioservice);

            (*acceptor).open(tcp::v4());
            (*acceptor).set_option(tcp::acceptor::reuse_address(true));
            (*acceptor).bind(ed);
            (*acceptor).listen(boost::asio::socket_base::max_connections);

            temp = (*acceptor).local_endpoint().port() / 256;
            message[2] = temp;
            temp = (*acceptor).local_endpoint().port() % 256;
            message[3] = temp;

            for(int i = 4; i < 8;i++) message[i] = 0;

            auto self(shared_from_this());
            async_write(socket_, boost::asio::buffer(message, 8),
                        [this, self](boost::system::error_code ec, size_t)
                        {
                            if(ec) return;
                            
                            web_socket = new tcp::socket(ioservice);
                            (*acceptor).accept(*web_socket);

                            boost::asio::write(socket_, boost::asio::buffer(message, 8));

                            ReadFromWeb();
                            ReadFromClient();
                        });
        }
        void ReadFromWeb()
        {
            auto self(shared_from_this());

            memset(reply_from_web, 0, max_length);
            (*web_socket).async_read_some(boost::asio::buffer(reply_from_web),
                                        [this, self](boost::system::error_code ec, size_t length)
                                        {
                                            if(ec)
                                            {
                                                if(ec == boost::asio::error::eof)
                                                {
                                                    (*web_socket).close();
                                                    socket_.close();
                                                }
                                                return;
                                            }
                                            WriteToClient(length);
                                        });
        }
        void WriteToClient(int length)
        {
            auto self(shared_from_this());
            async_write(socket_,boost::asio::buffer(reply_from_web, length),
                        [this, self](boost::system::error_code ec, size_t)
                        {
                            if(ec) return;
                            ReadFromWeb();
                        });
        }
        void ReadFromClient()
        {
            auto self(shared_from_this());

            memset(reply_from_client, 0, max_length);
            socket_.async_read_some(boost::asio::buffer(reply_from_client),
                                    [this, self](boost::system::error_code ec, size_t length)
                                    {
                                        if(ec)
                                        {
                                            if(ec == boost::asio::error::eof)
                                            {
                                                (*web_socket).close();
                                                socket_.close();
                                            }
                                            return;
                                        }
                                        RequestToWeb(length);
                                    });
        }
        void RequestToWeb(int length)
        {
            auto self(shared_from_this());
            async_write((*web_socket), boost::asio::buffer(reply_from_client, length),
                        [this, self](boost::system::error_code ec, size_t)
                        {
                            if(ec) return;
                            ReadFromClient();
                        });
        }
        bool CheckFireWall(char command)
        {
            ifstream file;
            file.open("socks.conf");

            if(!file)
            {
                cerr << "Can't find socks.conf" << endl;
                return true;
            }
            string rule;

            while(getline(file, rule))
            {
                stringstream s(rule);
                string event, type, ip;

                s >> event >> type >> ip;
 
                string unchecked_ip;

                if(D_IP[0] == '0')
                {
                    tcp::resolver resolver(ioservice);
                    tcp::resolver::query q(DOMAIN_NAME, D_PORT);
                    tcp::resolver::iterator it = resolver.resolve(q);
                    tcp::endpoint ed(*it);
                    unchecked_ip = ed.address().to_string();
                }
                else 
                {
                    unchecked_ip = D_IP;
                }
                if(event == "permit" && ((type == "c" && command == 0x01) || (type == "b"  && command == 0x02)))
                {
                    string reg_string;

                    for(int i = 0;i < (int)ip.length(); i++)
                    {
                        if(ip[i] == '*')
                        {
                            reg_string += "[0-9]+";

                        }
                        else
                        {
                            reg_string += ip[i];
                        }
                    }
 
                    regex reg(reg_string);
                    if(regex_match(unchecked_ip, reg))
                    {
                        file.close();
                        return true;
                    }
                }
            }
            file.close();
            return false;
        }
        void PrintSOCK4Information(string S_IP, string S_PORT, string D_IP, string D_PORT, string command, string reply)
        {
            string parse_ip;
            if(D_IP[0] == '0')
            {
                tcp::resolver resolver(ioservice);
                tcp::resolver::query q(DOMAIN_NAME, D_PORT);
                tcp::resolver::iterator it = resolver.resolve(q);
                tcp::endpoint ed(*it);
                parse_ip = ed.address().to_string();
            }
            cout << "<S_IP>" << S_IP << endl;
            cout << "<S_PORT>" << S_PORT << endl;
            if(parse_ip.empty()) cout << "<D_IP>" << D_IP << endl;
            else cout << "<D_IP>" << parse_ip << endl;
            cout << "<D_PORT>" << D_PORT << endl;
            cout << "<Command>" << command << endl;
            cout << "<Reply>" << reply << endl;
        }
    
        tcp::socket socket_;
        tcp::socket *web_socket;
        tcp::acceptor *acceptor;
        enum { max_length = 10240 };
        char data_[max_length];
        char reply_from_web[max_length];
        char reply_from_client[max_length];
        char status_str[200] = "HTTP/1.1 200 OK\n";
        char REQUEST_METHOD[20];
        char REQUEST_URI[1000];
        char QUERY_STRING[1000];
        char SERVER_PROTOCOL[100];
        char HTTP_HOST[100];
        char SERVER_ADDR[100];
        char SERVER_PORT[10];
        char REMOTE_ADDR[100];
        char REMOTE_PORT[10];
        char EXEC_FILE[100] = "./";
        char blackhole[100];
        char message[8];
        string DOMAIN_NAME;
        string D_PORT;
        string D_IP;
};

class Server 
{
    public:
        Server(short port) : acceptor_(ioservice, tcp::endpoint(tcp::v4(), port)),
                            socket_(ioservice)
                            {
                                do_accept();
                            }
    private:
        void do_accept()
        {
            acceptor_.async_accept(socket_, [this](boost::system::error_code ec) 
            {
                if(ec) return;

                ioservice.notify_fork(io_service::fork_prepare);           //prepare fork
                pid_t pid = fork();
                if(pid != 0)
                {
                    ioservice.notify_fork(io_service::fork_parent);        //parent
                    socket_.close();

                    pid_pool.push_back(pid);

                    int waitPID, status;
                    for(int i = 0;i < (int)pid_pool.size();i++)
                    {
                        waitPID = waitpid(pid_pool[i], &status, WNOHANG);
                        if(waitPID == pid_pool[i]) pid_pool.erase(pid_pool.begin() + i, pid_pool.begin() + i + 1);
                    }

                    do_accept();
                }
                else
                {
                    ioservice.notify_fork(io_service::fork_child);           //child
                    make_shared<Session>(move(socket_))->start();
                }
            });
        }
        tcp::acceptor acceptor_;
        tcp::socket socket_;
        vector<pid_t> pid_pool;
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)                                       //no port
    {
      cerr << argv[0]<<" <port>"<<endl;
      return 1;
    }

    Server s(atoi(argv[1]));                 //port

    ioservice.run();
  }
  catch (exception& e)
  {
    cerr << "Exception: " << e.what() << endl;
  }
  return 0;
}

