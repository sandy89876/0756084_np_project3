#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#define INIT 0
#define READING 1
#define WRITING 2

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

class shellSession: public enable_shared_from_this<shellSession>{
  private:
    string ip;
    string port;
    string index;//h0
    int fno;
    int status;

    tcp::resolver _resolver;
    tcp::socket _socket;
    array<char, 1024> _data;
    string msg;

  public:
    shellSession(io_service& global_io_service, string i, string p, string ind, int f_no)
    : _socket(global_io_service), _resolver(global_io_service){
        ip = i;
        port = p;
        index = ind;
        fno = f_no;
        status = INIT;
        cout << "<script>console.log(\"shellSession construct\");</script>";
    }
    void start(){
        cout << "<script>console.log(\"start called\");</script>";
        do_resolve();
    }
    shared_ptr<shellSession> get_ptr(){
        return shared_from_this();
    }

    string get_ip(){
        return ip;
    }
    string get_index(){
        return index;
    }
  private:
    void do_resolve(){   
        auto self = shared_from_this();
        cout << "<script>console.log(\"do_resolve called\");</script>";
        tcp::resolver::query query(ip, port);
        _resolver.async_resolve(query, [this,self](boost::system::error_code ec, tcp::resolver::iterator it){
            do_connect(ec, it);
        });

      // async_resolve(tcp::resolver::query query(ip, port), do_connect);
    }
    void do_connect(const boost::system::error_code &ec, tcp::resolver::iterator it){
        auto self(shared_from_this());
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        cout << "<script>console.log(\"do_connect called\");</script>";
        if(!ec){
            tcp::endpoint ep = *it;
            _socket.async_connect(ep, [this,self](boost::system::error_code ec){
                do_read(ec);
            });
        }
    }
    void do_read(boost::system::error_code ec){
        auto self(shared_from_this());
        if(!ec){
            cout << "<script>console.log(\"do_read called\");</script>";
            
            // The connection was successful
            _socket.async_read_some(buffer(_data),[this,self](boost::system::error_code ec, size_t length) {
                if (!ec){
                    string tmp(_data.begin(), _data.end());

                    vector<string> msg_lines = split_line(tmp,"\n");
                    for(vector<string>::iterator it = msg_lines.begin(); it != msg_lines.end(); ++it){
                        if((*it).find("\r") != 0){
                            cout << "<script>$('#" << index << "\').text($('#" << index << "\').text() + \"" << (*it) << "\\n\");</script>";
                            if((*it) == "% "){
                                //do send
                                do_send();
                            }else if((*it) == "exit"){
                                //leave
                            }else{
                                //keep read(?)
                            }
                        }
                    }

                }
            });
        }
        

    }
    void do_send(){
      /*
      if(){

      }
      */

    }

};
