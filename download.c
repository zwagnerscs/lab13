#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libsocket/libinetsocket.h>

FILE * connect_to_server();
void menu();
char get_choice();
void list_files(FILE *s);
void download(FILE *s);
void quit(FILE *s);

int main()
{
    FILE * s = connect_to_server();
    printf("\n");
    char choice;
    do {
        menu();
        choice = get_choice();
        printf("\n");
        switch(choice)
        {
            case 'l':
            case 'L':
                list_files(s);
                printf("\n");
                break;
        
            case 'd':
            case 'D':
                download(s);
                printf("\n");
                break;
            
            case 'q':
            case 'Q':
                quit(s);
                exit(0);
                break;
            
            default:
                printf("Choice must be [D], [L], or [Q]\n\n");
        }
    } while (toupper(choice) != 'Q');
}

FILE * connect_to_server()
{
    int socketfd = create_inet_stream_socket("runwire.com", "1234", LIBSOCKET_IPv4, 0);
    if(!socketfd){
        printf("Can't connect to server.");
        exit(1);
    }
    FILE * s = fdopen(socketfd, "r+");
    char buffer[1000];
    char * code;
    fgets(buffer, 1000, s);
    printf("%s", buffer);
    code = strstr(buffer, "+OK");
    if (code == NULL) {
        exit(1);
    }
    
    return s;
}

void menu()
{
    printf("List files [L]\n");
    printf("Download a file [D]\n");
    printf("Quit [Q]\n");
    printf("\n");
}

char get_choice()
{
    printf("Your choice: ");
    char buf[100];
    do {
        fgets(buf, 100, stdin);
    } while (buf[0] == '\n');
    return buf[0];
}

void list_files(FILE *s)
{
    fprintf(s, "LIST\n");
    char buffer[1000];
    int size;
    char * file_name;
    int count = 0;
    printf ("%s\n", fgets(buffer, 1000, s));
    while(*fgets(buffer, 1000, s) != '.'){
        sscanf (buffer, "%d", &size);
        float new_size = (float)size / 1024;
        printf ("%d | ", count);
        count = count + 1;
        printf("%.2f KB", new_size);
        file_name = strstr(buffer, " ");
        printf(" |%s\n", file_name);
    }
    
    
}

void download(FILE *s)
{
    char data[1000];
    char buffer[1000];
    char * code;
    printf("Enter a file name: ");
    scanf("%s", data);
    fprintf(s, "SIZE %s\n", data);
    fgets(buffer, 1000, s);
    printf("%s", buffer);
    code = strstr(buffer, "+OK");
    if (code == NULL) {
        printf("Error!\n");
        exit(1);
    }
    
    int size;
    sscanf(buffer, "+OK %d", &size);
    printf("%d\n", size);
    
    fprintf (s, "GET %s\n", data);
    fgets(buffer, 1000, s);
    printf("%s", buffer);
    code = strstr(buffer, "+OK");
    if (code == NULL) {
        printf("Error!");
        exit(1);
    }
    
    FILE * f = fopen (data, "w");
    int bytes;
    while (size > 1000) {
        bytes = fread(data, 1, 1000, s);
        size = size - bytes;
        fwrite(data, 1, bytes, f);
    }
    while (size > 0) {
        bytes = fread(data, 1, size, s);
        size = size - bytes;
        fwrite(data, 1, bytes, f);
    }
    fclose(f);
}


void quit(FILE *s)
{
    fclose(s);
}