#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX 1024




typedef struct msbuff {
	 msbuff(long type_, char* text_): type(type_)
    {
        strcpy(text, text_);
    }
	//default constructor
    msbuff(): type(0)
	{
		char text_[] = " ";
		strcpy(text, text_);
	}
    
    long type;
    char text[MAX];
} msg_t;


int main()
{
	system("touch Ping");
	system("touch Pong");
	key_t key_ping = ftok("Ping", 65);
	key_t key_pong = ftok("Pong", 66);
	
	
	if (key_ping == -1 || key_pong == -1)
	{
		perror("Ftok");
		return 1;
	}

	int ping_id = msgget(key_ping, 0666 | IPC_CREAT);
	int pong_id = msgget(key_pong, 0666 | IPC_CREAT);

	if (ping_id == -1 || pong_id == -1)
	{
		perror("Error occurred while trying to create/get message queue");
		return 1;
	}
	char ping_[] = "PING";
	msg_t ping(1, ping_);
	msg_t pong;
	
	printf("The game has started!\n");
	printf("If you want to stop it, please press ^C\n");
	printf("Enjoy (:\n\n");
	sleep(2);
	if (msgsnd(ping_id, &ping, MAX, 0) == -1)
	{
		perror("Unable to send a message");
		return 1;
	}
	printf("PING\n");
	
	int working = 1;
	while(working)
	{
		int counts = msgrcv(pong_id, &pong, sizeof(msg_t), 2, 0);
		if (counts == -1)
		{
			perror("Unable to receive a message");
			return 1;
		}
		else if (counts > 0)
		{
			if (strcmp(pong.text, "PONG") == 0)
			{
				
				if(msgsnd(ping_id, &ping, sizeof(ping), 0) == -1)
				{
					perror("Unable to send a message");
					return 1;
				}
				sleep(1);
				printf("PING\n");
			}
		}
	}


	return 0;
}