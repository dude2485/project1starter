#include "consts.h"
#include "io.h"
#include "transport.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#define MSS 1012 // MSS = Maximum Segment Size (aka max length)

typedef struct
{
    uint16_t seq;
    uint16_t ack;
    uint16_t length;
    uint16_t window;
    uint16_t flags;
    uint16_t unused;
    uint8_t payload[0];
} packet;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: server <port>\n");
        exit(1);
    }

    /* Create sockets */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // use IPv4  use UDP

    /* Construct our address */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // use IPv4
    server_addr.sin_addr.s_addr =
        INADDR_ANY; // accept all connections
                    // same as inet_addr("0.0.0.0")
                    // "Address string to network bytes"
    // Set receiving port
    int PORT = atoi(argv[1]);
    server_addr.sin_port = htons(PORT); // Big endian

    /* Let operating system know about our config */
    int did_bind =
        bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));

    struct sockaddr_in client_addr; // Same information, but about client
    socklen_t s = sizeof(struct sockaddr_in);
    char buffer;

    // Wait for client connection
    // Assume the socket has been set up with all other variables
    char buf[sizeof(packet) + MSS] = {0};
    packet *pkt = (packet *)buf;
    int bytes_recvd = recvfrom(sockfd, pkt, sizeof(packet) + MSS, 0, (struct sockaddr *)&server_addr, &s);

    uint16_t seq = ntohs(pkt->seq); // Make sure to convert to little endian
    
    init_io();
    listen_loop(sockfd, &client_addr, SERVER, input_io, output_io);

    return 0;
}
