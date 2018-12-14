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

        //cgi environment variables
        string request_uri; // /xxx.cgi
        string query_string; //h0=...
        string http_host;
        string server_addr;
        string server_port;
        string remote_addr;
        string remote_port;

        string execute_file_path;

    public:
        EchoSession(ip::tcp::socket socket) : _socket(move(socket)) {
            query_string="";
            server_addr = _socket.local_endpoint().address().to_string();
            server_port = to_string(_socket.local_endpoint().port());
            remote_addr = _socket.remote_endpoint().address().to_string();
            remote_port = to_string(_socket.remote_endpoint().port());

        }

        void start(){
            cout << "start called" << endl;
            catch_request();
        }

    private:
        void catch_request() {
            cout << "do_read called" << endl;
            auto self(shared_from_this());
            _socket.async_read_some(buffer(_data, max_length),[this, self](boost::system::error_code ec, std::size_t length) {
                if(!ec){
                    string tmp(_data.begin(),_data.end());
                    cout << "http request:" << endl << tmp << endl; 
                    parse_request(tmp);
                    create_child();
                }
            });
            
        }

        void parse_request(string request){
            vector<string> http_headers = split_line(request, "\r\n");
            for(vector<string>::iterator it = http_headers.begin(); it != http_headers.end(); ++it){
                cout << *it << "..." << endl;     
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

            tmp = (split_line(http_headers[1], " "))[1];
            http_host = tmp.substr(0, tmp.find(":"));

            char path[100];
            getcwd(path, sizeof(path));
            string working_path(path);
            cout << "working_path = " << working_path << endl;
            execute_file_path = working_path + "/" + request_uri.substr(1);
            cout << "execute_file_path = " << execute_file_path << endl;
        }

        void do_write() {
            auto self(shared_from_this());
            _socket.async_send(buffer(_data, max_length),[this, self](boost::system::error_code ec, size_t  transfered_length ) {
            });
        }

        void set_cgi_env(){
            setenv("REQUEST_METHOD", "GET", 1);
            setenv("REQUEST_URI", request_uri.c_str(), 1);
            setenv("QUERY_STRING", query_string.c_str(), 1);
            setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
            setenv("HTTP_HOST", http_host.c_str(), 1);
            setenv("SERVER_ADDR", server_addr.c_str(), 1);
            setenv("SERVER_PORT", server_port.c_str(), 1);
            setenv("REMOTE_ADDR", remote_addr.c_str(), 1);
            setenv("REMOTE_PORT", remote_port.c_str(), 1);
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
                cout << "before write" << endl;
                _data = {"HTTP/1.1 200 OK\r\n"};
                do_write();
                cout << "after write" << endl;
                dup2(_socket.native_handle(), STDOUT_FILENO);
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