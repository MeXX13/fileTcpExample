#include "savedclients.h"

SavedClients::SavedClients()
{

}

SavedClients& SavedClients::getInstance()
{
    static SavedClients INSTANCE;
    return INSTANCE;
}


