#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{

    string m1;
    string m2;
    int bando;

    int fd[2], nbytes;
    pid_t childpid;
    //string readbuffer;
    char readbuffer[80], readbuffer2[80];
    pipe(fd);

    cout << "Seleccione su bando (0 - Alianza/1 - Horda): ";
    cin >> bando;

    switch (bando)
    {
    case 0:
        m1 = "For the Alliance.";
        m2 = "Yes, my King.";
        break;
    case 1:
        m1 = "For the Horde.";
        m2 = "Yes, Warchief.";
        break;
    default:
        m1 = "I'm a level 1... critter.";
        m2 = "Got that right... n00b...";
        break;
    }

    if ((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        //cout << m1 << endl;
        close(fd[0]);
        write(fd[1], m1.c_str(), m1.size());
        exit(0);
    }

    else
    {
        close(fd[1]);
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        //nbytes = read(fd[0], readbuffer.c_str(), readbuffer.size());
        cout << readbuffer << endl;
        cout << m2 << endl;
        //printf("recieved string: %s", readbuffer);
    }

    return 0;
}
