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
	Server* server_instance;
};

// Méthode pour gérer une connexion dans un thread
void* handleClientThread(void* arg)
{
	ThreadData* data = (ThreadData*)arg;
	int client_socket = data->client_socket;
	Server* server_instance = data->server_instance;

	// Gérer la connexion du client ici
	server_instance->handleClient(client_socket);

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

bool is_thread_finished(pthread_t thread)
{
	int err = pthread_kill(thread, 0);
	if (err == 0)
	{
		return false;
	}
	else if (err == ESRCH)
	{
		return true;
	}
	else
	{
		std::cerr << "Erreur lors de la vérification du thread: " << err << std::endl;
	return false;
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

		const std::vector<t_server>& servers = parse.getServersParsed();
		std::vector<Server> serverInstances;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			Server newServer;
			initializeServer(newServer, servers[i]);
			newServer.start();
			serverInstances.push_back(newServer);
		}

		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);

		std::vector<pollfd> fds;
		for (size_t i = 0; i < serverInstances.size(); ++i)
		{
			pollfd listening_fd;
			listening_fd.fd = serverInstances[i].getListeningSocket();
			listening_fd.events = POLLIN;
			fds.push_back(listening_fd);
		}

		std::vector<pthread_t> threads;

		while (true)
		{
			int activity = poll(fds.data(), fds.size(), 100); // Timeout de 100 millisecondes
			if (activity < 0)
			{
				std::cerr << "Error in poll()" << std::endl;
				continue;
			}

			for (size_t i = 0; i < serverInstances.size(); ++i)
			{
				if (fds[i].revents & POLLIN)
				{
					int client_socket = accept(serverInstances[i].getListeningSocket(), NULL, NULL);
					if (client_socket < 0)
					{
						std::cerr << "Error accepting connection" << std::endl;
						continue;
					}

					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					pthread_t thread;
					ThreadData* data = new ThreadData;
					data->client_socket = client_socket;
					data->server_instance = &serverInstances[i];
					pthread_create(&thread, NULL, handleClientThread, (void*)data);
					threads.push_back(thread);
				}
			}

			for (std::vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); )
			{
				pthread_t thread = *it;
				if (is_thread_finished(thread))
				{
					pthread_join(thread, NULL);
					it = threads.erase(it);
				}
				else
				{
					++it;
				}
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
