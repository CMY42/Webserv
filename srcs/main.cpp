#include <iostream>
#include "colors.hpp"
#include "Configuration/ConfigCheck.hpp"
#include "Configuration/ConfigParse.hpp"
#include "Server/InitializeServer.hpp"
#include "Server/Server.hpp"

void print_struct_vals(const std::vector<t_server>& servers)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		std::cout << ">" << servers[i].server_name << "<" << std::endl << std::endl;
		std::cout << ">" << servers[i].port << "<" << std::endl << std::endl;
		std::cout << ">" << servers[i].client_max_body_size << "<" << std::endl << std::endl;

		for (std::map<int, std::string>::const_iterator error_page_it = servers[i].error_pages.begin(); error_page_it != servers[i].error_pages.end(); ++error_page_it)
			std::cout << ">" << error_page_it->first << "< >" << error_page_it->second << "<" << std::endl;
		std::cout << std::endl;

		for (std::map<std::string, std::map<std::string, std::string> >::const_iterator route_it = servers[i].routes.begin(); route_it != servers[i].routes.end(); ++route_it)
		{
			std::cout << ">" << route_it->first << "<" << std::endl;
			const std::map<std::string, std::string>& sub_routes = route_it->second;
			for (std::map<std::string, std::string>::const_iterator sub_route_it = sub_routes.begin(); sub_route_it != sub_routes.end(); ++sub_route_it)
				std::cout << ">" << sub_route_it->first << "< >" << sub_route_it->second << "<" << std::endl;
			std::cout << std::endl;
		}
		std::cout << std::endl << "........................." << std::endl << std::endl;
	}
}

int main(int argc, char *argv[])
{
	std::string config_file;

	if (argc == 1)
		config_file = "config.yaml";
	else if (argc == 2)
		config_file = argv[1];
	else
	{
		std::cout << "Invalid number of arguments, please use: ./webserv [config_file]" << std::endl;
		return 1;
	}
	try
	{
		ConfigCheck		config(config_file);
		ConfigParse		parse(config);
		std::cout << "--------------------------------------------------" << std::endl << std::endl;
		print_struct_vals(parse.getServersParsed());
		std::cout << "--------------------------------------------------" << std::endl << std::endl;
		// Création d'une instance de serveur
		Server server;

		// Initialisation du serveur avec les données parsées du fichier de configuration
		initializeServer(server, parse.getServersParsed());

		// Démarrage du serveur
		server.start();

		// Gestion des connexions entrantes
		server.handleConnections();

		// Arrêt du serveur
		server.stop();
	}
	catch (std::exception &e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
	return 0;
}
