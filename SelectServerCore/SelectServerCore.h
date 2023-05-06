#pragma once
class Session;
class Listener;

struct fd_set_ex : fd_set 
{
    Session* fd_session[FD_SETSIZE];
};

#define FD_CLR_EX(fd, set, session) do {                                                    \
    u_int __i;                                                                              \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count ; __i++) {                          \
        if (((fd_set FAR *)(set))->fd_array[__i] == fd) {                                   \
            while (__i < ((fd_set FAR *)(set))->fd_count-1) {                               \
                ((fd_set FAR *)(set))->fd_array[__i] =                                      \
                    ((fd_set FAR *)(set))->fd_array[__i+1];                                 \
                                                                                            \
((fd_set_ex FAR*)(set))->fd_session[__i] = ((fd_set_ex FAR*)(set))->fd_session[__i + 1];    \
__i++;                                                                                      \
                                                                                            \
                                                                                            \
            }                                                                               \
            ((fd_set FAR *)(set))->fd_count--;                                              \
            break;                                                                          \
        }                                                                                   \
    }                                                                                       \
} while(0, 0)

#define FD_SET_EX(fd, set, session) do {                                                    \
    u_int __i;                                                                              \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) {                           \
        if (((fd_set FAR *)(set))->fd_array[__i] == (fd)) {                                 \
            break;                                                                          \
        }                                                                                   \
    }                                                                                       \
    if (__i == ((fd_set FAR *)(set))->fd_count) {                                           \
        if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) {                                 \
            ((fd_set FAR *)(set))->fd_array[__i] = (fd);                                    \
            ((fd_set_ex FAR *)(set))->fd_session[__i] = (session);                          \
            ((fd_set FAR *)(set))->fd_count++;                                              \
        }                                                                                   \
    }                                                                                       \
} while(0, 0)


/*
* 
* 	//u_int fd_count;               // how many are SET? 
	//SOCKET  fd_array[FD_SETSIZE];   // an array of SOCKETs 
struct fd_set_ex : fd_set
{
	Session* fd_session[FD_SETSIZE];
} ;

#define FD_CLR_EX(fd, set, session) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count ; __i++) { \
        if (((fd_set FAR *)(set))->fd_array[__i] == fd) { \
            while (__i < ((fd_set FAR *)(set))->fd_count-1) { \
                ((fd_set FAR *)(set))->fd_array[__i] = \
                    ((fd_set FAR *)(set))->fd_array[__i+1]; \
                __i++; \
            } \
            ((fd_set FAR *)(set))->fd_count--; \
            break; \
        } \
    } \
} while(0, 0)

#define FD_SET(fd, set) do { \
    u_int __i; \
    for (__i = 0; __i < ((fd_set FAR *)(set))->fd_count; __i++) { \
        if (((fd_set FAR *)(set))->fd_array[__i] == (fd)) { \
            break; \
        } \
    } \
    if (__i == ((fd_set FAR *)(set))->fd_count) { \
        if (((fd_set FAR *)(set))->fd_count < FD_SETSIZE) { \
            ((fd_set FAR *)(set))->fd_array[__i] = (fd); \
            ((fd_set FAR *)(set))->fd_count++; \
        } \
    } \
} while(0, 0)
*/

class SelectServerCore
{
private:
	fd_set					   m_fdSocketInfors;
	fd_set					   m_fdReads;
	Listener*				   m_listener;
	std::map<SOCKET, Session*> m_sessionFinder;
public:
	SelectServerCore();
	~SelectServerCore();

	bool Register(Listener* listener);
	bool Register(Session* session);
	bool Release(Session* session);
	bool Dispatch();
};

