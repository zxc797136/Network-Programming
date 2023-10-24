#include <iostream>
#include <cstdlib>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

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
        {                                                                     //accept HTTP request
            auto self(shared_from_this());
            socket_.async_read_some(boost::asio::buffer(data_, max_length), 
                [this, self](boost::system::error_code ec, size_t length)
                {
                    sscanf(data_, "%s %s %s %s %s", REQUEST_METHOD, REQUEST_URI, SERVER_PROTOCOL, blackhole, HTTP_HOST);
                    if(!ec)
                        do_write(length);
                }); 
        }
        void do_write(size_t length)
        {
            auto self(shared_from_this());
            async_write(socket_, buffer(status_str, strlen(status_str)),
                [this, self](boost::system::error_code ec, size_t /*length*/)
                {
                    if(!ec)
                    {
                        strcpy(SERVER_ADDR,socket_.local_endpoint().address().to_string().c_str());
                        strcpy(REMOTE_ADDR,socket_.remote_endpoint().address().to_string().c_str());

                        sprintf(SERVER_PORT, "%u", socket_.local_endpoint().port());
                        sprintf(REMOTE_PORT, "%u", socket_.remote_endpoint().port());

                        int flag = 0;
                        int query_i = 0;

                        for(int i = 0;i < 1000; i++)
                        {
                            if(REQUEST_URI[i] == '\0' ) 
                            {
                                break;
                            }
                            else if(REQUEST_URI[i] == '?') 
                            {
                                flag = 1;
                                continue;
                            }

                            if(flag)
                                QUERY_STRING[query_i++] = REQUEST_URI[i];
                        }

                        for(int i = 1; i < 100; i++) 
                        {
                            if(REQUEST_URI[i] == '\0' || REQUEST_URI[i] == '?') 
                            {
                                break;
                            }
                            EXEC_FILE[i + 1] = REQUEST_URI[i];
                        }

                        //----------------------------------------------------------set env
                        setenv("REQUEST_METHOD", REQUEST_METHOD, 1);                  
                        setenv("REQUEST_URI", REQUEST_URI, 1);
                        setenv("QUERY_STRING", QUERY_STRING, 1);
                        setenv("SERVER_PROTOCOL", SERVER_PROTOCOL, 1);
                        setenv("HTTP_HOST", HTTP_HOST, 1);
                        setenv("SERVER_ADDR", SERVER_ADDR, 1);
                        setenv("SERVER_PORT", SERVER_PORT, 1);
                        setenv("REMOTE_ADDR", REMOTE_ADDR, 1);
                        setenv("REMOTE_PORT", REMOTE_PORT, 1);
                        setenv("EXEC_FILE", EXEC_FILE, 1);
                        //---------------------------------------------------------print env
                        cout << "EXEC_FILE: " << getenv("EXEC_FILE") << endl;        
                        cout << "REQUEST_METHOD: " << getenv("REQUEST_METHOD") << endl;
                        cout << "REQUEST_URI: " << getenv("REQUEST_URI") << endl;
                        cout << "QUERY_STRING: " << getenv("QUERY_STRING") << endl;
                        cout << "SERVER_PROTOCOL: " << getenv("SERVER_PROTOCOL") << endl;
                        cout << "HTTP_HOST: " << getenv("HTTP_HOST") << endl;
                        cout << "SERVER_ADDR: " << getenv("SERVER_ADDR") << endl;
                        cout << "SERVER_PORT: " << getenv("SERVER_PORT") << endl;
                        cout << "REMOTE_ADDR: " << getenv("REMOTE_ADDR") << endl;
                        cout << "REMOTE_PORT: " << getenv("REMOTE_PORT") << endl;

                        //---------------------------------------------- replace socket to cgi program
                        ioservice.notify_fork(io_service::fork_prepare);              //prepare fork
                        if (fork() != 0)
                        {
                            ioservice.notify_fork(io_service::fork_parent);             //parent
                            socket_.close();
                        }
                        else 
                        {
                            ioservice.notify_fork(io_service::fork_child);               //child
                            int sock = socket_.native_handle();                         //Get the native socket representation
                            dup2(sock, 0);                                              //stdin
                            dup2(sock, 1);                                              //stdout
                            socket_.close();

                            if(execlp(EXEC_FILE, EXEC_FILE, NULL) < 0) 
                            {
                                cerr << "error" << endl;
                            }
                        }
                        do_read();
                    }
                });
        }
    
        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
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
                if(!ec) 
                {
                    make_shared<Session>(move(socket_))->start();
                }

                do_accept();
            });
        }
        tcp::acceptor acceptor_;
        tcp::socket socket_;
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

