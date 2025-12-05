#ifndef QSO_H
#define QSO_H

typedef struct {
    char date[15];
    char time[10];
    char callsign[20];
    char band[20];
    char mode[10];
    char rstr[5];
    char rsts[5];
    char name[20];
    char qth[40];
    char remarks[100];
} TQSO;



#endif // QSO_H
