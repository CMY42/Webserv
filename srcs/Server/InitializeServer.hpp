#ifndef INITIALIZE_SERVER_HPP
#define INITIALIZE_SERVER_HPP

#include "../Configuration/ConfigCheck.hpp"
#include "../Configuration/ConfigParse.hpp"
#include "Server.hpp"

void initializeServer(Server& server, const t_server& parsedServer);

#endif
