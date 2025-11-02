#include "../io/iofn.c"
#include "../io/netfn.c"

char buf[1024];

// Helper function to zero out memory
void zero_mem(char* ptr, int size) {
    int i;
    for (i = 0; i < size; i++) {
        ptr[i] = 0;
    }
}

int main()
{
    // First, let's check struct sizes
    struct sockaddr_in test_addr;
    int addr_size = sizeof(test_addr);
    print("sizeof(sockaddr_in) = ");
    print_int(addr_size);
    nline();
    
    print("Initializing network...\n");
    int mmm = net_init();
    print("WSAStartup returned: ");
    print_int(mmm);
    nline();
    if (mmm != 0) {
        print("Failed to initialize WinSock\n");
        return 1;
    }
    
    SOCKET server = socket_create();
    print("socket() returned: ");
    print_int(server);
    nline();
    
    if (server == INVALID_SOCKET) {
        print("Failed to create socket\n");
        net_cleanup();
        return 2;
    }

    print("Binding to port 8085...\n");
    int bind_result = bind_socket(server, 8085);
    print("bind() returned: ");
    print_int(bind_result);
    nline();
    
    if (bind_result == SOCKET_ERROR) {
        print("Bind failed\n");
        print_int(get_last_error());
        close_socket(server);
        net_cleanup();
        return 3;
    }

    print("Listening...\n");
    int listen_result = listen_socket(server, 5);
    print("listen() returned: ");
    print_int(listen_result);
    nline();
    
    if (listen_result == SOCKET_ERROR) {
        print("Listen failed\n");
        close_socket(server);
        net_cleanup();
        return 4;
    }

    print("Server listening on port 8085...\n");
    print("Waiting for client connection...\n");
    print("Calling accept() now - should block...\n");

    struct sockaddr_in client_addr;
    // Initialize client_addr to zero
    zero_mem((char*)&client_addr, sizeof(client_addr));
    
    int len = 16;
    
    SOCKET client = accept_connection(server, &client_addr, &len);
    
    print("accept() returned: ");
    print_int(client);
    nline();
    
    if (client == INVALID_SOCKET) {
        print("Accept failed\n");
        close_socket(server);
        net_cleanup();
        return 5;
    }

    print("Client connected!\n");

    int n = recv_data(client, buf, 1023);
    if (n > 0) {
        buf[n] = 0;
        print("Received: ");
        print(buf);
        nline();
        
        char* response = "Hello from server";
        send_data(client, response, 17);
        print("Response sent\n");
    } else {
        print("Recv failed or no data\n");
    }

    close_socket(client);
    close_socket(server);
    net_cleanup();
    
    print("Server shutdown\n");
    return 0;
}