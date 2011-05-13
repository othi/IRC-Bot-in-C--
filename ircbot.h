#ifndef IRCBOT_H
#define IRCBOT_H

using namespace std;

#include <string>
#include <iostream>
#include <sys/select.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

#define CRLF "\r\n"

class IRCBot
{
public:
    IRCBot();
    IRCBot(string nick, string user, string server, string port);

    void setNick(string nick) { this->nick = nick; }
    void setUser(string user) { this->user = user; }

    void setServer(string server) { this->server = server; }
    void setPort(string port) { this->port = port; }

    void Connect();
    void Run();
    void Parse(string msg);
    void Send(string msg);
    void Login();

private:
    bool connected;

    string nick;
    string user;
    string server;
    string port;

    int server_socket;
};

#endif // IRCBOT_H
