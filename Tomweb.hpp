#ifndef TOMWEB_HPP
# define TOMWEB_HPP

#define BUFFER_SIZE 1024
#define PORT 8081
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <algorithm>
#include <vector>
#include <map>


#define ERROR204 "www/errors/204.html"
#define ERROR400 "www/errors/400.html"
#define ERROR401 "www/errors/401.html"
#define ERROR402 "www/errors/402.html"
#define ERROR403 "www/errors/403.html"
#define ERROR404 "www/errors/404.html"
#define ERROR405 "www/errors/405.html"
#define ERROR408 "www/errors/408.html"
#define ERROR409 "www/errors/409.html"
#define ERROR411 "www/errors/411.html"


struct location
{
    std::string URI;
    std::vector<std::string> allowed;
    std::string root;
    bool autoindex;
    std::vector<std::string> indexes;
    std::vector<std::string> cgi_path;
    std::vector<std::string> cgi_ex;
    int doesntExist;
    std::string returning;
};

struct server_t
{
    int host;
    int port;
    std::string servername;
    std::map<int, std::string> errorpages;
    int client_max_body_size;
    std::vector<location> locations;
};

class Server;

class Connection {
    private:

    public:
        Connection(int socket_fd);
        ~Connection();
        int                                 port;
        int                                 IsAfterResponseClose;
        int                                 socket_fd;
        int                                 isReadingHeader;
        int                                 isWriting; //write back to socket.
        int                                 fdWritingTo;
        int                                 fdReadingFrom; // haven't
        int                                 doesClientClosed;
        int                                 contentLength;
        int                                 autoIndex; // haven't done?

        std::string                         host;
        std::string                         contentType;
        std::string                         contentDisposition;
        std::string                         boundary;
        std::string                         form_name;
        std::string                         file_name;

        std::string         method;
        std::string         URI;
        std::string         HTTP_version;

        int                 errNbr;
        int                 circleTotal;
        int                 circleRead;
        int                 circleWrite;

        std::string         have_read;
    
        void    reset();
};

class Server
{
    private:

    public:
        ~Server();
        Server(server_t& s);
        std::vector<Connection>                         connections;
        std::vector<int>                                to_add_fds;
        std::vector<std::vector<location> >             locations;
        std::vector<std::string>                        server_names;
        struct sockaddr_in                              address;
        std::map<int, std::string>                      errorPages;
        int                                             err;
        int                                             serverFd;
        int                                             body_size_max;
        int                                             port;
        int                                             host;
        std::string get_error_page(int numb);
        std::string return_default(int numb);
        void accept();
};

int parse(std::string path, std::vector<server_t>& s);
bool checkIfFileExistsAndNotDirectory(std::string& path);
location get_location(std::vector<location>& locations, std::string& path);
std::vector<std::string> get_allowed(location& loc);
bool isAllowed(location& loc, std::string& method);
std::string get_path_of_standart_error(int errorNumb);
std::string get_error_page(Server& serv, int errorNumb);
std::string get_path_to_file(location& loc, std::string path);
bool isDirectory(const char *path);
long long my_atoi(std::string numb);
bool isNumber(std::string& number);
void handle_URI(std::string& URI);
bool checkIfFileExistsAndNotDirectory(std::string& path);

int	open_file(Connection &current_connection);
int	body_handle_post(Connection &current_connection);
int	connection_delete(int &fd_to_del, std::vector<Server> &servers);
int	connections_delete(std::vector<int> &to_del_fds, std::vector<Server> &servers);
void	connection_accept(Server &server);
void connections_add(std::vector<Server> &servers);
int	extract_IsAfterResponseClose(Connection &current_connection, std::string &header_o);
int	extract_contentLength(Connection &current_connection, std::string &header_o);
int	extract_contentType(Connection &current_connection, std::string &header_o);
int	extract_host(Connection &current_connection, std::string &header_o);
int	extract_boundary(Connection &current_connection, std::string &header_o);
int	header_extract(Connection &current_connection, std::string &header_o);
int	request_line(Server &server, Connection &current_connection);
int	request_header(Server &server, Connection &current_connection);
int	set_errNbr(Connection &current_connection, int nbr,
			int isReadingHeader, int isReadingBody, int isWriting);
int	socket_read(Connection &current_connection, std::vector<int> &to_del);
int	connection_read(Server &server, Connection &current_connection,
                std::vector<int> &to_del);
void	load_config_n_socket_create(int ac, char **av, std::vector<Server> &servers, int &max_fd);
std::string	get_status_code(int nbr);
void	load_config(int ac, char **av, std::vector<server_t> &server_config);
int	connection_write(Server &server, Connection &current_connection, std::vector<int> &to_del);


int deleter(Connection &current_connection);

std::string handle_0(void);
std::string	handle_204(void);
std::string	handle_400(void);
std::string	handle_401(void);
std::string	handle_403(void);
std::string	handle_404(void);
std::string	handle_408(void);
std::string	handle_409(void);
std::string	handle_411(void);
std::string	handle_00(void);
#endif