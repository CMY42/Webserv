#include "PC_1.hpp"
bool areAllPathCharactersValid(const std::string &path)
{
	const std::string allowedSpecialChars = "_-.~:/?#[]@!$&'()*+,;=";
	for (size_t i = 0; i < path.length(); i++)
	{
		if (!isalnum(path[i]) && allowedSpecialChars.find(path[i]) == std::string::npos)
			return false;
	}
	return true;
}

bool isSafePath(const std::string &path)
{
	// Check for directory traversal
	return path.find("..") == std::string::npos;
}

bool isPathLengthValid(const std::string &path, size_t maxLength)
{
	return path.length() <= maxLength;
}

//----------------------------------------------------------------------------------------------------------

void printMap(const std::map<std::string, std::string>& post_file_map) {
    // Vérification si la map est vide
    if (post_file_map.empty()) {
        std::cout << "The map is empty." << std::endl;
        return;
    }

    // Déclaration d'un itérateur pour parcourir la map
    std::map<std::string, std::string>::const_iterator it;

    // Parcours de la map avec l'itérateur
    for (it = post_file_map.begin(); it != post_file_map.end(); ++it) {
        std::cout << "Key: " << it->first << " - Value: " << it->second << std::endl;
    }
}

bool isExtensionAllowed(const std::string& uri)
{
// Tableau des extensions de fichier autorisées
    const char* allowedExtensions[] = {".html", ".css", ".js", ".jpg", ".png", ".txt", ".json", ".pl"};
    const int numExtensions = 8; // Nombre d'extensions dans le tableau

    // Trouver la dernière occurrence du slash dans l'URI pour délimiter le début du nom du fichier
    std::size_t lastSlashPos = uri.rfind('/');

    // Trouver la dernière occurrence du point dans l'URI après le dernier slash
    std::size_t lastDotPos = uri.rfind('.');
    if (lastDotPos == std::string::npos || lastDotPos < lastSlashPos) {
        return true;  // Aucune extension ou le point est avant le dernier slash (partie du chemin)
    }

    // Vérifier si le point est à la fin de l'URI
    if (lastDotPos == uri.length() - 1) {
        return true;  // Extension vide est considérée comme absence d'extension
    }

    // Extraire l'extension du fichier
    std::string extension = uri.substr(lastDotPos);

    // Parcourir le tableau d'extensions autorisées pour vérifier si l'extension est autorisée
    for (int i = 0; i < numExtensions; i++) {
        if (extension == allowedExtensions[i]) {
            return true;
        }
    }

    return false;
}

void Part_C::parse(const std::string& requestText, s_server2& config)
{
    std::istringstream requestStream(requestText);
    std::string line;

    // Parse de la ligne de requête
    if (std::getline(requestStream, line)) {
        std::istringstream lineStream(line);
        lineStream >> method >> uri >> httpVersion;
    }
    if (method != "GET" && method != "POST" && method != "DELETE")
	{
		status = 405;
		throw Part_C::InvalidRequestException("Error Method 405");
	}
    if (!areAllPathCharactersValid(uri))
	{
		status = 400;
		throw Part_C::InvalidRequestException("Error Uri 400");
	}
	if (!isSafePath(uri))
	{
		status = 403;
		throw Part_C::InvalidRequestException("Error Uri 403");
	}
	if (!isPathLengthValid(uri, 4096))
	{
		status = 414;
		throw Part_C::InvalidRequestException("Error Uri 414");
	}
    if (httpVersion != "HTTP/1.1" && httpVersion != "HTTP/1.0")
	{
		status = 505;
		throw Part_C::InvalidRequestException("Error HttpVersion 505");
	}
    if (!isExtensionAllowed(uri))
    {
        status = 404;
		throw Part_C::InvalidRequestException("Error HttpVersion 404");
    }


    // Parse des en-têtes
    while (std::getline(requestStream, line))
    {
        if (line.empty() || line[0] == '\r') break; // Vérifie la fin des en-têtes grâce à la ligne vide
        std::string::size_type colonPos = line.find(":");
        if (colonPos != std::string::npos && colonPos != 0)
        { // Vérifie aussi que ':' n'est pas le premier caractère
            std::string headerName = line.substr(0, colonPos);
            // Assurez-vous qu'il y a un espace après ':' avant de commencer la valeur
            std::string headerValue = (colonPos + 2 < line.size()) ? line.substr(colonPos + 2) : "";

            // Trim leading and trailing whitespaces from headerName and headerValue if necessary
            // ...

            if (!headerName.empty() && !headerValue.empty())
            { // Vérifie que ni le nom ni la valeur de l'en-tête ne sont vides
                headers[headerName] = headerValue; // Ajout de l'en-tête à la map
            }
            else
            {
                // Gestion d'erreur ou log si le nom de l'en-tête ou la valeur est vide
                status = 400;
		        throw Part_C::InvalidRequestException("Error Headers 400");
            }
        }
        else
        {
            // Gestion d'erreur ou log si la ligne d'en-tête est mal formée (pas de ':' ou ':' est le premier caractère)
            status = 400;
            std::cout << "\nTEST ERROR\n";
		    throw Part_C::InvalidRequestException("Error Headers 400");
        }
    }

    // Le reste est le corps de la requête
    if(method == "DELETE")
        return;


    std::istreambuf_iterator<char> begin(requestStream), end;
    std::string potential_body(begin, end);

    if (potential_body.size() > config.client_max_body_size)
    {
        status = 413;
        throw Part_C::InvalidRequestException("Error Body 413");
    }
    if (headers.find("Content-Type") != headers.end())
    {
        if(headers["Content-Type"].find("multipart/form-data") != std::string::npos)
        {
            std::cout << "\n-----> Body form multipart/form-data\n";
            parseMultiPartBody(potential_body);
        }
        else if(headers["Content-Type"].find("application/x-www-form-urlencoded") != std::string::npos)
        {
            std::cout << "\n-----> Body form application/x-www-form-urlencoded\n" << potential_body << std::endl;
            std::map<std::string, std::string> post_file_map = parseUrlEncoded(potential_body);
            printMap(post_file_map);
            post_file_name = post_file_map["filename"];
            post_file_content = post_file_map["content"];
        }
        else if(headers["Content-Type"].find("test/file") != std::string::npos)
        {
            std::cout << "\n-----> ERROR hehe\n\n";
            status = 405;
            throw Part_C::InvalidRequestException("Error Body 405");
        }
        else
        {
            std::cout << "\n-----> Body form not supported\n\n";
            status = 415;
            throw Part_C::InvalidRequestException("Error Body 415");
        }
    }
    std::cout << "post_file_name : " << post_file_content << "\n";
    std::string body = potential_body;
}

void Part_C::print_parse()
{
    std::cout << "-------------------> Impression du parsing\n";
    std::cout << "method : " << method << std::endl;
    std::cout << "uri : " << uri << std::endl;
    std::cout << "httpVersion : " << httpVersion << std::endl;
    std::cout << "headers : " << std::endl;
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << "     " << it->first << " : " << it->second << std::endl;
    }
    std::cout << "body : " << body << std::endl;
    std::cout << std::endl;
}

//----------------------------------------------------------------------------------------------------------

std::string Part_C::getMultiPartBoundary()
{
	std::string content_type = headers.find("Content-Type")->second;
	std::string boundaryPrefix = "boundary=";
    size_t boundaryPos = content_type.find(boundaryPrefix);
    
    if (boundaryPos == std::string::npos) {
        throw std::runtime_error("Boundary delimiter not found in Content-Type");
    }

    boundaryPos += boundaryPrefix.length();
    size_t boundaryEnd = content_type.find(";", boundaryPos);
    
    if (boundaryEnd == std::string::npos) {
        // Boundary is at the end of the string
        return content_type.substr(boundaryPos);
    } else {
        // Boundary ends before a semicolon
        return content_type.substr(boundaryPos, boundaryEnd - boundaryPos);
    }
}

struct FormData {
    std::string headers;
    std::string content;
};

void Part_C::parseMultiPartBody(std::string bodyLines)
{
    //std::cout << LOG_COLOR << "[LOG] " << RESET << "Content-Type: multipart/form-data" << std::endl;
	std::string boundary = getMultiPartBoundary();
    //std::string test = "123456789";

    std::vector<FormData> formDataParts;
    std::size_t pos = 0, lastPos = 0;

    while ((pos = bodyLines.find(boundary, lastPos)) != std::string::npos) {
        std::size_t endOfPart = bodyLines.find(boundary, pos + boundary.length());
        if (endOfPart == std::string::npos) {
            break;
        }

        // Trim the leading boundary and trailing new line
        std::size_t partStart = pos + boundary.length() + 2; // Skip the boundary and the newline
        std::size_t partEnd = endOfPart;

        // Check if it's the last part
        if (bodyLines.substr(partEnd, boundary.length() + 2) == boundary + "--") {
            partEnd -= 2; // Adjust to avoid the '--' at the end of the last boundary
        }

        std::string part = bodyLines.substr(partStart, partEnd - partStart - 2); // Subtract 2 to remove the trailing CR LF

        // Separate headers and content
        std::size_t headerEnd = part.find("\r\n\r\n");
        std::string headers = part.substr(0, headerEnd);
        std::string content = part.substr(headerEnd + 4);

        FormData formData;
        formData.headers = headers;
        formData.content = content;
        formDataParts.push_back(formData);

        lastPos = partEnd;
    }

    for (std::vector<FormData>::iterator it = formDataParts.begin(); it != formDataParts.end(); ++it)
    {
        std::size_t fnPos = it->headers.find("filename=\"");
        if (fnPos != std::string::npos) {
            fnPos += 10; // Skip 'filename="'
            std::size_t fnEnd = it->headers.find('"', fnPos);
            if (fnEnd != std::string::npos) {
                post_file_name = it->headers.substr(fnPos, fnEnd - fnPos);
                post_file_content = it->content;
            }
        }
    }

        std::cout << "XXX File Name: " << post_file_name << "\n";
        std::cout << "XXX File Content:\n" << post_file_content << "\n";

    if (post_file_name.empty())
    {
        status = 400; // Bad Request
		throw Part_C::InvalidRequestException("Error Parse Multi Part Body 400");
    }

	/*std::size_t start = bodyLines.find(boundary);
	if (start == std::string::npos)
	{
		status = 400; // Bad Request
		throw Part_C::InvalidRequestException("Error Parse Multi Part Body 400");
	}
    //std::cout << test + "XX\n";

    boundary[boundary.length() - 1] = '-';
    boundary[boundary.length()] = '-';
    boundary[boundary.length() + 1] = '\0';
    std::cout << boundary + "\n";

	std::size_t end = bodyLines.find(boundary);
	if (end == std::string::npos)
	{
        std::cout << "no end boundary\n";
		status = 400; // Bad Request
		throw Part_C::InvalidRequestException("Error Parse Multi Part Body 400");
	}
	std::string headers_and_body = bodyLines.substr(start + boundary.length(), end - start - boundary.length());
	std::string headers = headers_and_body.substr(0, headers_and_body.find(std::string("\r\n") + std::string("\r\n")));
	std::string body = headers_and_body.substr(headers_and_body.find(std::string("\r\n") + std::string("\r\n")) + 4);
	std::size_t filename_start = headers.find("filename=");

    std::cout << "\n   X---X\n";
    std::cout << headers_and_body << "\n   X---X\n";
    std::cout << headers << "\n   X---X\n";
    std::cout << body << "\n   X---X\n";

	if (filename_start == std::string::npos)
	{
        std::cout << "other\n";
		status = 400; // Bad Request
		throw Part_C::InvalidRequestException("Error Parse Multi Part Body 400");
	}
	std::size_t filename_end = headers.find('"', filename_start + 10);
	post_file_name = headers.substr(filename_start + 10, filename_end - filename_start - 10);

	post_file_content = body.substr(0, body.length() - 2);*/
}

std::map<std::string, std::string> Part_C::parseUrlEncoded(const std::string& data)
{
    std::map<std::string, std::string> result;
    std::istringstream dataStream(data);
    std::string pair;

    while (std::getline(dataStream, pair, '&'))
    {
        std::string::size_type delimiterPos = pair.find('=');
        if (delimiterPos != std::string::npos)
        {
            std::string key = pair.substr(0, delimiterPos);
            std::string value = pair.substr(delimiterPos + 1);

            // Remplacer '+' par ' ' dans value
            std::replace(value.begin(), value.end(), '+', ' ');

            // Décoder les caractères en pourcentage ici si nécessaire

            result[key] = value;
        }
    }

    return result;
}

std::map<std::string, std::string> parseUrlEncodedData(const std::string& data)
{
    std::map<std::string, std::string> result;
    std::istringstream dataStream(data);
    std::string pair;

    while (std::getline(dataStream, pair, '&'))
    {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos)
        {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            result[key] = value; // Dans une version complète, vous devriez décoder les pourcentages ici.
        }
    }

    return result;
}