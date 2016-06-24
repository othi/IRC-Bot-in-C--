#ifndef IRCBOT_H
#define IRCBOT_H

using namespace std;

class IRCBot;

#include <string>
#include <iostream>
#include <sys/select.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <set>

#include "common.h"
#include "apiserver.h"

#define CRLF "\r\n"
#define BUFSIZE 512

#define WHITE "00"
#define BLACK "01"
#define DARKBLUE  "02"
#define GREEN "03"
#define RED "04"
#define DARKRED "05"
#define VIOLET "06"
#define ORANGE "07"
#define YELLOW "08"
#define LIGHTGREEN "09"
#define TURQUOISE "10"
#define LIGHTBLUE "11"
#define BLUE "12"
#define PINK "13"
#define GREY "14"
#define LIGHTGREY "15"

#define COLOR "\x03"
#define BOLD "\x02"

struct schannel {
    string name;
    string modes;
    string topic;
    set<string> ops;
    set<string> peons;
    set<string> users;

};

typedef map<string, schannel> channel_map;

class IRCBot
{
public:    
    IRCBot();
    ~IRCBot();
    IRCBot(APIServer* api);

    void setNick(string nick) { this->nick = nick; }
    void setPassword(string password) { this->password = password; }
    void setUser(string user) { this->user = user; }

    void setServer(string server) { this->server = server; }
    void setPort(string port) { this->port = port; }

    void Connect();
  
    void Send(string msg);
    void Raw(string msg);
    void Run();
    void Parse(string msg);
    void Disconnect();
	void killbot();
    void Login();
    void Perform();
    void Privmsg(string target, string message);
    void Join(string channel, string key = "");
    void Part(string channel);
    void Nick(string newnick);
    void Quit(string message);

    string Color(string text, string fg, string bg = "");
    string Bold(string text);

	int ConnectToIRC();
	channel_map channels;
private:
    int server_socket;

    APIServer* api;

    bool connected;
    bool performed;

    string nick;
    string password;
    string user;
    string server;
    string port;

    

    void OnPing(string ping_id);
    void OnPrivmsg(string nick, string hostname, string args);
    void OnJoin(string nick, string hostname, string args);
    void OnPart(string nick, string hostname, string args);
    void OnNotice(string nick, string hostname, string args);
    void OnKick(string nick, string hostname, string args);
    void OnNickInUse(string nick);
    void OnInitialTopic(string args);
    void OnTopic(string nick, string hostname, string args);
    void OnInitialUsers(string args);
    void OnMode(string nick,string hostname, string args);
    void OnOp(string nick,string hostname, string channel, string user);
    void OnDeOp(string nick,string hostname, string channel, string user);
    void OnVoice(string nick,string hostname, string channel, string user);
    void OnDeVoice(string nick,string hostname, string channel, string user);
    void OnQuit(string nick, string hostname, string args);

    void ChanInfo(string c);
};

#endif // IRCBOT_H
