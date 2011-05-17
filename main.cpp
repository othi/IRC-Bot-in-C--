#include "main.h"

int main(int argc, char *argv[])
{
    pthread_t api_thread;
    pthread_t bot_thread;

    APIServer* api = new APIServer(31337);

    cout << "a " << api << endl;

    if (pthread_create(&bot_thread, NULL, &createBotThread, (void*) api) != 0)
    {
        cout << "error creating bot thread" << endl;
    }

    api->Run();

    //pthread_join(api_thread, NULL);
    pthread_join(bot_thread, NULL);
}
/*
void* createAPIThread (void* api)
{
    api = (void*) (new APIServer());
    ((APIServer*) api)->Run();

    pthread_exit(0);
    return api;
}
*/
void* createBotThread (void* api)
{
    IRCBot* bot = new IRCBot((APIServer*) api);
    bot->setNick("chrissibot");
    bot->setUser("chrissibot");
    bot->setServer("irc.gamesurge.net");
    bot->setPort("6667");

    bot->Connect();

    pthread_exit(0);
    return api;
}
