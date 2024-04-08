#include <string>
#include <sstream>
#include <iostream>

// Fonction pour générer une réponse HTTP à partir du code de statut et du contenu donnés
std::string generateHTTPResponse(int statusCode, const std::string& content)
{
    // Créer une chaîne de caractères pour stocker la réponse HTTP
    std::ostringstream response;

    // Ajouter la ligne de statut à la réponse
    response << "HTTP/1.1 " << statusCode << " ";

    // Ajouter la description de l'état en fonction du code de statut
    if (statusCode == 200)
	{
        response << "OK";
    }
	else if (statusCode == 404)
	{
        response << "Not Found";
    }
	else
	{
        response << "Server Error";
    }

    // Ajouter les en-têtes de la réponse
    response << "\r\nContent-Length: " << content.length();
    response << "\r\nContent-Type: text/plain";
    response << "\r\n\r\n"; // Ajouter une ligne vide avant le corps de la réponse

    // Ajouter le contenu de la réponse
    response << content;

    // Retourner la réponse HTTP générée
    return response.str();
}

int main()
{
    // Exemple de contenu de réponse
    std::string content = "Hello, World!";

    // Générer une réponse HTTP avec le code de statut 200 et le contenu donnés
    std::string response = generateHTTPResponse(200, content);

    // Afficher la réponse générée
    std::cout << "HTTP Response:" << std::endl;
    std::cout << response << std::endl;

    return 0;
}
