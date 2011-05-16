#ifndef MAIN_H
#define MAIN_H

#define _BSD_SOURCE

#include <unistd.h>

#include "ircbot.h"
#include "apiserver.h"

int main (int argc, char* argv[]);

void* createAPIThread (void* api);
void* createBotThread (void* api);

#endif // MAIN_H
