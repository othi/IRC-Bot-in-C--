#include "apiserver.h"

APIServer::APIServer()
{
}

APIServer::APIServer(int port)
{
    this->port = port;
	this->guimode = false;
}

void APIServer::Run()
{
    // initialize socket list
    FD_ZERO(&db);

    // temporary socket list for select
    fd_set db_temp;

    // init temporary socket list
    FD_ZERO(&db_temp);

    // receiving buffer
    char buf[1024];
	

    // address structure
    struct sockaddr_in my_addr;
    struct sockaddr_in client;

    int clientfd, bytes_read;
	

	//weil chrissi failed und es nicht hinbekommt buf[0] mit "?" oder nem andern const char* zu vergleichen muss er nun nen string anlegen :(
	string temp;
    // listening socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // famille d'adresse
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // fix "address already in use"
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    // bind socket to port and address
    if(bind(server_socket, (struct sockaddr*) &my_addr, sizeof(my_addr)) != 0)
    {
        perror("bind failed -> ");
        exit(1);
    }

    // listen on socket
    if (listen(server_socket, MAX_CLIENTS) != 0)
    {
        perror("listen failed -> ");
        exit(2);
    }

    // add listening socket to set
    FD_SET(server_socket, &db);

    cout << "api server ready" << endl;

    // infinite loop
    while (1)
    {
        // select modifies the set
        db_temp = db;
        if (select(FD_SETSIZE, &db_temp, NULL, NULL, NULL) == -1)
        {
            close(server_socket);
            cout << "server closing" << endl;
            exit(0);
        }
        // loop through the set
        for (int i = 0; i<=FD_SETSIZE; i++)
        {
            // if there's a connection in the set
            if (FD_ISSET(i, &db_temp))
            {
                if (i == server_socket) // new connection!
                {
                    socklen_t sin_size = sizeof(struct sockaddr_in);
                    // accept connection
                    clientfd = accept(server_socket,
                                      (struct sockaddr*) &client, &sin_size);

                    if (clientfd < 0)
                    {
                        perror("accept failed ->");
                        exit(4);
                    }
                    else
                    {
                        // connected
                        FD_SET(clientfd, &db); // save to set
                        cout << "new connection!" << endl;
                    }
                }
                else // a client sends something
                {
                    // reset buffer
                    memset(buf, '\0', 1024);

                    if ((bytes_read = recv(i, buf, 1024, 0)) < 0)
                    {
                        close(i);
                        perror("recv failed ->");
                        exit(5);
                    }
                    if (bytes_read == 0) // client disconnected
                    {
                        close(i);
                        FD_CLR(i, &db); // remove from set
                        cout << "client disconnected" << endl;
                        continue;
                    }
                    else // client sends a message
                    {
						temp = buf;
						
						if(temp == "sup.i am a gui and no chat client.") this->guimode = true;
						if(temp.substr(0,4)!="QUIT" && temp.substr(0,4)!="PONG" && !this->guimode) {
							vector<string> tokens = Common::split(temp, ' ');
							if(tokens.size()>2) {
								// temp = (string)tokens[2];
								// temp = temp.substr(0,2); // hier stirbt er grad wegen file descriptor...
								
								if(temp==":?") {
									tokens[2]=tokens[2].substr(2);
									this->interpret(tokens);
								}
							
							else
								bot->Raw(buf);
							}

						}
						else if(guimode) {
							cout << "command from client received." << endl;
							vector<string> tokens = Common::split(temp, ' ');
							this->interpret(tokens);
						}
	
                    }
                }
            }
        }
    }
    // close socket
    close(server_socket);
}

void APIServer::SendRawToClients (string message)
{
    int j;
    // loop through connections
    for (j = 0; j<FD_SETSIZE; j++)
    {
        // don't send it to us
        if (FD_ISSET(j, &db) && j != server_socket)
        {
            // send message
            if (send(j, message.c_str(), message.length(), 0) < 0)
            {
                perror("send failed ->");
                exit(6);
            }
        }
    }
}

void APIServer::interpret(vector<string> tokens) {
	unsigned int i;
	if(!this->guimode) 
		i = 2;
	else 
		i = 0;
	
		if(tokens[i]=="join") {
		if(tokens.size()==(i+2))
			this->bot->Join(tokens[i+1]);
		else 
			answer("You have to specify a channel",tokens[1]);
		if(tokens.size()==(i+3))
			this->bot->Join(tokens[i+1],tokens[i+2]);
		}
	
	else if(tokens[i]=="nick") {
		if(tokens.size()==(i+2))
			this->bot->Nick(tokens[i+1]);
		else 
			answer("You have to specify a channel",tokens[1]);
	}
}

void APIServer::answer(string message,string target=false) {
	if(this->guimode) {
		this->SendRawToClients("message");
	}
	else this->SendRawToClients("Master!Master@universe.net PRIVMSG "+target+" :"+message);
}
