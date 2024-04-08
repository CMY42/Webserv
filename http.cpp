#include <string>
#include <sstream>
#include <iostream>

// Fonction pour analyser une requête HTTP et extraire la méthode et l'URI
void parseHTTPRequest(const std::string& request, std::string& method, std::string& uri)
{
    // Initialiser les variables pour stocker la méthode et l'URI
    method.clear();
    uri.clear();

    // Utiliser un flux de chaînes pour extraire la méthode et l'URI de la requête
    std::istringstream iss(request);
    iss >> method >> uri;

    // Afficher la méthode et l'URI pour vérification
    std::cout << "Method: " << method << std::endl;
    std::cout << "URI: " << uri << std::endl;
}

int main()
{
    // Exemple de requête HTTP
    std::string request = "GET /index.html HTTP/1.1";

    // Variables pour stocker la méthode et l'URI
    std::string method, uri;

    // Appel de la fonction pour analyser la requête
    parseHTTPRequest(request, method, uri);

    return 0;
}
