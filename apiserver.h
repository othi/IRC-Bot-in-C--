#ifndef APISERVER_H
#define APISERVER_H

class APIServer;

#include "ircbot.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>

#define MAX_CLIENTS 1000

using namespace std;

class APIServer
{
public:
    APIServer();
    APIServer(int port);
    void SetBot (IRCBot* bot) { this->bot = bot; }
    void Run();
    void SendRawToClients (string message);

private:
    IRCBot* bot;
    fd_set db;
    int server_socket;
    int port;
};

#endif // APISERVER_H
