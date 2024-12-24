#include "pch.h"
#include "accountManager.h"
#include "PlayerManager.h"

AccountManager GAccountManager;

void AccountManager::AccountThenPlayer()
{
    WRITE_LOCK;
    GPlayerManager.Lock();
}

void AccountManager::Lock()
{
    WRITE_LOCK;
}