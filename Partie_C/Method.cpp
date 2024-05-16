#include "PC_1.hpp"

void Part_C::method_GET()
{
    std::cout << std::endl << "-------------------> GET" << std::endl;
}

bool checkFileExists(const std::string &path)
{
	std::ifstream file(path.c_str());
	return (file.good());
}

void Part_C::method_POST()
{
    std::cout << std::endl << "-------------------> POST" << std::endl;

    std::string upload_path = basePath + uri + "/" + post_file_name;
    std::cout << std::endl << "upload_path : " << upload_path << std::endl;
	if (checkFileExists(upload_path))
	{
        std::cout << "-----upload_path : " << upload_path << "\n";
		status= 409; // Conflict
		throw Part_C::InvalidRequestException("Error Post 409");
	}

	std::ofstream file;
	file.open(upload_path.c_str(), std::ios::out);

	if (!file.is_open())
	{
		status = 500; // Internal Server Error
		throw Part_C::InvalidRequestException("Error Post 500");
	}
	file << post_file_content;
	file.close();

	status = 201; // Created
    contentType = "text/plain"; // Ajustez selon le type de réponse que vous voulez renvoyer
    content = "Resource created successfully."; // Personnalisez le message selon le résultat du traitement
}

void Part_C::method_DELETE()
{
    std::cout << std::endl << "-------------------> DELETE" << std::endl;

    // L'URI de la requête doit être analysée pour déterminer la ressource à supprimer.
    // Ici, nous supposons que l'URI contient le nom du fichier à supprimer dans un certain répertoire.
    std::string delete_path = basePath + uri; // delete_file_name doit être extrait de l'URI
    std::cout << delete_path << std::endl;

    if (!checkFileExists(delete_path))
    {
        status = 404; // Not Found
        throw Part_C::InvalidRequestException("Error Delete 404");
    }

    // Tentative de suppression du fichier
    if (std::remove(delete_path.c_str()) == 0) // La fonction std::remove retourne 0 en cas de succès
    {
        status = 200; // OK
        contentType = "text/plain";
        content = "File deleted successfully.";
    }
    else
    {
        // La suppression a échoué, peut-être en raison d'un problème de permissions
        status = 500; // Internal Server Error
        throw Part_C::InvalidRequestException("Error Delete 500");
    }
}