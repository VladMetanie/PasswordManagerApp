#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/wait.h>


#define PORT 3176

#define Server "ManagerParole"

extern int errno;

void fnc_treat_zombies(int SignalNr)
{
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}

int fileExists(const char *filename) {
    return access(filename, F_OK) != -1;
}

void sendMessageAndRecieveRespone(int client, char *mesaj, char *response){
            write(client, mesaj, 1000);
            read(client, response, 1000);
}

char* readWholeFile(char file_path[100]){
    
    FILE *file = fopen(file_path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", file_path);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, file_size, file);
    if (bytesRead != file_size) {
        fprintf(stderr, "Error reading file: %s\n", file_path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(file);

    return buffer;
}


int main()
{
  struct sockaddr_in server; 
  struct sockaddr_in from;
  int sd; 
  int pid;
  char msg[1000];
  char rasp[100];
  int on = 1;
  signal(SIGCHLD, fnc_treat_zombies);

  

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[server]Eroare la socket().\n");
    return errno;
  }
  

  sd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

  
  bzero(&server, sizeof(server));
  bzero(&from, sizeof(from));

  
  server.sin_family = AF_INET;
  
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  
  server.sin_port = htons(PORT);

  
  if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[server]Eroare la bind().\n");
    return errno;
  }

  
  if (listen(sd, 2) == -1)
  {
    perror("[server]Eroare la listen().\n");
    return errno;
  }

  while (1)
  {
    int client;
    
    int length = sizeof(from);

    printf("[server]Asteptam la portul %d...\n", PORT);
    fflush(stdout);

    
    if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
    {
      perror("[server]Eroare la accept().\n");
      continue;
    }
    int sockp[2];
    pid = fork();
    if (pid == -1)
    {
      printf("Eroare la fork\n");
      return -1;
    }
    printf("[server]S-a conectat un utilizator\n");
    fflush(stdout);
    if (pid > 0)
    {
      close(sd);
      while (1)
      {
        printf("Citim noua comanda\n");
        bzero(msg, 1000);
        fflush(stdout);
        int mesj = read(client, msg, 1000);

        if (mesj < 1)
        {
          perror("[server]Eroare la citirea de la client\n");
          return -1;
          continue;
        }
        printf("[server]Comanda primita de la client\n");
        printf("Comanda: %s", msg);
        fflush(stdout);

        if (strcmp(msg, "register\n") == 0){
            printf("Register\n");
            bzero(msg, 1000);
            strcpy(msg, "Introduceti userul noului cont:");
            printf("[server]Se trimite mesajul catre client %s\n", rasp);

            if (write(client, msg, 1000) <= 0)
            {
              perror("Eroare la scrierea catre client!");
              close(client);
              exit(0);
            }

            char newusername[1000];
            char newpassword[1000];
            char newcoppypassword[1000];

            bzero(newusername, 1000);
            fflush(stdout);
            int mesj = read(client, newusername, 1000);

            if (mesj < 1)
            {
              perror("[server]Eroare la citirea de la client\n");
              return -1;
              continue;
            }

            while(1){
              bzero(msg, 1000);
              strcpy(msg, "Introduceti parola noului cont:");
              sendMessageAndRecieveRespone(client, msg, newpassword);

              bzero(msg, 1000);
              strcpy(msg, "Introduceti din nou parola noului cont:");
              sendMessageAndRecieveRespone(client, msg, newcoppypassword);

              if(strcmp(newpassword, newcoppypassword) == 0){
                break;
              }else{
                bzero(msg, 1000);
                strcpy(msg, "Parolele nu coincid. Apsati enter pt a continua");
                sendMessageAndRecieveRespone(client, msg, newcoppypassword);
              }
            }

            bzero(msg, 1000);
            strcpy(msg, "Apasati enter pentru a continua");
            sendMessageAndRecieveRespone(client, msg, newpassword);

            bzero(msg, 1000);
            strcpy(msg, "Introduceti o noua comanda:");
            write(client, msg, 1000);

            FILE *usersFile = fopen("data/users.txt", "a");
            newusername[strlen(newusername) - 1] = 0;
            fprintf(usersFile, "%s %s",newusername, newcoppypassword );

            fclose(usersFile);

        }



        if (strcmp(msg, "logare\n") == 0)
        {
          while (1)
          {
            bzero(msg, 1000);
            strcpy(msg, "Introduceti userul");
            printf("[server]Se trimite mesajul catre client %s\n", rasp);

            if (write(client, msg, 1000) <= 0)
            {
              perror("Eroare la scrierea catre client!");
              close(client);
              exit(0);
            }

            bzero(msg, 1000);
            fflush(stdout);
            int mesj = read(client, msg, 1000);

            if (mesj < 1)
            {
              perror("[server]Eroare la citirea de la client\n");
              return -1;
              continue;
            }

            printf("[server]Comanda primita de la client\n");
            printf("Comanda: %s", msg);

            fflush(stdout);

            if (strcmp(msg, "quit\n") == 0)
            {
              return -1;
            }
            msg[strlen(msg)-1] = 0;
            
            printf("Mesajul primit de la client este[%s]\n", msg);


            FILE *file = fopen("data/users.txt", "r");

            char buffer[200];
            char username[100];
            char password[100];
            int found = 0;


            while (fgets(buffer, sizeof(buffer), file) != NULL) {
              
              if (sscanf(buffer, "%s %s", username, password) == 2) {
                  
                  if(strcmp(username, msg) == 0){
                    found = 1;
                    break;
                  }
              } else {
                  fprintf(stderr, "Invalid format in line: %s", buffer);
              }
            }

            if (found == 1)
            {
              while (1)
              {

                strcpy(msg, "Introduceti parola\n");
                fflush(stdout);
                printf("[server]Se trimite mesajul catre client %s\n", rasp);
                if (write(client, msg, 1000) <= 0)
                {
                  perror("Eroare la scrierea catre client!");
                  close(client);
                  exit(0);
                }

                bzero(msg, 1000);
                fflush(stdout);
                int mesj = read(client, msg, 1000);

                if (mesj < 1)
                {
                  perror("[server]Eroare la citirea de la client\n");
                  return -1;
                  continue;
                }
                printf("[server]Comanda primita de la client\n");
                printf("Comanda: %s", msg);
                msg[strlen(msg)-1] = 0;

                if (strcmp(msg, password) == 0)
                {
                  while(1){
                    fflush(stdout);
                    bzero(msg, 1000);
                    strcpy(msg, "show, add, newpassword, delete");
                    if (write(client, msg, 1000) <= 0)
                      {
                        perror("Eroare la scrierea catre client!");
                        close(client);
                        exit(0);
                      }
                    bzero(msg, 1000);
                    fflush(stdout);
                    int mesj = read(client, msg, 1000);

                    if(strcmp(msg,"show\n") == 0){
                      bzero(msg, 1000);
                      fflush(stdout);

                      char aux[100];
                      strcpy(aux, "data/");
                      strcat(aux, username);
                      strcat(aux, ".txt");


                      char ceva[1000];
                      if(!fileExists(aux)){
                        sendMessageAndRecieveRespone(client, "Nu avem categorii. Apasati enter pentru a continua\n", ceva );
                        continue;
                      }

                      FILE *categorylist = fopen(aux, "r");

                      char buffer[100];
                      strcpy(msg, "");
                      while (fgets(buffer, sizeof(buffer), categorylist) != NULL) {
                          buffer[strlen(buffer) - 1] = 0;
                          char aux1[200];
                          strcpy(aux1, "data/");
                          strcat(aux1, username);
                          strcat(aux1, "_");
                          strcat(aux1, buffer);
                          //aux1[strlen(aux1)-1] = 0;
                          strcat(aux1, ".txt");

                          strcat(msg, "\n");
                          strcat(msg, "\n");
                          strcat(msg, "Categoria: ");
                          strcat(msg, buffer);
                          strcat(msg, "\n");

                          char buffer1[100];
                          FILE *categoryFile = fopen(aux1, "r");
                          while (fgets(buffer1, sizeof(buffer1), categoryFile) != NULL) {
                            
                            char *token = strtok(buffer1, " \n");

                            strcat(msg, "Titlul: ");
                            strcat(msg, token);

                            token = strtok(NULL, " \n");

                            strcat(msg, " Username: ");
                            strcat(msg, token);

                            token = strtok(NULL, " \n");

                            strcat(msg, " Parola: ");
                            strcat(msg, token);

                            token = strtok(NULL, " \n");

                            strcat(msg, " URL: ");
                            strcat(msg, token);

                            token = strtok(NULL, " \n");

                            fgets(buffer1, sizeof(buffer1), categoryFile);

                            strcat(msg, "\n");
                            strcat(msg, "Notite: ");
                            strcat(msg, buffer1);
                            strcat(msg, "\n");
                          }

                      }

                      strcat(msg, "Apasati pe enter pt a continua");
                      write(client, msg, 1000);
                      read(client, msg, 1000);
                    }
                    if(strcmp(msg,"add\n") == 0){
                      strcpy(msg, "Numele categoriei de adaugat:\n");
                      write(client, msg, 1000);

                      bzero(msg, 1000);
                      fflush(stdout);
                      int mesj = read(client, msg, 1000);

                      msg[strlen(msg) - 1] = 0;

                      char aux[100];
                      strcpy(aux, "data/");
                      strcat(aux, username);
                      strcat(aux, ".txt");
                      FILE *categorylist = fopen(aux, "a");
                      fprintf(categorylist, "%s\n" ,msg);
                      fclose(categorylist);

                      strcpy(aux, "data/");
                      strcat(aux, username);
                      strcat(aux, "_");
                      strcat(aux, msg);
                      strcat(aux, ".txt");
                      FILE *categoryFile = fopen(aux, "a");


                      if (categoryFile == NULL) {
                          perror("Error creating file");
                          return 1; 
                      }

                      fclose(categoryFile);
                    }
                    if(strcmp(msg,"newpassword\n") == 0){
                      strcpy(msg, "Numele categoriei la care adaugam parola:\n");
                      write(client, msg, 1000);

                      bzero(msg, 1000);
                      fflush(stdout);
                      read(client, msg, 1000);
                      msg[strlen(msg) - 1] = 0;

                      char aux[200];
                      strcpy(aux, "data/");
                      strcat(aux, username);
                      strcat(aux, "_");
                      strcat(aux, msg);
                      strcat(aux, ".txt");

                      if(!fileExists(aux)){
                        bzero(msg, 1000);
                        strcpy(msg, "Categorie inexistenta. Apasati enter pentru a continua.\n");
                        write(client, msg, 1000);
                        read(client, msg, 1000);
                        fflush(stdout);
                        continue;
                      }else{
                        char parola[1000], titlu[1000], url[1000], username[1000], notite[1000];

                        strcpy(msg, "Titlul :\n");
                        write(client, msg, 1000);

                        bzero(msg, 1000);
                        fflush(stdout);
                        read(client, titlu, 1000);
                        titlu[strlen(titlu) - 1] = 0;

                        strcpy(msg, "Username :\n");
                        write(client, msg, 1000);

                        bzero(msg, 1000);
                        fflush(stdout);
                        read(client, username, 1000);
                        username[strlen(username) - 1] = 0;

                        strcpy(msg, "Parola:\n");
                        write(client, msg, 1000);

                        bzero(msg, 1000);
                        fflush(stdout);
                        read(client, parola, 1000);
                        parola[strlen(parola) - 1] = 0;

                        strcpy(msg, "Url:\n");
                        write(client, msg, 1000);

                        bzero(msg, 1000);
                        fflush(stdout);
                        read(client, url, 1000);
                        url[strlen(url) - 1] = 0;

                        strcpy(msg, "Notite:\n");
                        write(client, msg, 1000);

                        bzero(msg, 1000);
                        fflush(stdout);
                        read(client, notite, 1000);
                        notite[strlen(notite) - 1] = 0;

                        FILE* categoryFile = fopen(aux, "a");

                        fprintf(categoryFile, "%s %s %s %s\n%s\n", titlu, username, parola, url, notite);

                        fclose(categoryFile);
                      }
                    }

                    if(strcmp(msg,"delete\n") == 0){
                      char category[100];
                      char title[100];

                      bzero(msg, 1000);
                      strcpy(msg, "Introduceti categoria de la care doriti sa stergeti:");
                      sendMessageAndRecieveRespone(client, msg, category);

                      bzero(msg, 1000);
                      strcpy(msg, "Introduceti titlul care doriti sa il stergeti:");
                      sendMessageAndRecieveRespone(client, msg, title);

                      category[strlen(category) - 1] = 0;
                      title[strlen(title) - 1] = 0;

                      char aux[200];

                      strcpy(aux, "data/");
                      strcat(aux, username);
                      strcat(aux, "_");
                      strcat(aux, category);
                      strcat(aux, ".txt");

                      if(!fileExists(aux)){
                        bzero(msg, 1000);
                        strcat(msg, "Nu s-a gasit elementul. Apasati enter pentru a continua");
                        write(client, msg, 1000);
                        read(client, msg, 1000);
                        continue;
                      }

                      char* buf = readWholeFile(aux);

                      char *s;

                      s = strtok(buf, "\n");
                      FILE *f = fopen(aux, "w");
                      while(s != NULL){
                        if(strncmp(s, title, strlen(title)) == 0){
                            s = strtok(NULL, "\n");
                            s = strtok(NULL, "\n");
                        }else{
                          fprintf(f, "%s\n", s);
                          s = strtok(NULL, "\n");
                          fprintf(f, "%s\n", s);
                          s = strtok(NULL, "\n");
                        }
                      }
                      bzero(msg, 1000);
                      strcat(msg, "Delete cu succes. Apasati pe enter pt a continua");
                      write(client, msg, 1000);
                      read(client, msg, 1000);
                      fclose(f);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}