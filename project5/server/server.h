#ifndef SERVER_H
#define SERVER_H

#define MSGSZ 128

struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
};

#endif