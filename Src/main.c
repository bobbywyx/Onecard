#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "server.h"

#include "main.h"

void print_help();
void print_logo();

int main(int argc, char *argv[])
{
    int n=4,c=5,d=2,r=1;
    bool a = false;
    bool arg_valid_flag = false;
    char log_path[100] = "log.txt";
    int opt = 0;
    while (1) {
        int option_index = 0;
        struct option long_options[] = {
                {"help",            no_argument,       0,  'h' },
                {"version",         no_argument,       0,  'v' },
                {"log",             required_argument, 0,  'l' },
                {"player-number",   required_argument, 0,  'n' },
                {"initial-cards",   required_argument, 0,  'c' },
                {"decks",           required_argument, 0,  'd' },
                {"rounds",          required_argument, 0,  'r' },
                {"auto",            no_argument,       0,  'a' },
                {0,                 0,                 0,  0   }
        };
        opt = getopt_long(argc, argv, "hvn:c:d:r:a", long_options, &option_index);
        if (opt == -1) break;
        arg_valid_flag = true;
        switch (opt) {
            case 0:  // long options with argument
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                break;
            case 'h':
                print_help();
                break;
            case 'v':
                printf("version:  "VERSION"\n");
                exit(EXIT_SUCCESS);
            case 'l':
                strcpy(log_path, optarg);
                printf("log file name: %s", log_path);
                break;
            case 'n':
                n= atoi(optarg);
                break;
            case 'c':
                c= atoi(optarg);
                break;
            case 'd':
                d= atoi(optarg);
                break;
            case 'r':
                r= atoi(optarg);
                break;
            case 'a':
                printf("auto run start\n");
                a = true;
                break;
            case '?':
                arg_valid_flag = false;
                break;
            default:
                arg_valid_flag = false;
                printf("?? getopt returned character code 0%o ??\n", opt);
        }
    }
    if (optind < argc) {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
            printf("%s ", argv[optind++]);
        printf("\n");
    }
    print_logo();
    if(argc ==1)  printf("\n \n \nrequire arguments, see by    -h | --help \n");
    else if(!arg_valid_flag || n < 2 || c < 2 || d < 2 || r < 1 ) printf("Wrong input argument(s), please check by -h pr --help\n");
    else server_init(n, c, d, r,a,log_path);
    exit(EXIT_SUCCESS);
}

void print_help(){
    printf(""
           "-h | --help                print this help message\n"
           "-v | --version             print version\n"
           "--log filename             write the logs in filename (default: onecard.log)\n"
           "-n n | --player-number=n   n players, n must be larger than 2 (default: 4)\n"
           "-c c | --initial-cards=c   deal c cards per player, c must be at least 2 (default: 5)\n"
           "-d d | --decks=d           use d decks 52 cards each, d must be at least 2 (default: 2)\n"
           "-r r | --rounds=r          play r rounds, r must be at least 1 (default: 1)\n"
           "-a   | --auto              run in demo mode\n"
           );
    exit(EXIT_SUCCESS);
}

void print_logo(){
    printf("\n"
           " _____       __  __      ____                ____       ______      ____        ____      \n"
           "/\\  __`\\    /\\ \\/\\ \\    /\\  _`\\             /\\  _`\\    /\\  _  \\    /\\  _`\\     /\\  _`\\    \n"
           "\\ \\ \\/\\ \\   \\ \\ `\\\\ \\   \\ \\ \\L\\_\\           \\ \\ \\/\\_\\  \\ \\ \\L\\ \\   \\ \\ \\L\\ \\   \\ \\ \\/\\ \\  \n"
           " \\ \\ \\ \\ \\   \\ \\ , ` \\   \\ \\  _\\L            \\ \\ \\/_/_  \\ \\  __ \\   \\ \\ ,  /    \\ \\ \\ \\ \\ \n"
           "  \\ \\ \\_\\ \\   \\ \\ \\`\\ \\   \\ \\ \\L\\ \\           \\ \\ \\L\\ \\  \\ \\ \\/\\ \\   \\ \\ \\\\ \\    \\ \\ \\_\\ \\\n"
           "   \\ \\_____\\   \\ \\_\\ \\_\\   \\ \\____/            \\ \\____/   \\ \\_\\ \\_\\   \\ \\_\\ \\_\\   \\ \\____/\n"
           "    \\/_____/    \\/_/\\/_/    \\/___/              \\/___/     \\/_/\\/_/    \\/_/\\/ /    \\/___/ \n"
           "                                                                                            \n"
           "                                                                                            \n");
}
