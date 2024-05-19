#include <iostream>
#include <csignal>
#include <vector>
#include <pthread.h> // Pour la gestion des threads
#include <fcntl.h> // Pour fcntl() et O_NONBLOCK
#include <poll.h> // Pour poll()
#include <sys/socket.h> // Pour accept()
#include "srcs/colors.hpp"
#include "srcs/Configuration/ConfigCheck.hpp"
#include "srcs/Configuration/ConfigParse.hpp"
#include "srcs/Server/InitializeServer.hpp"
#include "srcs/Server/Server.hpp"

// Création d'une instance de serveur
Server server;

// Structure pour passer les données aux threads
struct ThreadData
{
	int client_socket;
	// Mettre la strucutre ailleurs?
	//Ajouter d'autres données si necessaire ??
};

// Méthode pour gérer une connexion dans un thread
void* handleClientThread(void* arg)
{
	ThreadData* data = (ThreadData*)arg;
	int client_socket = data->client_socket;
	// Gérer la connexion du client ici
	server.handleClient(client_socket);
	close(client_socket);
	delete data;
	pthread_exit(NULL);
}

void signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		std::cout << "Signal " << signal << " received. Shutting down server..." << std::endl;
		server.stop();
		exit(0);
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
		ConfigCheck config(config_file);
		ConfigParse parse(config);

		// Création d'une instance de serveur pour chaque configuration de serveur
		const std::vector<t_server>& servers = parse.getServersParsed();
		std::vector<Server> serverInstances;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			Server newServer;
			initializeServer(newServer, servers[i]); // Passer un seul serveur à la fois
			newServer.start(); // Démarrer le serveur
			serverInstances.push_back(newServer);
		}

		// Gestionnaire de signal
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		// Tableau de threads pour gérer les connexions
		std::vector<pthread_t> threads;

		// Boucle pour accepter les connexions et les gérer
		while (true)
		{
			for (size_t i = 0; i < serverInstances.size(); ++i)
			{
				// Utilisation de poll() pour attendre les événements d'entrée/sortie
				std::vector<pollfd> fds;
				pollfd listening_fd;
				listening_fd.fd = serverInstances[i].getListeningSocket();
				listening_fd.events = POLLIN;
				listening_fd.revents = 0;
				fds.push_back(listening_fd);

				int activity = poll(&fds[0], fds.size(), -1);
				if (activity < 0)
				{
					std::cerr << "Error in poll()" << std::endl;
					continue;
				}

				// Si le socket d'écoute a une activité
				if (fds[0].revents & POLLIN)
				{
					// Accepter la nouvelle connexion
					int client_socket = accept(serverInstances[i].getListeningSocket(), NULL, NULL);
					if (client_socket < 0)
					{
						std::cerr << "Error accepting connection" << std::endl;
						continue;
					}

					// Configurer le socket en mode non bloquant
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					// Créer un thread pour gérer la connexion de manière asynchrone
					pthread_t thread;
					ThreadData* data = new ThreadData;
					data->client_socket = client_socket;
					pthread_create(&thread, NULL, handleClientThread, (void*)data);
					threads.push_back(thread);
				}
			}

			// Supprimer les threads terminés de la liste
			for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); )
			{
				pthread_t thread = *it;
				if (pthread_tryjoin_np(thread, NULL) == 0)
					it = threads.erase(it);
				else
					++it;
			}
		}
	}
	catch (std::exception &e)
	{
		std::cerr << RED << e.what() << RESET << std::endl;
		return 1;
	}
	return 0;
}
