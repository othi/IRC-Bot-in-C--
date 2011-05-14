#include "ircbot.h"

IRCBot::IRCBot()
{
}

IRCBot::~IRCBot()
{
    Quit("IRC Bot destroyed.");
}

IRCBot::IRCBot(string nick, string user, string server, string port)
{
    setNick(nick);
    setUser(user);
    setServer(server);
    setPort(port);

    this->connected = false;
    this->performed = false;
}

void IRCBot::Connect()
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
    server_socket = sockfd;

    Run();
}

void IRCBot::Send (string msg)
{
    msg.append(CRLF);
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
                if (i == msg1.size()-1 && msg1[i][msg1[i].length()-1] != '\n')
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
    Join("#othi");
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

    if (target == "#othi" && message == "!botping")
        Privmsg(target, nick+": Pong!");
}

void IRCBot::OnNotice(string nick, string hostmask, string args)
{
    if (!performed)
        Perform();
}


void IRCBot::OnJoin(string nick, string hostname, string args)
{
    string chan = args;

    Privmsg(chan, "Hi "+nick+"!");
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
