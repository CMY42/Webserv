#include "PC_1.hpp"


template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// Fonction pour traiter une requête HTTP reçue et renvoyer une réponse
Part_C::Part_C(int client_socket, std::string server_name, int port, size_t client_max_body_size, std::map<int, std::string> error_pages, std::map<std::string, std::map<std::string, std::string> > routes)
{
    s_server2 config;
    config.server_name = server_name;
    config.port = port;
    config.client_max_body_size = client_max_body_size;
    config.error_pages = error_pages;
    config.routes = routes;

    //init test
    config.server_name = "sitetest.com";
    config.port = 3003;
    config.client_max_body_size = 20480;

    config.error_pages[404] = "webpage/error_pages/error404.html";
    config.error_pages[405] = "webpage/error_pages/error405.html";

    std::map<std::string, std::string> details1;
    details1["index"] = "index.html";
    details1["methods"] = "GET POST";
    details1["root"] = "./webpage/sitetest/index.html";
    config.routes["/"] = details1;

    std::map<std::string, std::string> details2;
    details2["methods"] = "GET";
    details2["redirection"] = "https://signin.intra.42.fr";
    config.routes["intra42"] = details2;



    init();
    status = 200;

    size_t lastSlashPos = config.routes["/"]["root"].find_last_of("/");
    basePath = config.routes["/"]["root"].substr(0, lastSlashPos);

    //-------------------- Partie Request --------------------
    std::cout << "test\n";

    const int bufferSize = 1024;
    char request_buffer[bufferSize] = {0};

    // Lecture de la requête du client
    int bytesReceived = read(client_socket, request_buffer, bufferSize - 1);
    if (bytesReceived < 1)
    {
        std::cout << "Erreur de lecture ou connexion fermée par le client." << std::endl;
        return;
    }


    std::cout << std::endl << std::endl << "-------------------> Request" << std::endl << request_buffer << std::endl;

    try
    {
        parse(request_buffer, config);
        print_parse();

    //-------------------- Partie Execution --------------------

        if(isCGI())
            execute_cgi();
        else if(method == "GET")
            method_GET();
        else if(method == "POST")
            method_POST();
        else if(method == "DELETE")
            method_DELETE();

    }

    catch(InvalidRequestException &e)
    {
        std::cout << "-----> Error ! It seem to be a : " << status << '\n';
    }

    //-------------------- Partie Response --------------------

     // Utilisez basePath de la configuration pour trouver les fichiers
    std::string requestURI = uri == "/" ? config.routes["/"]["root"] : basePath + uri;
    std::string filePath = requestURI;
    std::ifstream fileStream(filePath.c_str());
    std::string httpResponse;

    std::cout << requestURI << std::endl;

    if (method == "GET")
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

    if (isCGI())
        httpResponse = "HTTP/1.1 " + toString(status) + " " + _statusCodes[status] + "\r\n" +
           "Content-Length: " + toString(cgi_content.length()) + "\r\n" +
           "Content-Type: text/html; charset=UTF-8\r\n" +  // Assurez-vous d'inclure Content-Type si nécessaire
           "\r\n" +
           cgi_content;
    else
        httpResponse = "HTTP/1.1 " + toString(status) + " " + _statusCodes[status] + "\r\n" +
               "Content-Type: " + contentType + "\r\n" +
               "Content-Length: " + toString(content.length()) + "\r\n" +
               //"Connection: close\r\n" +
               "\r\n" +
               content;

    
    //httpResponse = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><head><title>Date et Heure</title></head><body>\n<h1>Date et Heure Actuelles</h1>\n<p>Mon Jul 12 12:34:56 2021</p>\n</body></html>\n";

    std::cout << std::endl << std::endl << "-------------------> Response" << std::endl << httpResponse << std::endl;

    // Envoi de la réponse au client
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
            content = toString(status) + " : " + _statusCodes[status];
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
        contentType = "text/plain"; // Type par défaut
}

