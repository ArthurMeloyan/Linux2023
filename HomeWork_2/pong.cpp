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
	key_t key_ping, key_pong;
	int ping_id, pong_id;
	key_ping = ftok("Ping", 65);
	key_pong = ftok("Pong", 66);

	if (key_ping == -1 || key_pong == -1)
	{
		perror("Ftok");
		return 1;
	}
	ping_id = msgget(key_ping, 0666 | IPC_CREAT);
	pong_id = msgget(key_pong, 0666 | IPC_CREAT);
	if (ping_id == -1 || pong_id == -1)
	{
		perror("Error occurred while trying to creat/get message queue");
		return 1;
	}

	char pong_[] = "PONG";
	msg_t ping;
	msg_t pong(2, pong_);
	printf("If you want to stop, please, press ^C too\n");
	int working = 1;
	while (working)
	{	int counts = msgrcv(ping_id, &ping, sizeof(msg_t), 1, 0);
		if (counts == -1)
		{
			perror("Unable to receive a message");
			return 1;
		}
		else if (counts > 0)
		{
			if (strcmp(ping.text, "PING") == 0)
			{
				
				if(msgsnd(pong_id, &pong, sizeof(pong), 0) == -1)
				{
					perror("Unable to send a message");
					return 1;
				}
				sleep(2.3);
				printf("PONG\n");
				
			}
		}
	}

	if (msgctl(ping_id, IPC_RMID, NULL) == -1 || msgctl(pong_id, IPC_RMID, NULL) == -1)
	{
		perror("Unable to delete message queue");
		return 1;
	}

	return 0;
}