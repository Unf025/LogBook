#ifndef QSO_H
#define QSO_H
#include <string>
typedef struct {
    std::string date;
    std::string time;
    std::string callsign;
    std::string band;
    std::string mode;
    std::string rstr;
    std::string rsts;
    std::string name;
    std::string qth;
} TQSO;



#endif // QSO_H
