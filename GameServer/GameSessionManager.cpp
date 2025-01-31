#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager* GSessionManager = nullptr;

void GameSessionManager::Add(GameSessionRef session)
{
    WRITE_LOCK;
    _sessions.insert(session);
}

void GameSessionManager::Remove(GameSessionRef session)
{
    WRITE_LOCK;
    _sessions.erase(session);
}

void GameSessionManager::Broadcast(SendBufferRef sendbuffer)
{
    WRITE_LOCK;
    for (GameSessionRef session : _sessions)
    {
        session->Send(sendbuffer);
    }
}
