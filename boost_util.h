#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

string format_output(string line){
    int i = line.find("\r");
    if(i != -1){
        line = line.substr(0,i);
    }
    string tmp =R"(\")";
    boost::replace_all(line, "\"", tmp);
    // boost::replace_all(line, "<", "&lt;");
    // boost::replace_all(line, ">", "&gt;");
    return line;
}

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
    fstream file;

    tcp::resolver _resolver;
    tcp::socket _socket;
    array<char, 10240> _data;

  public:
    shellSession(io_service& global_io_service, string i, string p, string ind, string f_name)
    : _socket(global_io_service), _resolver(global_io_service){
        ip = i;
        port = p;
        index = ind;

        file.open(f_name, ios::in);
        if(!file){
            cout << "<script>console.log(\"cannot open file\");</script>"; 
        }

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
            
            _data = {{}};
            // The connection was successful
            _socket.async_read_some(buffer(_data),[this,self](boost::system::error_code ec, size_t length) {
                if (!ec){
                    string tmp(_data.begin(), _data.end());
                    // cout << "<script>console.log(\"receive:" << tmp <<"\");</script>";
                    vector<string> msg_lines = split_line(tmp,"\n");
                    for(vector<string>::iterator it = msg_lines.begin(); it != msg_lines.end(); ++it){
                        if((*it).find("\r\r") != 0){
                            if((*it) != "% "){
                                string tmp = format_output(*it);
                                cout << "<script>$('#" << index << "\').text($('#" << index << "\').text() + \"" << tmp << "\\n\");</script>";
                            }else{
                                cout << "<script>$('#" << index << "\').text($('#" << index << "\').text() + \"" << (*it) << "\");</script>";
                            }
                        }
                    }
                    if(tmp.find("% ") != -1){
                        do_send();
                    }else if(tmp.find("exit") != -1){
                        //leave
                    }
                    do_read(ec);
                    
                }
            });
        }
    }
    void do_send(){
        auto self(shared_from_this());

        string cmd;

        getline(file, cmd);
        int i = cmd.find("\r");
        if(i != -1){
            cmd = cmd.substr(0,i);
        }
        cout << "<script>$('#" << index << "\').text($('#" << index << "\').text() + \"" << cmd << "\\n\");</script>";
        cmd += "\n";
        _socket.async_send(buffer(cmd),[this, self](boost::system::error_code ec, std::size_t length) {
            if(!ec){
                do_read(ec);
            }
            
        });
        

    }

};
