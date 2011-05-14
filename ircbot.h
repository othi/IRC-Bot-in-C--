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
#include <vector>

#include "common.h"

#define CRLF "\r\n"
#define BUFSIZE 512

class IRCBot
{
public:
    IRCBot();
    ~IRCBot();
    IRCBot(string nick, string user, string server, string port);

    void setNick(string nick) { this->nick = nick; }
    void setUser(string user) { this->user = user; }

    void setServer(string server) { this->server = server; }
    void setPort(string port) { this->port = port; }

    void Connect();
    void Send(string msg);
    void Run();
    void Parse(string msg);
    void Disconnect();

    void Login();
    void Perform();
    void Privmsg(string target, string message);
    void Join(string channel, string key = "");
    void Nick(string newnick);
    void Quit(string message);

    void OnPing(string ping_id);
    void OnPrivmsg(string nick, string hostname, string args);
    void OnJoin(string nick, string hostname, string args);
    void OnPart(string nick, string hostname, string args);
    void OnNotice(string nick, string hostname, string args);
    void OnKick(string nick, string hostname, string args);
    void OnNickInUse(string nick);



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
