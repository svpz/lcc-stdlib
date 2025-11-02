#include "../io/netfn.c"
#include "../io/iofn.c"

char buf[1024];

int main() {
    print("Initializing network...\n");
    if (net_init() != 0) {
        print("Failed to initialize WinSock\n");
        return 1;
    }
    
    SOCKET s = socket_create();
    if (s == INVALID_SOCKET) {
        print("Failed to create socket\n");
        net_cleanup();
        return 2;
    }
    
    // Connect to 127.0.0.1:8085
    print("Connecting to server...\n");
    if (connect_to(s, 0x0100007F, 8085) == SOCKET_ERROR) {
        print("Connection failed with error: ");
        print_int(get_last_error());
        nline();
        close_socket(s);
        net_cleanup();
        return 3;
    }
    
    print("Connected! Sending message...\n");
    
    char* message = "Hello from client";
    print("About to call send with socket: ");
print_int(s);
nline();
print("Message address: ");
print_int((unsigned long long)message);
nline();
print("Length: 17\n");
    int sent = send_data(s, message, 17);
    if (sent == SOCKET_ERROR) {
        print("Send failed with error: ");
        print_int(get_last_error());
        nline();
    } else {
        print("Message sent, bytes: ");
        print_int(sent);
        nline();
    }

    print("Waiting for response...\n");
    int n = recv_data(s, buf, 1023);
    if (n > 0) {
        buf[n] = 0;
        print("Received: ");
        print(buf);
        nline();
    } else if (n == 0) {
        print("Connection closed by server\n");
    } else {
        print("Recv failed with error: ");
        print_int(get_last_error());
        nline();
    }

    close_socket(s);
    net_cleanup();
    
    print("Client shutdown\n");
    return 0;
}