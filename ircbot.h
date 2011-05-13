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
    void Send(string msg);
    void Run();
    void Parse(string msg);
    void Login();
    void Ping(string ping_id);
    void Perform();
    void Join(string channel, string key = "");
private:
    bool connected;

    string nick;
    string user;
    string server;
    string port;

    int server_socket;

    bool performed;
};

#endif // IRCBOT_H
