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

#include <sys/wait.h> // Nécessaire pour waitpid et les macros associées

void Part_C::execute_cgi()
{
    std::cout << std::endl << "-------------------> CGI" << std::endl;

    int pipefd[2];
    pid_t pid;
    char buf;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        alarm(10);
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        std::string parentDir = getExecutableParentDir();
        std::string newBP = basePath.substr(1);
        std::string scriptPath = parentDir + newBP + uri;
        const char* cScriptPath = scriptPath.c_str();

        char *args[] = {const_cast<char *>("perl"), const_cast<char *>(cScriptPath), NULL};
        char *envp[] = { NULL };

        execve("/usr/bin/perl", args, envp);
        perror("execve");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(pipefd[1]);
        std::ostringstream oss;
        while (read(pipefd[0], &buf, 1) > 0)
        {
            std::cout.put(buf);
            oss.put(buf);
        }
        close(pipefd[0]);

        int statusPID;
        waitpid(pid, &statusPID, 0);
        if (WIFSIGNALED(statusPID))
        {
            std::cerr << "XXX Erreur détectée XXX" << std::endl;
            status = 500; // Conflict
		    throw Part_C::InvalidRequestException("Error CGI 500");
        }
        else if (WIFEXITED(statusPID) && WEXITSTATUS(statusPID) != 0)
        {
            std::cerr << "XXX Erreur détectée XXX" << std::endl;
            status = 500; // Conflict
		    throw Part_C::InvalidRequestException("Error CGI 500");
        }
        cgi_content = oss.str();
    }
}
