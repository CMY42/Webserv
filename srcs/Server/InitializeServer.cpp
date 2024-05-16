#include "InitializeServer.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>

#include "InitializeServer.hpp"
#include "Server.hpp"
#include <iostream>
#include <vector>

void initializeServer(Server& server, const t_server& parsedServer)
{
	// Méthodes définies dans la classe Server pour initialiser ses membres
	server.setName(parsedServer.server_name);
	server.setPort(parsedServer.port);
	server.setMaxBodySize(parsedServer.client_max_body_size);
	server.setErrorPages(parsedServer.error_pages);
	server.setRoutes(parsedServer.routes);

	// Affichage des paramètres du serveur (à supprimer dans la version finale !!!!!!!!!)
	std::cout << "Server Name: " << server.getName() << std::endl;
	std::cout << "Port: " << server.getPort() << std::endl;
	std::cout << "Client Max Body Size: " << server.getMaxBodySize() << std::endl;
}



