#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#define INIT 0
#define READING 1
#define WRITING 2


class shellSession: enable_shared_from_this<shellSession>{
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
        cout << "<script>$('#" << index << "').innerHTML += \" session construct \";</script>";
        start();
    }
    void start(){
        cout << "<script>$('#" << index << "').innerHTML += \" start: \";</script>";
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
        cout << "<script>$('#" << index << "').innerHTML += \" before do_resolve self \";</script>";
        // auto self(get_ptr());
        cout << "<script>$('#" << index << "').innerHTML += \" do_resolve called \";</script>";
        tcp::resolver::query query(ip, port);
        _resolver.async_resolve(query, [this](boost::system::error_code ec, tcp::resolver::iterator it){
            do_connect(ec, it);
        });

      // async_resolve(tcp::resolver::query query(ip, port), do_connect);
    }
    void do_connect(const boost::system::error_code &ec, tcp::resolver::iterator it){
        // auto self(shared_from_this());
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        cout << "<script>$('#" << index << "').innerHTML += \" do_connect called \";</script>";
        if(!ec){
            tcp::endpoint ep = *it;
            _socket.async_connect(ep, [this](boost::system::error_code ec){
                do_read(ec);
            });
        }
    }
    void do_read(boost::system::error_code ec){
        // auto self(shared_from_this());
        if(!ec){
            cout << "<script>$('#" << index << "').innerHTML += \" do_read called \";</script>";
            
            // The connection was successful
            _socket.async_read_some(buffer(_data),[this](boost::system::error_code ec, size_t length) {
                read_handler(ec, length);
            });
        }
        

    }
    void do_send(){
        // auto self(shared_from_this());
      /*
      if(){

      }
      */

    }
    void read_handler(const boost::system::error_code &ec,size_t bytes_transferred){
        // auto self(shared_from_this());
        if (!ec){
            string tmp(_data.begin(), _data.end());
            int i = tmp.find("\n", 0);
            msg = tmp.substr(0,i);

            cout << "<script>$('#" << index << "').innerHTML += \"" << msg << "\";</script>";
            cout << "<script>$('#" << index << "').innerHTML += \" do_send called \";</script>";
            do_send();

            
        }
    }

};
