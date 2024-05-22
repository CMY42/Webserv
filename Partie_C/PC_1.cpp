#include "PC_1.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>

template<typename T>
std::string toString(const T& value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

Part_C::Part_C(int client_socket, std::string server_name, int port, size_t client_max_body_size, std::map<int, std::string> error_pages, std::map<std::string, std::map<std::string, std::string> > routes)
{
	s_server2 config;
	config.server_name = server_name;
	config.port = port;
	config.client_max_body_size = client_max_body_size;
	config.error_pages = error_pages;
	config.routes = routes;

	init();
	status = 200;

	basePath = config.routes["/"]["root"];

	std::cout << "----> BASEPATH : " << basePath <<"\n";

	const int bufferSize = 30000;
	char request_buffer[bufferSize] = {0};

	int bytesReceived = 0;
	int totalBytesRead = 0;
	memset(request_buffer, 0, bufferSize);

	while (totalBytesRead < bufferSize - 1)
	{
		bytesReceived = read(client_socket, request_buffer + totalBytesRead, bufferSize - 1 - totalBytesRead);
		if (bytesReceived > 0)
		{
			totalBytesRead += bytesReceived;
			request_buffer[totalBytesRead] = '\0';

			if (strstr(request_buffer, "\r\n\r\n") != NULL)
			{
				break;
			}
		}
		else if (bytesReceived == 0)
		{
			std::cout << "La connexion a été fermée par le client." << std::endl;
			return;
		}
		else
		{
			std::cerr << "Erreur de lecture." << std::endl;
			return;
		}
	}

	if (bytesReceived <= 0)
	{
		std::cerr << "Aucune donnée valide reçue ou connexion fermée prématurément." << std::endl;
		return;
	}

	if (strstr(request_buffer, "\r\n\r\n") == NULL)
	{
		std::cerr << "Les headers HTTP complets n'ont pas été reçus, même si les données ont été lues." << std::endl;
	}

	std::cout << std::endl << std::endl << "-------------------> Request" << std::endl << request_buffer << std::endl;
	std::cout << std::endl << std::endl << "-------------------> Request END\n\n";

	try
	{
		parse(request_buffer, config);
		print_parse();

		if(method == "DELETE")
			method_DELETE();
		else if(isCGI())
			execute_cgi();
		else if(method == "GET" )
			method_GET();
		else if(method == "POST")
			method_POST();
	}
	catch(InvalidRequestException &e)
	{
		std::cout << "-----> Error ! It seem to be a : " << status << '\n';
	}

	std::string requestURI = uri == "/" ? config.routes["/"]["root"] + "/" + config.routes["/"]["index"] : basePath + uri;
	std::string filePath = requestURI;
	std::ifstream fileStream(filePath.c_str());
	std::string httpResponse;

	std::cout << requestURI << std::endl;

	if (method == "GET" && status < 300)
	{
		if (!fileStream.is_open())
		{
			status = 404;
		}

		getContentType(filePath);

		std::stringstream response_buffer;
		response_buffer << fileStream.rdbuf();
		content = response_buffer.str();
	}

	final_status(config);

	if (isCGI() && (status == 200 || status == 201))
		httpResponse = "HTTP/1.1 " + toString(status) + " " + _statusCodes[status] + "\r\n" +
			"Content-Length: " + toString(cgi_content.length()) + "\r\n" +
			"Content-Type: text/html; charset=UTF-8\r\n" +
			"\r\n" +
			cgi_content;
	else
		httpResponse = "HTTP/1.1 " + toString(status) + " " + _statusCodes[status] + "\r\n" +
				"Content-Type: " + contentType + "\r\n" +
				"Content-Length: " + toString(content.length()) + "\r\n" +
				"\r\n" +
			content;

	std::cout << std::endl << std::endl << "-------------------> Response" << std::endl << httpResponse << std::endl;

	send(client_socket, httpResponse.c_str(), httpResponse.length(), 0);
}

void Part_C::final_status(s_server2& config)
{
	if (status != 200 && status != 201)
	{
		if (status == 404 || status == 405)
		{
			std::string filePath = "./" + config.error_pages[status];
			std::ifstream fileStream(filePath.c_str());

			getContentType(filePath);

			std::stringstream response_buffer;
			response_buffer << fileStream.rdbuf();
			content = response_buffer.str();
		}
		else
		{
			contentType = "text/plain";
			content = toString(status) + " : " + _statusCodes[status] + "\nyep its custom mazafaka (from Zalius, pls dont hurt us we are good friend :D)";
		}
	}
}

void Part_C::init()
{
	_statusCodes[200] = "OK";
	_statusCodes[201] = "Created";
	_statusCodes[202] = "Accepted";
	_statusCodes[204] = "No Content";
	_statusCodes[301] = "Moved Permanently";
	_statusCodes[302] = "Found";
	_statusCodes[304] = "Not Modified";
	_statusCodes[400] = "Bad Request";
	_statusCodes[401] = "Unauthorized";
	_statusCodes[403] = "Forbidden";
	_statusCodes[404] = "Not Found";
	_statusCodes[405] = "Method Not Allowed";
	_statusCodes[409] = "Conflict";
	_statusCodes[413] = "Payload Too Large";
	_statusCodes[415] = "Unsupported Media Type";
	_statusCodes[500] = "Internal Server Error";
	_statusCodes[501] = "Not Implemented";
	_statusCodes[505] = "HTTP Version Not Supported";
	_statusCodes[508] = "Infinite Loop Detected";
}

void Part_C::getContentType(const std::string& filePath)
{
	if (filePath.rfind(".html") == filePath.length() - 5)
		contentType = "text/html";
	else if (filePath.rfind(".css") == filePath.length() - 4)
		contentType = "text/css";
	else if (filePath.rfind(".js") == filePath.length() - 3)
		contentType = "application/javascript";
	else if (filePath.rfind(".png") == filePath.length() - 4)
		contentType = "image/png";
	else if (filePath.rfind(".jpg") == filePath.length() - 4 || filePath.rfind(".jpeg") == filePath.length() - 5)
		contentType = "image/jpeg";
	else
		contentType = "text/plain";
}
