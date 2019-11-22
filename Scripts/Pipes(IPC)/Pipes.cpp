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

    int n = 10000;
    int array[n];
    int nums[n];
    int k = 0;
    int answer[argc];

    //this loop makes an array or prime numbers acording to the node
    //if that node number is not a prime number it puts a 0 in that node
    for (int i = 2; i < n; i++)
    {
        for (int j = (i * i); j < n; j += i)
        {
            array[j - 1] = 1;
        }
    }
    //this loop makes an array of all the prime numbers;
    for (int i = 1; i < n; i++)
    {
        if (array[i - 1] == 0)
        {
            nums[k] = i;
            k++;
        }
    }
    //this loop puts the selected prime numbers in the array
    for (int j = 1; j < argc; j++)
    {
        int num = atoi(argv[j]);
        answer[j - 1] = nums[num];
    }
    //this loop prints the primes for testing
    for (int i = 1; i < argc; i++)
        cout << answer[i - 1] << endl;

    int fd[2], nbytes;
    pid_t childpid;
    string number;
    char readbuffer[80];
    string numbers;
    pipe(fd);

    if ((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (childpid == 0)
    {
        for (int i = 1; i < argc; i++)
        {
            number = to_string(answer[i - 1]);
            cout << number << endl;
            numbers = numbers + " " + number;
        }
        string word[] = {numbers};
        cout << word << endl;
        close(fd[0]);
        write(fd[1], word, (numbers.length() + 1));
        exit(0);
    }

    else
    {
        close(fd[1]);
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
        printf("recieved string: %s", readbuffer);
    }

    return 0;