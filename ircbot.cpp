#include "ircbot.h"

IRCBot::IRCBot()
{
    this->connected = false;
}

IRCBot::IRCBot(string nick, string user, string server, string port)
{
    setNick(nick);
    setUser(user);
    setServer(server);
    setPort(port);

    IRCBot();
}

void IRCBot::Connect()
{
    cout << "Connecting to " << this->server << ":" << this->port << " as " << this->nick << endl;

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


    server_socket = sockfd;

    Run();
}

void IRCBot::Run()
{
    Login();

    char buf[1024];

    memset(buf, 0, 1024);
    while (recv(server_socket, buf, 1024, 0) > 0)
    {
        string msg(buf);

        do
        {
            string msg1 = msg.substr(0, msg.find(CRLF, 0));
            Parse(msg1);
            msg = msg.substr(msg1.length()+2);
        }
        while (msg.length() > 1);

        memset(buf, 0, 1024);
    }

    cout << "Disconnected from server" << endl;
}

void IRCBot::Parse (string msg)
{
    cout << "< " << msg << endl;


}

void IRCBot::Login ()
{
    string s;
    s = "NICK " + this->nick;
    Send(s);

    s = "USER " + this->user + " 0 * :" + this->user;
    Send(s);
}


void IRCBot::Send (string msg)
{
    msg.append(CRLF);
    if (send(server_socket, msg.c_str(), msg.length(), 0) == (int) msg.length())
        cout << "> " << msg;
    else
        cout << "Message not sent completely: " << msg;
}
