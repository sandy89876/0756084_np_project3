#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
using namespace std;


struct server_info{
    string ip;
    int port;
    string file_name;
};

//function def
vector<string> split_line(string input,char* delimeter);
void parse_query_string(string query_string);


//global var
vector<server_info> server_list;
string html_content = R"(
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
    cout << "Content-type:text/html\n\n";
    cout << html_content;
    fflush(stdout);
    string query_string = getenv("QUERY_STRING");
    parse_query_string(query_string);
    // cout << "<div>" << server_list.begin()->ip << "</div>";
    // for(vector<server_info>::iterator it = server_list.begin(); it != server_list.end(); ++it){
    //     string tmp = it->ip+", "+to_string(it->port)+", "+it->file_name;
    //     // cout << "<script>document.getElementById('all').innerHTML +=\"" << it->ip << ", " << it->port << ", " << it->file_name << "\";</script>";
    //     cout << "<script>document.getElementById('v1').innerHTML += \"" <<tmp<< "\";</script>";
    //     fflush(stdout);
    // }

}   

void parse_query_string(string query_string){
    vector<string> parameters = split_line(query_string,"&");
    for(vector<string>::iterator it = parameters.begin(); it != parameters.end(); it = it+3){
        string host_name = (*it).substr((*it).find("=") +1);
        if(host_name != ""){
            server_info cur_server;

            cur_server.ip = (*it).substr((*it).find("=") +1);
            string td_ele = "<td>"+cur_server.ip+"</td>";
            cout << "<script>$('#server_title').append(\"<td>" << cur_server.ip << "</td>\");</script>";
            cout << "<script>$('#console_frame').append(\"<td id=\'" << (*it).substr(0,(*it).find("=")) << "\'></td>\");</script>";
            cur_server.port = stoi((*(it+1)).substr((*(it+1)).find("=") +1));
            cur_server.file_name = (*(it+2)).substr((*(it+2)).find("=") +1);
            server_list.push_back(cur_server);
        }
        
    }
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