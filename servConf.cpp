#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

// Fonction pour charger la configuration à partir d'un fichier JSON
std::map<std::string, std::string> loadConfig(const std::string& filename)
{
    std::map<std::string, std::string> config;

    // Ouvrir le fichier JSON
    std::ifstream file(filename.c_str());
    if (!file.is_open())
	{
        std::cerr << "Error opening file: " << filename << std::endl;
        return config;
    }

    // Lire le contenu du fichier JSON
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();

    // Parser le contenu JSON
    // Analyseur JSON ou bibli externe a inclure ???

    // Exemple de traitement des paramètres de configuration
    // En supposant que le fichier JSON contienne les paramètres suivants : {"port": 8080, "max_connections": 100}
    config["port"] = "8080";
    config["max_connections"] = "100";

    return config;
}

int main()
{
    // Charger la configuration à partir du fichier JSON
    std::map<std::string, std::string> config = loadConfig("config.json");

    // Afficher les paramètres de configuration chargés
    std::cout << "Server configuration:" << std::endl;
    for (auto it = config.begin(); it != config.end(); ++it)
	{
        std::cout << it->first << ": " << it->second << std::endl;
    }

    // paramètres de configuration a utiliser pour configurer le serveur
    // Exemple : int port = std::stoi(config["port"]); ou autre voir avec mon mat de la partie A

    return 0;
}
