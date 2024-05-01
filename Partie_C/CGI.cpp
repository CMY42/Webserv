#include "PC_1.hpp"

bool Part_C::isCGI()
{
    std::string::size_type dotIndex = uri.find_last_of('.');
	if (dotIndex == std::string::npos)
	    return false;

	std::string extension = uri.substr(dotIndex + 1);

	if (extension == "py")
		return true;
	if (extension == "pl")
		return true;
	return false;
}

std::string getExecutableParentDir()
{
char path[PATH_MAX];
    char realPath[PATH_MAX];  // Pour contenir le chemin absolu résolu
    ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
    if (count == -1) {
        std::cerr << "Erreur lors de l'obtention du chemin de l'exécutable" << std::endl;
        return "";
    }
    path[count] = '\0';  // Termine la chaîne avec un caractère nul

    // Résolution du chemin absolu
    if (realpath(path, realPath) == NULL) {
        std::cerr << "Erreur lors de la résolution du chemin de l'exécutable" << std::endl;
        return "";
    }

    // Trouver le dernier slash pour obtenir le répertoire contenant l'exécutable
    std::string pathStr(realPath);
    size_t lastSlash = pathStr.find_last_of('/');

    if (lastSlash == std::string::npos) {
        std::cerr << "Impossible de trouver le dossier parent" << std::endl;
        return "";
    }

    return pathStr.substr(0, lastSlash); // Retourne le chemin du répertoire parent
}

void Part_C::execute_cgi()
{
    std::cout << std::endl << "-------------------> CGI" << std::endl;

    int pipefd[2];
    pid_t pid;
    char buf;

    // Création d'un pipe pour capturer la sortie du script CGI
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Création d'un processus enfant
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Processus enfant

        // Redirection de stdout vers le pipe
        close(pipefd[0]); // Ferme la lecture du pipe, non utilisée dans l'enfant
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Chemin absolu vers le script CGI
        //std::string path_ws = "/the_ultimate_webserv";
        std::string parentDir = getExecutableParentDir(); // Suppose que cette fonction retourne std::string
        std::string scriptPath = parentDir + "/the_ultimate_webserv" + uri; // Concaténation correcte
        const char* cScriptPath = scriptPath.c_str();

        // Préparer les arguments pour execve
        char *args[] = {const_cast<char *>("perl"), const_cast<char *>(cScriptPath), NULL};

        // Préparer les variables d'environnement
        char *envp[] = { NULL };

        // Exécuter le script CGI en Perl
        execve("/usr/bin/perl", args, envp);

        // execve ne retourne que s'il y a une erreur
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Processus parent

        // Fermeture de l'extrémité d'écriture du pipe
        close(pipefd[1]);

        // Lire la sortie du script CGI
        std::ostringstream oss;
        while (read(pipefd[0], &buf, 1) > 0)
        {
            std::cout.put(buf);
            oss.put(buf);
        }
        cgi_content = oss.str();
        close(pipefd[0]);

        // Attendre que le processus enfant termine
        waitpid(pid, NULL, 0);
    }
}