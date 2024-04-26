#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <map>

class Server
{
private:
	std::string server_name;
	int port;
	size_t client_max_body_size;
	std::map<int, std::string> error_pages;
	std::map<std::string, std::map<std::string, std::string> > routes;
	int _listening_socket;

public:
	Server() {}
	Server(const std::string& name, int p, size_t max_body_size, const std::map<int, std::string>& errors, const std::map<std::string, std::map<std::string, std::string> >& rt);

	std::string getName() const { return server_name; }
	int getPort() const { return port; }
	size_t getMaxBodySize() const { return client_max_body_size; }

	// Méthodes pour définir les membres privés
	void setName(const std::string& name) { server_name = name; }
	void setPort(int p) { port = p; }
	void setMaxBodySize(size_t max_body_size) { client_max_body_size = max_body_size; }
	void setErrorPages(const std::map<int, std::string>& errors) { error_pages = errors; }
	void setRoutes(const std::map<std::string, std::map<std::string, std::string> >& rt) { routes = rt; }

	// Autres méthodes de la classe Server
	void start();
	void stop();
	void handleConnections();
	void handleClient(int client_socket);
};

#endif

