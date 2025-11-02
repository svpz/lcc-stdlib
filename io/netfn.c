// netfn.c — Low-level networking stdlib for LUB / LCC (Windows)
// Works without any #include, relies on extern declarations

typedef unsigned long long size_t;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned long ULONG;
typedef unsigned char BYTE;
typedef unsigned long long SOCKET;
typedef int BOOL;
typedef unsigned short sa_family_t;

// Basic types for sockaddr
struct sockaddr
{
    sa_family_t sa_family;
    char sa_data[14];
};

struct sockaddr_in
{
    sa_family_t sin_family;
    unsigned short sin_port;
    unsigned long sin_addr_s_addr;  // Flattened - no nested struct
    char sin_zero[8];
};

struct WSADATA
{
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
};

// WinSock2 constants
#define AF_INET 2
#define SOCK_STREAM 1
#define IPPROTO_TCP 6
#define INVALID_SOCKET 0xFFFFFFFFFFFFFFFF
#define SOCKET_ERROR -1

// Correct extern declarations - WinSock uses generic sockaddr*
extern int WSAStartup(WORD wVersionRequested, struct WSADATA *wsaData);
extern int WSACleanup();
extern SOCKET socket(int af, int type, int protocol);
extern int connect(SOCKET s, struct sockaddr *name, int namelen);
extern int send(SOCKET s, char *buf, int len, int flags);
extern int recv(SOCKET s, char *buf, int len, int flags);
extern int closesocket(SOCKET s);
extern int bind(SOCKET s, struct sockaddr *addr, int namelen);
extern int listen(SOCKET s, int backlog);
extern SOCKET accept(SOCKET s, struct sockaddr *addr, int *addrlen);
extern int WSAGetLastError();

int get_last_error()
{
    return WSAGetLastError();
}

// Initialize WinSock
int net_init()
{
    struct WSADATA wsa;
    return WSAStartup(0x0202, &wsa); // version 2.2
}

// Cleanup WinSock
void net_cleanup()
{
    WSACleanup();
}

// Create TCP socket
SOCKET socket_create()
{
    return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

// Connect to server (IPv4)
int connect_to(SOCKET sock, unsigned long ip, unsigned short port)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = ((port >> 8) & 0xFF) | ((port & 0xFF) << 8);
    server.sin_addr_s_addr = ip;
    int i;
    for (i = 0; i < 8; i++)
    {
        server.sin_zero[i] = 0;
    }

    return connect(sock, (struct sockaddr *)&server, sizeof(server));
}

// Bind socket to address
int bind_socket(SOCKET sock, unsigned short port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ((port >> 8) & 0xFF) | ((port & 0xFF) << 8);
    addr.sin_addr_s_addr = 0; // INADDR_ANY - bind to all interfaces

    int i;
    for (i = 0; i < 8; i++)
    {
        addr.sin_zero[i] = 0;
    }

    return bind(sock, (struct sockaddr *)&addr, sizeof(addr));
}

// Listen on socket
int listen_socket(SOCKET sock, int backlog)
{
    return listen(sock, backlog);
}

// Accept incoming connection
SOCKET accept_connection(SOCKET sock, struct sockaddr_in *client_addr, int *addrlen)
{
    struct sockaddr *sa = (struct sockaddr *)(client_addr);
    return accept(sock, sa, addrlen);
}

// Send data
int send_data(SOCKET sock, char *buf, size_t len)
{
    return send(sock, buf, (int)len, 0);
}

// Receive data
int recv_data(SOCKET sock, char *buf, size_t len)
{
    return recv(sock, buf, (int)len, 0);
}

// Close socket
void close_socket(SOCKET sock)
{
    closesocket(sock);
}