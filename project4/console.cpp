#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>


using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

map<string ,string>  params;
vector<string> commands;
int user_num;
io_service ioservice;

void get_para()
{
    istringstream s;
    s.str(getenv("QUERY_STRING"));

    string keyval, key, val;

    while(getline(s, keyval, '&'))
    {
        istringstream is(keyval);

        if(getline(getline(is, key, '='), val))
            params[key] = val;
    }
}

void print_html()
{
    cout << "Content-type: text/html" << endl << endl;
    cout << "<!DOCTYPE html>" <<
            "<html lang=\"en\"" <<
                "<head>" <<
                    "<meta charset=\"UTF-8\" />" <<
                    "<title>NP Project 3 Console</title>" <<
                    "<link" <<
                        " rel=\"stylesheet\"" <<
                        " href=\"https://cdn.jsdelivr.net/npm/bootstrap@4.5.3/dist/css/bootstrap.min.css\"" <<
                        " integrity=\"sha384-TX8t27EcRE3e/ihU7zmQxVncDAy5uIKz4rEkgIXeMed4M0jlfIDPvg6uqKI2xXr2\"" <<
                        " crossorigin=\"anonymous\""
                    "/>" <<
                    "<link" <<
                        " href=\"https://fonts.googleapis.com/css?family=Source+Code+Pro\"" <<
                        " rel=\"stylesheet\"" <<
                    "/>"
                    "<link" <<
                        " rel=\"icon\"" <<
                        " type=\"image/png\""
                        " href=\"https://cdn0.iconfinder.com/data/icons/small-n-flat/24/678068-terminal-512.png\"" <<
                    "/>" <<
                    "<style>" <<
                        "* {" <<
                            "font-family: 'Source Code Pro', monospace;" <<
                            "font-size: 1rem !important;" <<
                        "}" <<
                        "body {" <<
                            "background-color: #212529;" <<
                        "}" <<
                        "pre {" <<
                            "color: #cccccc;" <<
                        "}" <<
                        "b {" <<
                            "color: #01b468;" <<
                        "}" <<
                    "</style>" <<
                "</head>" <<
            "<body>" <<
                "<table class=\"table table-dark table-bordered\">" <<
                    "<thead>" <<
                        "<tr id=\"user\">" <<
                        "</tr>" <<
                    "</thead>" <<
                    "<tbody>" <<
                        "<tr id=\"console\">" <<
                        "</tr>" <<
                    "</tbody>" <<
                "</table>" <<
            "</body>" <<
            "</html>";
}
void output(string id, string content)
{   
    cout << "<script>document.getElementById('" << id << "').innerHTML += '" << content << "';</script>" << endl;
}
void output_command(string id, string content)
{
    cout << "<script>document.getElementById('" << id << "').innerHTML += '<b>" << content << "</b><br>';</script>" << endl;
}
void user_check()
{
    if(params["h4"] != "")
    {
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user1\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user2\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user3\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user4\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user5\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console1\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console2\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console3\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console4\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console5\"></pre></td>';</script>" << endl;
        output("user1",params["h0"] + ":" + params["p0"]);
        output("user2",params["h1"] + ":" + params["p1"]);
        output("user3",params["h2"] + ":" + params["p2"]);
        output("user4",params["h3"] + ":" + params["p3"]);
        output("user5",params["h4"] + ":" + params["p4"]);
        user_num = 5;
    }    
    else if(params["h3"] != "")
    {
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user1\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user2\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user3\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user4\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console1\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console2\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console3\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console4\"></pre></td>';</script>" << endl;
        output("user1",params["h0"] + ":" + params["p0"]);
        output("user2",params["h1"] + ":" + params["p1"]);
        output("user3",params["h2"] + ":" + params["p2"]);
        output("user4",params["h3"] + ":" + params["p3"]);
        user_num = 4;
    }    
    else if(params["h2"] != "")
    {
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user1\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user2\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user3\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console1\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console2\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console3\"></pre></td>';</script>" << endl;
        output("user1",params["h0"] + ":" + params["p0"]);
        output("user2",params["h1"] + ":" + params["p1"]);
        output("user3",params["h2"] + ":" + params["p2"]);
        user_num = 3;
    }
    else if(params["h1"] != "")
    {
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user1\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user2\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console1\"></pre></td>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console2\"></pre></td>';</script>" << endl;
        output("user1",params["h0"] + ":" + params["p0"]);
        output("user2",params["h1"] + ":" + params["p1"]);
        user_num = 2;
    }
    else
    {
        cout << "<script>document.getElementById('user').innerHTML += '<th id=\"user1\" scope=\"col\"></th>';</script>" << endl;
        cout << "<script>document.getElementById('console').innerHTML += '<td><pre id=\"console1\"></pre></td>';</script>" << endl;
        output("user1",params["h0"] + ":" + params["p0"]);
        user_num = 1;
    }
}

class Client 
{
    public:
        Client(io_service& ioservice,tcp::resolver::query socks_query, string serverhost,string serverport, string target, string filename) : 
        res(ioservice), sock(ioservice) , console(target),query(socks_query) , host(serverhost), port(stoi(serverport)), path("./test_case/" + filename) {}

        void start() 
        {
            string sock_request = "";
            sock_request += (char)4;                                      //sock version (VN)
            sock_request += (char)1;                                      //sock command code(CD) 1:connect 2:bind

            int temp = port / 256;                                        //D_PORT have 2 bytes
            sock_request += (char)(temp > 128 ? temp - 256 : temp);
            temp = port % 256;
            sock_request += (char)(temp > 128 ? temp - 256 : temp);

            sock_request += (char)0;                                      //D_IP
            sock_request += (char)0;
            sock_request += (char)0;
            sock_request += (char)1;

            sock_request += (char)0;                                      //NULL

            sock_request += host;                                        //domain name
            sock_request += (char)0;                                      //NULL

            tcp::resolver::iterator it = res.resolve(query);
            tcp::endpoint endpoint = *it;

            file.open(path, ios::in);
            sock.connect(endpoint);
            async_write(sock, boost::asio::buffer(sock_request),
                        [this](boost::system::error_code ec,size_t)
                        {
                            if(ec) return;
                            get_socks_reply();
                        });
        }

    public:
        int port;
        std::string host;
        
    private:
        void get_socks_reply()
        {
            sock.async_read_some(boost::asio::buffer(read_buffer, 8),
                                [this](boost::system::error_code ec, size_t length)
                                {
                                    if(ec || read_buffer[0] != 0)
                                    {
                                        sock.close();
                                        return;
                                    }
                                    do_read(ec);
                                });
        }
        // void do_connect(tcp::resolver::iterator i)
        // {
        //     sock.async_connect(*i, [this](const boost::system::error_code ec)
        //     {
        //         if(ec) return;
        //     });
        // }
        void do_read(const boost::system::error_code &ec)
        {
            if(!ec)
            {
                memset(read_buffer, 0, sizeof(read_buffer));
                sock.async_read_some(boost::asio::buffer(read_buffer),
                [this](const boost::system::error_code& ec, size_t bytes_transferred)
                {
                    string content(read_buffer);
                    boost::replace_all(content, "\'", "\\'");       //replacing "\'" inplace with "\\'"
                    boost::replace_all(content, "<", "&lt;");       //replacing "<" inplace with "&lt;"   
                    boost::replace_all(content, ">", "&gt;");       //replacing ">" inplace with "&gt;"
                    boost::replace_all(content, "\n", "<br>");      //replacing "\n" inplace with "<br>"
                    output(console, content);
                    if(content.find("%") == string::npos)
                        do_read(ec);
                    else
                        do_write(ec);
                });
            }
            else
                cerr << "read:" << ec.message() << endl;
        }
        void do_write(const boost::system::error_code& ec)
        {
            if(!ec)
            {
                if(!file.eof())
                {
                    string temp,command;
                    getline(file, temp);

                    stringstream s;
                    s.clear();
                    s.str(temp);

                    string token;
                    commands.clear();
                    while(s >> token)
                    {
                        commands.push_back(token);
                    }
                    for(int i = 0;i < commands.size();i++)
                    {
                        command += commands[i];
                        command += " ";
                    }
                    command += "\n";
                    temp = command;
                    boost::replace_all(temp, "\n", "");          //replacing "\n" inplace with ""
                    output_command(console, temp);
                    async_write(sock, boost::asio::buffer(command.c_str(), command.size()),
                    [this](const boost::system::error_code &ec, size_t bytes_transferred)
                    {
                        do_read(ec);
                    });
                }
                else
                {
                    sock.shutdown(tcp::socket::shutdown_receive);   //立即關閉socket,並可以用來喚醒等待線程
                    sock.close();
                }
            }
            else
            {
                cerr << "write:" << ec.message() << endl;
                sock.shutdown(tcp::socket::shutdown_receive);     //立即關閉socket,並可以用來喚醒等待線程
                sock.close();
            }
        }
        
        boost::asio::streambuf buf;
        string console;
        tcp::resolver::query query;
        tcp::resolver res;
        tcp::socket sock;
        ifstream file;
        string path;
        char read_buffer[1024];

};
// vector<Client*> clients;
class Console 
{
    public:
        Console(io_service& ioservice, tcp::resolver::query q) :
        res(ioservice), query(q) {}
        void StartToLink()
        {

            // for(int i = 0;i < (int)clients.size(); ++i){
            //     string sock_request = "";
            //     sock_request += (char)4;
            //     sock_request += (char)1;

            //     int temp = clients[i].port / 256;
            //     sock_request += (char)(temp > 128 ? temp - 256 : temp);
            //     temp = clients[i].port % 256;
            //     sock_request += (char)(temp > 128 ? temp - 256 : temp);

            //     sock_request += (char)0;
            //     sock_request += (char)0;
            //     sock_request += (char)0;
            //     sock_request += (char)1;

            //     sock_request += (char)0;

            //     sock_request += clients[i].host;
            //     sock_request += (char)0;

            //     tcp::resolver::iterator it = res.resolve(query);
            //     tcp::endpoint endpoint = *it;

            //     clients[i].start(sock_request, it);
            // }
        }
    private:
        boost::asio::streambuf buf;
        string console;
        tcp::resolver res;
        tcp::resolver::query query;
        char read_buffer[1024];
};
int main() 
{
    get_para();
    print_html();
    user_check();

    try
    {
        switch(user_num)
        {
            case 1:
            {
                // tcp::resolver::query sq(params["sh"],params["sp"]);
                // Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                // // if(socks){
                // //     tcp::resolver::query sq(params["sh"],params["sp"]);
                // //     Client sc(ioservice, sq);
                // //     sc.start();
                // // }
                // c1.start();
                
                tcp::resolver::query q1(params["h0"],params["p0"]);
                tcp::resolver::query sq(params["sh"],params["sp"]);
                Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                c1.start();
                ioservice.run();
                break;
            }
            case 2:
            {
                // Client c1(ioservice, params["h0"], params["p0"], "console1", params["f0"]);
                // Client c2(ioservice, params["h1"], params["p1"], "console2", params["f1"]);
                // if(socks){
                //     tcp::resolver::query sq(params["sh"],params["sp"]);
                //     Client sc(ioservice, sq);
                //     sc.start();
                // }
                tcp::resolver::query q1(params["h0"],params["p0"]);
                tcp::resolver::query q2(params["h1"],params["p1"]);
                tcp::resolver::query sq(params["sh"],params["sp"]);
                Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                Client c2(ioservice, sq, params["h1"], params["p1"], "console2", params["f1"]);
                c1.start();
                c2.start();
                ioservice.run();
                break;
            }
            case 3:
            {
                // tcp::resolver::query q1(params["h0"],params["p0"]);
                // tcp::resolver::query q2(params["h1"],params["p1"]);
                // tcp::resolver::query q3(params["h2"],params["p2"]);
                // Client c1(ioservice, params["h0"], params["p0"], "console1", params["f0"]);
                // Client c2(ioservice, params["h1"], params["p1"], "console2", params["f1"]);
                // Client c3(ioservice, params["h2"], params["p2"], "console3", params["f2"]);
                // if(socks){
                //     tcp::resolver::query sq(params["sh"],params["sp"]);
                //     Client sc(ioservice, sq);
                //     sc.start();
                // }
                tcp::resolver::query q1(params["h0"],params["p0"]);
                tcp::resolver::query q2(params["h1"],params["p1"]);
                tcp::resolver::query q3(params["h2"],params["p2"]);
                tcp::resolver::query sq(params["sh"],params["sp"]);
                Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                Client c2(ioservice, sq, params["h1"], params["p1"], "console2", params["f1"]);
                Client c3(ioservice, sq, params["h2"], params["p2"], "console3", params["f2"]);
                c1.start();
                c2.start();
                c3.start();
                ioservice.run();
                break;
            }
            case 4:
            {
            //     tcp::resolver::query q1(params["h0"],params["p0"]);
            //     tcp::resolver::query q2(params["h1"],params["p1"]);
            //     tcp::resolver::query q3(params["h2"],params["p2"]);
            //     tcp::resolver::query q4(params["h3"],params["p3"]);
            //     Client c1(ioservice, params["h0"], params["p0"], "console1", params["f0"]);
            //     Client c2(ioservice, params["h1"], params["p1"], "console2", params["f1"]);
            //     Client c3(ioservice, params["h2"], params["p2"], "console3", params["f2"]);
            //     Client c4(ioservice, params["h3"], params["p3"], "console4", params["f3"]);
                // if(socks){
                //     tcp::resolver::query sq(params["sh"],params["sp"]);
                //     Client sc(ioservice, sq);
                //     sc.start();
                // }
                tcp::resolver::query q1(params["h0"],params["p0"]);
                tcp::resolver::query q2(params["h1"],params["p1"]);
                tcp::resolver::query q3(params["h2"],params["p2"]);
                tcp::resolver::query q4(params["h3"],params["p3"]);
                tcp::resolver::query sq(params["sh"],params["sp"]);
                Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                Client c2(ioservice, sq, params["h1"], params["p1"], "console2", params["f1"]);
                Client c3(ioservice, sq, params["h2"], params["p2"], "console3", params["f2"]);
                Client c4(ioservice, sq, params["h3"], params["p3"], "console4", params["f3"]);
                c1.start();
                c2.start();
                c3.start();
                c4.start();
                ioservice.run();
                break;
            }
            case 5:
            {
                tcp::resolver::query q1(params["h0"],params["p0"]);
                tcp::resolver::query q2(params["h1"],params["p1"]);
                tcp::resolver::query q3(params["h2"],params["p2"]);
                tcp::resolver::query q4(params["h3"],params["p3"]);
                tcp::resolver::query q5(params["h4"],params["p4"]);
                tcp::resolver::query sq(params["sh"],params["sp"]);
                Client c1(ioservice, sq, params["h0"], params["p0"], "console1", params["f0"]);
                Client c2(ioservice, sq, params["h1"], params["p1"], "console2", params["f1"]);
                Client c3(ioservice, sq, params["h2"], params["p2"], "console3", params["f2"]);
                Client c4(ioservice, sq, params["h3"], params["p3"], "console4", params["f3"]);
                Client c5(ioservice, sq, params["h4"], params["p4"], "console5", params["f4"]);
                c1.start();
                c2.start();
                c3.start();
                c4.start();
                c5.start();
                ioservice.run();
                break;
            }
        }
    }catch (exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    
    return 0;
}



