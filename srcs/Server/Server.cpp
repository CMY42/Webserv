/*#include "Server.hpp"
#include "InitializeServer.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>

// Constructeur
Server::Server(const std::string& name, int p, size_t max_body_size, const std::map<int, std::string>& errors, const std::map<std::string, std::map<std::string, std::string> >& rt)
	: server_name(name), port(p), client_max_body_size(max_body_size), error_pages(errors), routes(rt) {}

// Méthode pour démarrer le serveur
void Server::start()
{
	// Création du socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Error creating socket" << std::endl;
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

// Méthode pour gérer les connexions entrantes
void Server::handleConnections()
{
	fd_set readfds;
	int max_fd = _listening_socket;

	// Boucle infinie pour attendre les connexions et les gérer
	while (true)
	{
		// Effacer l'ensemble de descripteurs de fichiers en attente
		FD_ZERO(&readfds);

		// Ajouter le descripteur de fichier du socket d'écoute à l'ensemble
		FD_SET(_listening_socket, &readfds);

		// Mettre à jour le descripteur de fichier maximum
		max_fd = _listening_socket;

		// Sélectionner les descripteurs de fichiers prêts en lecture
		int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
		if (activity < 0)
		{
			std::cerr << "Error in select()" << std::endl;
			continue;
		}

		// Si le socket d'écoute a une activité
		if (FD_ISSET(_listening_socket, &readfds))
		{
			// Accepter la nouvelle connexion
			int client_socket = accept(_listening_socket, NULL, NULL);
			if (client_socket < 0)
			{
				std::cerr << "Error accepting connection" << std::endl;
				continue;
			}

			// Créer un nouveau thread pour gérer la connexion client
			std::thread client_thread(handleClient, client_socket);
			client_thread.detach(); // Détacher le thread pour qu'il s'exécute en arrière-plan
		}
		// Si le socket d'écoute a une activité
		if (FD_ISSET(_listening_socket, &readfds))
		{
			// Accepter la nouvelle connexion
			int client_socket = accept(_listening_socket, NULL, NULL);
			if (client_socket < 0)
			{
				std::cerr << "Error accepting connection" << std::endl;
				continue;
			}

			// Ajouter le nouveau descripteur de fichier à l'ensemble
			FD_SET(client_socket, &readfds);
			if (client_socket > max_fd)
			{
				max_fd = client_socket;
			}
		}

		// Gérer les données disponibles sur les sockets clients
		// Ne fonctionne pas SEGFAULT
		// Voir avec ADRIEN si c'est son code qui doit gerer cela??
		for (int i = _listening_socket + 1; i <= max_fd; ++i)
		{
			if (FD_ISSET(i, &readfds))
			{
				// Lire les données du socket client, traiter les demandes,...
				// Exemple, ou je supprime la connexion
				char buffer[1024];
				int bytes_received = recv(i, buffer, sizeof(buffer), 0);
				if (bytes_received <= 0)
				{
					// Erreur ou connexion fermée par le client
					close(i);
					FD_CLR(i, &readfds);
				}
				else
				{
					// Traitement des données reçues du client partie de ADRIEN???
					// ...
				}
			}
		}
	}
}
void Server::handleConnections()
{
    // Boucle infinie pour attendre les connexions et les gérer
    while (true)
    {
        // Accepter la nouvelle connexion
        int client_socket = accept(_listening_socket, NULL, NULL);
        if (client_socket < 0)
        {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        // Créer un nouveau processus pour gérer la connexion client
        pid_t pid = fork();
        if (pid == -1)
        {
            std::cerr << "Error forking process" << std::endl;
            close(client_socket);
            continue;
        }
        else if (pid == 0) // Processus enfant
        {
            // Gérer la connexion client dans le processus enfant
            handleClient(client_socket);

            // Fermer le socket client dans le processus enfant
            close(client_socket);
            exit(0); // Terminer le processus enfant
        }
        else // Processus parent
        {
            // Fermer le socket client dans le processus parent
            close(client_socket);
        }

        // Attendre que les processus enfants se terminent pour éviter les zombies
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}

void Server::handleClient(int client_socket)
{
    // Placeholder pour le traitement du client
    // À adapter selon vos besoins
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0)
    {
        // Erreur ou connexion fermée par le client
        close(client_socket);
        return;
    }

    // Traitement des données reçues (par exemple, analyse des requêtes HTTP)

    // Génération de la réponse appropriée
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World\n";

    // Envoi de la réponse au client
    send(client_socket, response.c_str(), response.length(), 0);

    // Fermeture de la connexion
    close(client_socket);
}*/

#include "Server.hpp"
#include "InitializeServer.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <vector>

// Constructeur
Server::Server(const std::string& name, int p, size_t max_body_size, const std::map<int, std::string>& errors, const std::map<std::string, std::map<std::string, std::string> >& rt)
    : server_name(name), port(p), client_max_body_size(max_body_size), error_pages(errors), routes(rt) {}

// Méthode pour démarrer le serveur
void Server::start()
{
    // Création du socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
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

void Server::handleConnections()
{
    // Tableau de structures pollfd pour stocker les descripteurs de fichiers à surveiller
    std::vector<pollfd> fds;
    pollfd listening_fd;
    listening_fd.fd = _listening_socket;
    listening_fd.events = POLLIN;
    listening_fd.revents = 0;
    fds.push_back(listening_fd);

    // Boucle infinie pour attendre les connexions et les gérer
    while (true)
    {
        // Utilisation de poll() pour attendre les événements d'entrée/sortie
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
            int client_socket = accept(_listening_socket, NULL, NULL);
            if (client_socket < 0)
            {
                std::cerr << "Error accepting connection" << std::endl;
                continue;
            }

            // Ajouter le descripteur de fichier du client à surveiller
            pollfd client_fd;
            client_fd.fd = client_socket;
            client_fd.events = POLLIN;
            client_fd.revents = 0;
            fds.push_back(client_fd);
        }

        // Parcourir les descripteurs de fichiers surveillés pour les clients
        for (size_t i = 1; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN) // Si le descripteur est prêt en lecture
            {
                // Gérer la connexion client
                handleClient(fds[i].fd);

                // Supprimer le descripteur de fichier du client surveillé
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
            }
        }
    }
}

void Server::handleClient(int client_socket)
{
	// Placeholder pour le traitement du client
	char buffer[1024];
	int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
	if (bytes_received <= 0)
	{
		// Erreur ou connexion fermée par le client
		close(client_socket);
		return;
	}

std::cout << "Bytes received: " << bytes_received << std::endl;

	// Traitement des données reçues (par exemple, analyse des requêtes HTTP)
	//POUR ADRIEN ???

	// Génération de la réponse appropriée
	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World\n";

	// Envoi de la réponse au client
	send(client_socket, response.c_str(), response.length(), 0);
}
