#ifndef SERVER_H
#define SERVER_H

#define MSGSZ 256

struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
};

#endif