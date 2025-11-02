#include "../io/iofn.c"
#include "../io/netfn.c"

char recv_buffer[2048];
char send_buffer[2048];

// Helper to send HTTP response
void send_http_response(SOCKET client, char* status, char* body) {
    int pos = 0;
    
    // "HTTP/1.1 "
    send_buffer[pos++] = 'H';
    send_buffer[pos++] = 'T';
    send_buffer[pos++] = 'T';
    send_buffer[pos++] = 'P';
    send_buffer[pos++] = '/';
    send_buffer[pos++] = '1';
    send_buffer[pos++] = '.';
    send_buffer[pos++] = '1';
    send_buffer[pos++] = ' ';
    
    // Status (e.g., "200 OK")
    int i = 0;
    while (status[i] != 0) {
        send_buffer[pos++] = status[i++];
    }
    send_buffer[pos++] = '\r';
    send_buffer[pos++] = '\n';
    
    // "Content-Type: text/html\r\n"
    char* ct = "Content-Type: text/html\r\n";
    i = 0;
    while (ct[i] != 0) {
        send_buffer[pos++] = ct[i++];
    }
    
    // "\r\n" (end of headers)
    send_buffer[pos++] = '\r';
    send_buffer[pos++] = '\n';
    
    // Body
    i = 0;
    while (body[i] != 0) {
        send_buffer[pos++] = body[i++];
    }
    
    send_data(client, send_buffer, pos);
}

int main() {
    if (net_init() != 0) {
        print("Failed to init\n");
        return 1;
    }
    
    SOCKET server = socket_create();
    if (server == INVALID_SOCKET) {
        print("Failed to create socket\n");
        net_cleanup();
        return 2;
    }
    
    if (bind_socket(server, 8080) == SOCKET_ERROR) {
        print("Bind failed\n");
        close_socket(server);
        net_cleanup();
        return 3;
    }
    
    if (listen_socket(server, 5) == SOCKET_ERROR) {
        print("Listen failed\n");
        close_socket(server);
        net_cleanup();
        return 4;
    }
    
    print("HTTP Server listening on port 8080...\n");
    print("Press Ctrl+C to stop\n");
    
    // Infinite loop - serve forever
    while (1) {
        struct sockaddr_in client_addr;
        int len = 16;
        
        print("Waiting for connection...\n");
        SOCKET client = accept_connection(server, &client_addr, &len);
        
        if (client == INVALID_SOCKET) {
            print("Accept failed\n");
            continue;  // Skip to next iteration
        }
        
        print("Client connected!\n");
        
        // Receive HTTP request
        int n = recv_data(client, recv_buffer, 2047);
        if (n > 0) {
            recv_buffer[n] = 0;
            print("Request:\n");
            print(recv_buffer);
            nline();
            
            // Send simple response
            char* html = "<html><body><h1>Hello from LCC HTTP Server!</h1></body></html>";
            send_http_response(client, "200 OK", html);
            print("Response sent\n");
        } else {
            print("Failed to receive data\n");
        }
        
        close_socket(client);
        print("Connection closed\n\n");
    }
    
    // This code never runs, but good practice
    close_socket(server);
    net_cleanup();
    
    return 0;
}