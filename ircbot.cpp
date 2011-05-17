#include "ircbot.h"

IRCBot::IRCBot()
{
}

IRCBot::~IRCBot()
{
    Quit("IRC Bot destroyed.");
}

IRCBot::IRCBot(APIServer* api)
{
    this->api = api;
    api->SetBot(this);
    this->connected = false;
    this->performed = false;
}

void IRCBot::Connect()
{
    server_socket = ConnectToIRC();

    Run();
}

int IRCBot::ConnectToIRC()
{
    cout << "Connecting to " << this->server << ":" << this->port << " as " << this->nick << "... " << endl;

    int sockfd;
    int temp;

    struct addrinfo hints;
    struct addrinfo *result, *rp;

    // resolve name
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    temp = getaddrinfo(this->server.c_str(), this->port.c_str(), &hints, &result);
    if (temp != 0) {
        cout << "could not resolve " << this->server << endl;
        exit(1);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);

        if (sockfd == -1)
            continue;

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

        close(sockfd);
    }

    if (rp == NULL)                 /* No address succeeded */
    {
        cout << "could not bind to socket" << endl;
        exit(1);
    }

    freeaddrinfo(result);           /* No longer needed */


    cout << "Connected." << endl;
    this->connected = true;

    return sockfd;
}

void IRCBot::Send (string msg)
{
    msg.append(CRLF);
    Raw(msg);
}

void IRCBot::Raw(string msg)
{
    if (send(server_socket, msg.c_str(), msg.length(), 0) == (int) msg.length())
        cout << "> " << msg;
    else
        cout << "Message not sent completely: " << msg;
}

void IRCBot::Run()
{
    Login();

    char buf[BUFSIZE];

    memset(buf, 0, BUFSIZE);

    string dangling_msg = "";

    while (recv(server_socket, buf, BUFSIZE, 0) > 0)
    {
        string msg = dangling_msg + string(buf);
        dangling_msg = "";

        vector<string> msg1 = Common::split(msg, '\r');

        for (int i = 0; i < (int)msg1.size(); i++)
        {
            if (msg1[i][0] == '\n')
                msg1[i] = msg1[i].substr(1);
            if (!msg1[i].empty())
            {
                // handle case when we didn't get the whole line
                if (i == (int)msg1.size()-1 && msg1[i][msg1[i].length()-1] != '\n')
                {
                    dangling_msg = msg1[i];
                    break;
                }
                Parse(msg1[i]);
            }
        }

        memset(buf, 0, BUFSIZE);
    }

    cout << "Disconnected from server" << endl;
    this->connected = false;
}

void IRCBot::Parse (string msg)
{
    cout << "< " << msg << endl;

    api->SendRawToClients(msg+CRLF);

    if (msg.substr(0, 4) == "PING")
        OnPing(msg.substr(5));

    else if (msg.substr(0, 1) == ":")
    {
        // parse message
        vector<string> tokens = Common::split(msg, ' ');
        string nick, hostmask;

        if (tokens[0].find('!') != tokens[0].npos)
        {
            vector<string> usermask = Common::split(tokens[0].substr(1), '!');
            nick = usermask[0];
            hostmask = usermask[1];
        }

        string event = tokens[1];

        string args = msg.substr(tokens[0].length()+tokens[1].length()+2);

        if (event == "PRIVMSG")
            OnPrivmsg(nick, hostmask, args);
        else if (event == "JOIN")
            OnJoin(nick, hostmask, args);
        else if (event == "NOTICE")
            OnNotice(nick, hostmask, args);
        else if (event == "433")
            OnNickInUse(tokens[3]);
        else if (event == "332")
            OnInitialTopic(args);
        else if (event == "TOPIC")
            OnTopic(nick, hostmask, args);
        else if (event == "353")
            OnInitialUsers(args);
        else if (event == "MODE")
            OnMode(nick, hostmask, args);
        else if (event == "QUIT")
            OnQuit(nick, hostmask, args);
        else if (event == "PART")
            OnPart(nick, hostmask, args);
        else if (event == "KICK")
            OnKick(nick, hostmask, args);
    }
}

void IRCBot::Login ()
{
    string s;
    s = "NICK " + this->nick;
    Send(s);

    s = "USER " + this->user + " 0 * :" + this->user;
    Send(s);
}


void IRCBot::OnPing (string ping_id)
{
    string s("PONG ");
    s += ping_id;

    Send(s);
}

void IRCBot::Perform()
{
    
	Join("#chrissibot");
    performed = true;
}

void IRCBot::Join(string channel, string key)
{
    string s;
    s = "JOIN "+channel;

    if (key != "")
        s.append(" "+key);

    Send(s);
}

void IRCBot::Privmsg(string target, string message)
{
    string s = "PRIVMSG "+target+" :"+message;

    Send(s);
}

void IRCBot::OnPrivmsg(string nick, string hostmask, string args)
{
    string target = args.substr(0, args.find(' '));
    string message = args.substr(target.length()+2);

    if (target == "#othi" && message == ".ping")
        Privmsg(target, Bold(nick)+": Pong!");

    else if (message == ".chaninfo")
        ChanInfo(target);

    else if (message.substr(0, 4) == ".raw")
	Send(message.substr(5));
}

void IRCBot::OnNotice(string nick, string hostmask, string args)
{
    if (!performed)
        Perform();
}


void IRCBot::OnJoin(string nick, string hostname, string args)
{
    string chan = args;

    if (nick == this->nick)
    {
        struct schannel _chan;
        _chan.name = chan;
        _chan.topic = "";
        _chan.modes = "";
        _chan.ops = set<string>();
        _chan.peons = set<string>();
        _chan.users = set<string>();

        channels.insert(channel_map::value_type(chan, _chan));
    }
    else
    {
        struct schannel* _chan = &channels.find(chan)->second;
        _chan->users.insert(nick);
        Privmsg(chan, "Hi "+nick+"!");
    }
}

void IRCBot::OnPart(string nick, string hostname, string args)
{
    vector<string> tokens = Common::split(args, ' ');
    string chan;
    if (tokens[0][0] == ':')
        chan = tokens[0].substr(1);
    else
        chan = tokens[0];

    if (nick == this->nick)
    {
        channels.erase(chan);
    }
    else
    {
        struct schannel* _chan = &channels.find(chan)->second;
        _chan->users.erase(nick);
        _chan->peons.erase(nick);
        _chan->ops.erase(nick);
    }
}

void IRCBot::OnQuit(string nick, string hostname, string args)
{
    string message = args.substr(1);

    map<string, schannel>::iterator it;
    for (it = channels.begin(); it != channels.end(); it++)
    {
        struct schannel* _chan = &it->second;
        _chan->users.erase(nick);
        _chan->peons.erase(nick);
        _chan->ops.erase(nick);
    }
}

void IRCBot::OnKick(string nick, string hostname, string args)
{
    vector<string> tokens = Common::split(args, ' ');
    string chan = tokens[0];
    string knick = tokens[1];
    string message = args.substr(chan.length()+knick.length()+3);

    if (knick == this->nick)
    {
        channels.erase(chan);
    }
    else
    {
        struct schannel* _chan = &channels.find(chan)->second;
        _chan->users.erase(nick);
        _chan->peons.erase(nick);
        _chan->ops.erase(nick);
    }
}

void IRCBot::OnMode (string nick, string hostname, string args)
{
    vector<string> tokens = Common::split(args, ' ');

    string target = tokens[0];
    if (target == this->nick)
        return;

    string modes = tokens[1];
    bool giving = false;
    int user_pos = 2;
    for (int i = 0; i < (int)modes.length(); i++)
    {
        if (modes[i] == '+')
        {
            giving = true;
            continue;
        }
        else if (modes[i] == '-')
        {
            giving = false;
            continue;
        }

        if (modes[i] == 'o')
        {
            if (giving) // gave ops
                OnOp(nick, hostname, target, tokens[user_pos++]);
            else
                OnDeOp(nick, hostname, target, tokens[user_pos++]);
        }
        else if (modes[i] == 'v')
        {
            if (giving)
                OnVoice(nick, hostname, target, tokens[user_pos++]);
            else
                OnDeVoice(nick, hostname, target, tokens[user_pos++]);
        }
    }
}

void IRCBot::OnOp(string nick,string hostname, string channel, string user)
{
    struct schannel* _chan = &channels.find(channel)->second;
    _chan->ops.insert(user);
}

void IRCBot::OnDeOp(string nick,string hostname, string channel, string user)
{
    struct schannel* _chan = &channels.find(channel)->second;
    _chan->ops.erase(user);
}

void IRCBot::OnVoice(string nick,string hostname, string channel, string user)
{
    struct schannel* _chan = &channels.find(channel)->second;
    _chan->peons.insert(user);
}

void IRCBot::OnDeVoice(string nick,string hostname, string channel, string user)
{
    struct schannel* _chan = &channels.find(channel)->second;
    _chan->peons.erase(user);
}

void IRCBot::OnTopic (string nick, string hostname, string args)
{
    string target = args.substr(0, args.find(' '));
    string topic = args.substr(target.length()+2);
    channels.find(target)->second.topic = topic;
}

void IRCBot::OnInitialTopic (string args)
{
    vector<string> tokens = Common::split(args, ' ');
    string topic = args.substr(args.find(':')+1);
    channels.find(tokens[1])->second.topic = topic;

}

void IRCBot::OnInitialUsers (string args)
{
    vector<string> tokens = Common::split(args, ' ');
    struct schannel* chan = &channels.find(tokens[2])->second;
    for (int i = 3; i < (int)tokens.size(); i++)
    {
        if (tokens[i][0] == ':')
            tokens[i] = tokens[i].substr(1);

        if (tokens[i][0] == '@')
        {
            tokens[i] = tokens[i].substr(1);
            chan->ops.insert(tokens[i]);

        }
        else if (tokens[i][0] == '+')
        {
            tokens[i] = tokens[i].substr(1);
            chan->peons.insert(tokens[i]);
        }

        chan->users.insert(tokens[i]);
    }
}

void IRCBot::OnNickInUse(string nick)
{
    Nick(nick+"_");
}

void IRCBot::Nick(string newnick)
{
    this->nick = newnick;
    Send("NICK "+this->nick);
}

void IRCBot::Quit(string message)
{
    string s = "QUIT :"+message;

    Send(s);

    Disconnect();
}

void IRCBot::Disconnect()
{
    close(server_socket);
}

string IRCBot::Color(string text, string fg, string bg)
{
    if (bg != "")
        bg = "," + bg;

    return COLOR + fg + bg + text + COLOR;
}

string IRCBot::Bold(string text)
{
    return BOLD + text + BOLD;
}

void IRCBot::ChanInfo (string c)
{
    struct schannel chan = channels.find(c)->second;
    Privmsg(c, "Channel information for "+chan.name);
    Privmsg(c, Bold("Modes: ") +  "n/a" + chan.modes);
    Privmsg(c, Bold("Topic: ") + chan.topic);

    set<string>::iterator it;
    string u = "";
    for (it = chan.ops.begin(); it != chan.ops.end(); it++)
        u += *it+" ";
    Privmsg(c, Bold("Ops: ") + u);

    u = "";
    for (it = chan.peons.begin(); it != chan.peons.end(); it++)
        u += *it+" ";
    Privmsg(c, Bold("Voice: ") + u);

    u = "";
    for (it = chan.users.begin(); it != chan.users.end(); it++)
        u += *it+" ";
    Privmsg(c, Bold("Users: ") + u);

}
