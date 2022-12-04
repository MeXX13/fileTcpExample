#ifndef SAVEDCLIENTS_H
#define SAVEDCLIENTS_H

#include "socketthread.h"

class SavedClients
{
private:
    SavedClients();

public:
    QVector<socketThread *> mClients;  // клиенты
    static SavedClients& getInstance();
};

#endif // SAVEDCLIENTS_H
