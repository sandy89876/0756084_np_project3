#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <boost/asio.hpp>
using namespace std;

#define INIT 0
#define READING 1
#define WRITING 2

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
                <td>hhh</td>
            </tr>
            <tr id="console_frame">
                <td>ddd</td>
            </tr>
        </table>
    </body>
</html>
)";


int main(int argc, const char * argv[]){
    cout << html_content;

    string query_string = getenv("QUERY_STRING");
    
}   
