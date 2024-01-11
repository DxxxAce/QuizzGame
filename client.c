#include "client.h"

void log_info(const char* message) {
    printf("%s", message);
    fflush (stdout);
}

int main()
{
    int server_socket;
    struct sockaddr_in server_addr;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("[client] Erorr occured while creating socket.\n");
      return errno;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
    {
      perror("[client] Erorr occured while connecting to server.\n");
      return errno;
    }
    
    char server_message[BUFFER_SIZE];
    char client_reply[BUFFER_SIZE];

    _Bool running = 1;
    
    while (running)
    {
      if (read(server_socket, server_message, BUFFER_SIZE) == -1)
      {
        perror("[client] Error occured while reading message from server.\n");
        return errno;
      }

      log_info(server_message);

      if (strcmp(server_message, QUIT_MESSAGE) == 0)
      {
        running = 0;
        break;
      }
      else if (strstr(server_message, "Q:") != NULL)
      {
        char* aux = strrchr(server_message, '[') + 1;
        strcpy(aux + strlen(aux) - 1, aux + strlen(aux));
        sleep(atoi(aux));

        
      }

      if (read (0, client_reply, BUFFER_SIZE) == -1)
      {
        perror("[client] Error occurred while reading from user input.\n");
        return errno;
      }

      log_info("[Client reply]: ");
      log_info(client_reply);

      if (strstr(client_reply, "login :"))
      {
        log_info("is log in command\n"); 

        while(1)
        {
          if (write(server_socket, client_reply, BUFFER_SIZE) == -1)
          {
            perror("[client] Error occured while sending message to server.\n");
            return errno;
          }

          if (read(server_socket, server_message, BUFFER_SIZE) == -1)
          {
            perror("[client] Error occured while reading message from server.\n");
            return errno;
          }

          if (strcmp(server_message, LOGIN_MESSAGE) == 0)
          {
            log_info(server_message);
            break;
          }

          memset(client_reply, 0, sizeof(client_reply));
        }

        memset(client_reply, 0, sizeof(client_reply));

        if (read (0, client_reply, BUFFER_SIZE) == -1)
        {
          perror("[client] Error occurred while reading from user input.\n");
          return errno;
        }
      }


      if (write(server_socket, client_reply, BUFFER_SIZE) == -1)
      {
        perror("[client] Error occured while sending message to server.\n");
        return errno;
      }

      memset(client_reply, 0, sizeof(client_reply));
    }

    close(server_socket);

    return 0;
}