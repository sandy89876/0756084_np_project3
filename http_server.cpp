#include <array>
#include <boost/asio.hpp>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
using namespace boost::asio;

io_service global_io_service;

vector<string> split_line(string input,char* delimeter){
    char *comm = new char[input.length()+1];
    strcpy(comm, input.c_str());
    
    char* token = strtok(comm, delimeter);
    vector<string> result;
    while(token != NULL){
        result.push_back(token);
        token = strtok(NULL, delimeter);
    }
    return result;
}


class EchoSession : public enable_shared_from_this<EchoSession> {
    private:
       enum { max_length = 1024 };
        ip::tcp::socket _socket;
        array<char, max_length> _data;

        string request_uri; // /xxx.cgi
        string query_string; //h0=...
        string execute_file_path;

    public:
        EchoSession(ip::tcp::socket socket) : _socket(move(socket)) {
            query_string="";
        }

        void start(){
            cout << "start called" << endl;
            // create_child();
            read_request();
        }

    private:

        void read_request() {
            cout << "do_read called" << endl;
            auto self(shared_from_this());
            _socket.async_read_some(buffer(_data, max_length),[this, self](boost::system::error_code ec, std::size_t length) {
                if(!ec){
                    string tmp(_data.begin(),_data.end());
                    cout << "tmp=" << tmp << endl; 
                    parse_request(tmp);
                    create_child();
                }
            });
            
        }

        void parse_request(string request){
            vector<string> http_headers = split_line(request, "\r\n");
            for(vector<string>::iterator it = http_headers.begin(); it != http_headers.end(); ++it){
                cout << "" << *it << "!" << endl;     
            }

            string tmp = (split_line(http_headers[0], " "))[1];
            if(tmp.find("?") != string::npos){
                int index = tmp.find("?"); 
                request_uri = tmp.substr(0, index);
                query_string = tmp.substr(index+1);
                cout << "request_uri= " << request_uri << " query_string= " << query_string << endl;
            }else{
                request_uri = tmp;
                cout << "request_uri= " << request_uri << endl;
            }
            char path[100];
            getcwd(path, sizeof(path));
            string working_path(path);
            cout << "working_path = " << working_path << endl;
            execute_file_path = working_path + request_uri.substr(1);

        }
        // void do_write(std::size_t length) {
        //     auto self(shared_from_this());
        //     _socket.async_send(buffer(_data, length),[this, self](boost::system::error_code ec, std::size_t /* length */) {
        //         if (!ec) do_read();
        //     });
        // }

        void set_cgi_env(){
            setenv("REQUEST_METHOD", "GET", 1);
            setenv("REQUEST_URI", request_uri.c_str(), 1);
            setenv("QUERY_STRING", query_string.c_str(), 1);
            setenv("SERVER_PROTOCOL", "", 1);
            setenv("HTTP_HOST", "", 1);
            setenv("SERVER_ADDR", "", 1);
            setenv("SERVER_PORT", "", 1);
            setenv("REMOTE_ADDR", "", 1);
            setenv("REMOTE_PORT", "", 1);
        }



        void create_child(){

            // cout << "bef fork _socket=" << _socket << endl;
            cout << "create_child called" << endl;

            int child_pid = fork();
            if(child_pid < 0){
                cout << "fork error" << endl;
            }else if(child_pid == 0){
                //child process, execute cgi
                auto self(shared_from_this());
                cout << "childddd" << endl;

                set_cgi_env();
                dup2(_socket.native_handle(), STDOUT_FILENO);
                cout << "HTTP/1.1 200 OK" << endl;
                execlp(execute_file_path.c_str(), NULL);

            }else{
                waitpid(-1, NULL, WNOHANG);
            }
        }
};

class EchoServer {
    private:
        ip::tcp::acceptor _acceptor;
        ip::tcp::socket _socket;

    public:
        EchoServer(short port)
        : _acceptor(global_io_service, ip::tcp::endpoint(ip::tcp::v4(), port)),
        _socket(global_io_service) {
            do_accept();
        }

    private:
        void do_accept(){
            _acceptor.async_accept(_socket, [this](boost::system::error_code ec) {
                if (!ec){
                    make_shared<EchoSession>(move(_socket))->start();
                }
            cout << "do_accept called" << endl;
            // cout << "_socket=" << _socket << endl;
            do_accept();
            });
        }
};

int main(int argc, char* const argv[]) {
    if (argc != 2) {
        std::cerr << "Usage:" << argv[0] << " [port]" << endl;
        return 1;
    }

    try{
        short port = atoi(argv[1]);
        EchoServer server(port);
        global_io_service.run();
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}