#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include "boost_util.h"
using namespace std;

#define INIT 0
#define READING 1
#define WRITING 2


//function def
vector<string> split_line(string input,char* delimeter);
void server_setting(string query_string);
io_service global_io_service;

//global var
int client_num = 0;

string html_content = R"(Content-Type:text/html

<!DOCTYPE html>
<html lang=\"en\">
    <head>
        <meta charset=\"UTF-8\" />
        <title>NP Project 3 Console</title>
        <link
          rel="stylesheet"
          href="https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css"
          integrity="sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO"
          crossorigin="anonymous"
        />
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
    </head>
    <body>
        <table class="table table-dark">
            <tr id="server_title">
            </tr>
            <tr id="console_frame">
            </tr>
        </table>
    </body>
</html>
)";


int main(int argc, const char * argv[]){
    cout << html_content;

    string query_string = getenv("QUERY_STRING");
    server_setting(query_string);
    /*
    while(client_num > 0){
        
    }
    */

}   
/*
void reading_state_handler(server_info cur_server){
    //render data on html, if receive '%', change to write state, if receive 'exit', close connection

}

void writing_state_handler(server_info cur_server){
    //read file and write one line to fd, and change to reading state
}
*/

void server_setting(string query_string){
    vector<string> parameters = split_line(query_string,"&");
    for(vector<string>::iterator it = parameters.begin(); it != parameters.end(); it = it+3){
        string host_name = (*it).substr((*it).find("=") +1);
        if(host_name != ""){
            client_num++;

            string ip = (*it).substr((*it).find("=") +1);
            string port = (*(it+1)).substr((*(it+1)).find("=") +1);
            string index = (*it).substr(0,(*it).find("="));
            string fileName = "test_case/"+(*(it+2)).substr((*(it+2)).find("=") +1);
            FILE *fp = fopen(fileName.c_str(),"r");
            int fno = fileno(fp);


            //<td>nplinux1</td>
            cout << "<script>$('#server_title').append(\"<td>" << ip << "</td>\");</script>";
            //<td id='h0'></td>
            cout << "<script>$('#console_frame').append(\"<td id=\'" << index << "\'></td>\");</script>";
            
            shared_ptr<shellSession> cur_session(new shellSession(global_io_service, ip, port, index, fno));
        }
        
    }
    global_io_service.run();
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