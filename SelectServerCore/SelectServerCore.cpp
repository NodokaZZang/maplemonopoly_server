#include "pch.h"
#include "SelectServerCore.h"
#include "Listener.h"
#include "Session.h"
SelectServerCore::SelectServerCore()
{
	
}

SelectServerCore::~SelectServerCore()
{
}

bool SelectServerCore::Register(Listener* listener)
{
    m_listener = listener;
	FD_ZERO(&m_fdSocketInfors);
	FD_SET(m_listener->GetSocket(), &m_fdSocketInfors);
	return true;
}

bool SelectServerCore::Register(Session* session)
{
	return false;
}

bool SelectServerCore::Release(Session* session)
{
	return false;
}

bool SelectServerCore::Dispatch()
{
	while (true)
	{
		SOCKET socketClient;
		SOCKET socketTemp;

		m_fdReads = m_fdSocketInfors;

		int iRet = select(0, &m_fdReads, 0, 0, 0);

		if (iRet == SOCKET_ERROR)
			return false;

        for (int i = 0; i < m_fdReads.fd_count; i++)
        {
            socketTemp = m_fdReads.fd_array[i];

            if (socketTemp == m_listener->GetSocket())
            {
                Session* result = m_listener->ProcessAccept();
                if (result)
                {
                    FD_SET(result->GetSocket(), &m_fdSocketInfors);
                    m_sessionFinder.insert({result->GetSocket(), result});
                }
            }
            else
            {
                auto iter = m_sessionFinder.find(socketTemp);
                if (iter != m_sessionFinder.end())
                {
                    Session* session = iter->second;
                    if (session->Recv() == false)
                    {
                        session->OnDisconnected();
                        m_sessionFinder.erase(session->GetSocket());
                        delete session;
                        FD_CLR(socketTemp, &m_fdSocketInfors);
                    }
                }
            }
        }
	}

	return true;
}
