#include "server.h"

void log_info(const char* message) {
    printf("%s", message);
    fflush (stdout);
}

static void* handle_connection(void* arg)
{
    struct thread_data td;
	td = *((struct thread_data*)arg);
    pthread_detach(pthread_self());
	play_game(td);
    close ((intptr_t)arg);
    return NULL;
};

void play_game(struct thread_data td)
{
    char server_message[BUFFER_SIZE] = QUESTION1;
    char client_message[BUFFER_SIZE];
    char correct_answer;
    _Bool running = 1;

    client_login(td);

    // FILE* xml_file;
    // xml_file = fopen("quizz_data.xml", "r");
    // if (xml_file == NULL)
    // {
    //     perror("[server] Error, XML file missing.");
    //     return;
    // }

    // get_questions(xml_file);

    while (running)
    {
        read(td.client_socket, client_message, BUFFER_SIZE);

        strcpy(client_message + strlen(client_message) - 1, client_message + strlen(client_message));

        //log_info(client_message);

        if (strcmp(client_message, "quit") == 0)
        {
            running = 0;
            strcpy(server_message, QUIT_MESSAGE);
            write(td.client_socket, server_message, BUFFER_SIZE);

            close(thread->client_socket);
            pthread_exit(NULL);
        }
        else if (strcmp(client_message, "play") == 0)
        {
            pthread_mutex_lock(&lock);
            
            if (!is_game_started())
            
            {
                start_time = time(NULL);
            }

            pthread_mutex_unlock(&lock);

            for (int i = sync_to_question(); i < QUESTION_COUNT; i++)
            {
                sprintf(server_message, QUESTION1, sync_new_player(i));
                write(td.client_socket, server_message, BUFFER_SIZE);
                read(td.client_socket, client_message, BUFFER_SIZE);

                strcpy(client_message + strlen(client_message) - 1, client_message + strlen(client_message));

                if (strcmp(client_message, "quit") == 0)
                {
                    running = 0;
                    strcpy(server_message, QUIT_MESSAGE);
                    write(td.client_socket, server_message, BUFFER_SIZE);

                    close(thread->client_socket);
                    pthread_exit(NULL);
                }
                else
                {
                    log_info(client_message);

                    if (strcmp(client_message, ANSWER1) == 0)
                    {
                        td.client_score += 10;
                    }
                }
            }

            sprintf(server_message, END_GAME_MESSAGE, td.client_score);
            td.client_score = 0;
            write(td.client_socket, server_message, BUFFER_SIZE);
        }
    }

    //fclose(xml_file);
};

void client_login(struct thread_data td)
{	
    char client_request[BUFFER_SIZE];
    char server_response[BUFFER_SIZE] = WELCOME_MESSAGE;

    _Bool login_status = 0;
    
    while(!login_status)
    {
        write(td.client_socket, server_response, BUFFER_SIZE);
        read(td.client_socket, client_request, BUFFER_SIZE );

        strcpy(client_request + strlen(client_request) -1, client_request + strlen(client_request) );

        if (strncmp(client_request, "login : ", strlen("login : ")) == 0)
        {
            td.client_username = (char*)(client_request + strlen("login : "));

            if (td.client_username != NULL && td.client_username[0] != '\0')
            {
                login_status = 1;
                strcpy(server_response, LOGIN_MESSAGE);
                write(td.client_socket, server_response, BUFFER_SIZE);
            }
            else
            {
                strcpy(server_response, INVALID_USERNAME_MESSAGE);
            }
        }
    }
};

_Bool is_game_started()
{
    if (start_time != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int sync_to_question()
{
    return (time(NULL) - start_time) / QUESTION_TIME;
}

int sync_new_player(int i)
{
    return start_time + (i + 1) * QUESTION_TIME - time(NULL);
}

// void get_questions(FILE* xml)
// {
//     int i = 0;
//     while (fscanf(xml,"<qa>") == 1 && i < QUESTION_COUNT)
//     {
//         fscanf(xml, "<question>");

//         i++;
//     }
// }

int main()
{
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    _Bool running = 1;
    pthread_t thread_counter[MAX_THREADS];
	int thread_index=0;
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        perror("[server] Mutex init has failed\n");
        return errno;
    }

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("[server] Error occured while creating socket.\n");
        return errno;
    }

    int on=1;
    setsockopt(server_socket ,SOL_SOCKET ,SO_REUSEADDR ,&on ,sizeof(on));

    bzero (&server_addr, sizeof(server_addr));
    bzero (&client_addr, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("[server] Error occured while binding server.\n");
        return errno;
    }

    thread = (struct thread_data*)malloc(MAX_THREADS * sizeof(struct thread_data));

    if (listen(server_socket, SERVER_BACKLOG) == -1)
    {
        perror("[server] Error occured while listening for client connections.\n");
        return errno;
    }

    printf ("[server] Waiting at port %d...\n", PORT);
    fflush (stdout);

    while (running)
    {
        int client;
        unsigned int length = sizeof(client_addr);

        if ((client = accept(server_socket, (struct sockaddr*)&client_addr, &length)) == -1)
        {
            perror("[server] Error occured while accepting a client connection.\n");
            continue;
        }

        thread->client_id = thread_index++;
        thread->client_socket = client;

        player_counter++;

        // TODO: clear thread counter
        // handle player counter decrease when some one is quiting.
        if (pthread_create(&thread_counter[thread_index], NULL, &handle_connection, thread) != 0)
        {
            perror("[server] Error occurred while attempting to create a new thread.\n");
        }
    }

    return 0;
}

