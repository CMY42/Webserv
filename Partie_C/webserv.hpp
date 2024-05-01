#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <string>
#include <fstream>

#include <sstream>
#include <algorithm>
#include <map>
#include <vector>

#include <fcntl.h>

#include <sys/wait.h>
#include <limits.h>

#include "PC_1.hpp"


// Simule une structure de configuration simple
/*struct ServerConfig
{
    int port = 8080;
    long unsigned int max_body_size = 9999;
    
    std::string basePath = "./the_ultimate_webserv";
    std::string index = "/index.html";
};*/

// structure du serveur VRé
typedef struct s_server2
{
	std::string													server_name;
	int															port;
	size_t														client_max_body_size;
	std::map<int, std::string> 									error_pages;
	std::map<std::string, std::map<std::string, std::string> >	routes;
}	t_server2;

//void main_C(int client_socket, s_server& config);

//  http://localhost:8080
//  g++ -o server test.cpp
//  ./server 

#endif