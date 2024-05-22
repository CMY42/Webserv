// server.cpp
#include "Server.hpp"
#include "InitializeServer.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <vector>
#include "../../Partie_C/PC_1.hpp"

// Constructeur
Server::Server(const std::string& name, int p, size_t max_body_size, const std::map<int, std::string>& errors, const std::map<std::string, std::map<std::string, std::string> >& rt)
	: server_name(name), port(p), client_max_body_size(max_body_size), error_pages(errors), routes(rt) {}

// Méthode pour démarrer le serveur
void Server::start()
{
	// Création du socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return;
	}

	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		std::cerr << "Error setting socket options" << std::endl;
		close(server_fd);
		return;
	}

	// Définition de l'adresse IP et du port du serveur
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; // Utilise n'importe quelle interface réseau disponible
	address.sin_port = htons(port);

	// Lier le socket à l'adresse et au port spécifiés
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
	{
		std::cerr << "Error binding socket" << std::endl;
		close(server_fd);
		return;
	}

	// Mettre le socket en mode d'écoute pour accepter les connexions entrantes
	if (listen(server_fd, SOMAXCONN) == -1)
	{
		std::cerr << "Error listening on socket" << std::endl;
		close(server_fd);
		return;
	}

	// Stocker le descripteur de fichier du socket d'écoute dans la classe
	_listening_socket = server_fd;

	std::cout << "Server started and listening on port " << port << std::endl;
}

// Méthode pour arrêter le serveur
void Server::stop()
{
	if (_listening_socket != -1)
	{
		close(_listening_socket);
		_listening_socket = -1;
		std::cout << "Server stopped" << std::endl;
	}
	else
	{
		std::cerr << "Server is not running" << std::endl;
	}
}

void Server::handleClient(int client_socket)
{
	std::cout << std::endl << "-------------------- Partie C --------------------" << std::endl << std::endl;
	std::cout << "Client socket: " << client_socket << std::endl;
	std::cout << "Server name: " << this->server_name << std::endl;
	std::cout << "Port: " << this->port << std::endl;
	std::cout << "Client max body size: " << this->client_max_body_size << std::endl;
	std::cout << "Error pages: " << std::endl;
	Part_C part_c(client_socket, this->server_name, this->port, this->client_max_body_size, this->error_pages, this->routes);
}
