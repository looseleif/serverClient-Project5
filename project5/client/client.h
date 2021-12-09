#ifndef CLIENT_H
#define CLIENT_H

#define MSGSZ 256

struct msgbuf {
   long mtype;
   char mtext[MSGSZ];
};

#endif