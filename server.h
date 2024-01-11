#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include "shared_data.h"

#define MAX_THREADS 10
#define SERVER_BACKLOG 5
#define USERNAME_SIZE 16

#define QUESTION_COUNT 5
#define QUESTION_TIME 10

extern int errno;

int player_counter = 0, max_score = 0;
long start_time = 0;

typedef struct thread_data
{
	int client_id, client_score;
	int client_socket;
	char* client_username;
} thread_data;

thread_data *thread;
pthread_mutex_t lock;

char questions[QUESTION_COUNT][BUFFER_SIZE];
char answers[QUESTION_COUNT];

void log_info(const char*);
static void* handle_connection(void*);
void play_game(struct thread_data);
void client_login(struct thread_data);
_Bool is_game_started();
int sync_to_question();
int sync_new_player(int);
void get_questions(FILE*);
