#include "main.h"

int main(int argc, char *argv[])
{

    IRCBot bot("othibot", "othibot", "irc.gamesurge.net", "6667");

    bot.setNick("othibot");
    bot.setUser("othibot");
    bot.setServer("irc.gamesurge.net");
    bot.setPort("6667");

    bot.Connect();
}
