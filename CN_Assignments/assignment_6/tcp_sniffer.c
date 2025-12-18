#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock_raw;
    struct sockaddr saddr;
    int saddr_len = sizeof(saddr);

    unsigned char *buffer = (unsigned char *) malloc(65536);

    printf("Starting TCP Packet Sniffer...\n");

    // Create RAW socket
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("Socket Error");
        return 1;
    }

    while (1) {
        // Receive packet
        int data_size = recvfrom(sock_raw, buffer, 65536, 0,
                                 &saddr, (socklen_t *)&saddr_len);

        if (data_size < 0) {
            perror("Recvfrom error");
            return 1;
        }

        struct ethhdr *eth = (struct ethhdr *) buffer;

        // Check if packet is IP
        if (ntohs(eth->h_proto) == ETH_P_IP) {

            struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));

            // Check if protocol is TCP
            if (iph->protocol == 6) {

                unsigned short iphdrlen = iph->ihl * 4;
                struct tcphdr *tcph = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);

                printf("\n==== TCP Packet Captured =====\n");

                // Ethernet Info
                printf("Ethernet Header\n");
                printf("   |-Source MAC      : %02x:%02x:%02x:%02x:%02x:%02x\n",
                       eth->h_source[0], eth->h_source[1], eth->h_source[2],
                       eth->h_source[3], eth->h_source[4], eth->h_source[5]);

                printf("   |-Destination MAC : %02x:%02x:%02x:%02x:%02x:%02x\n",
                       eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
                       eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

                // IP Info
                struct sockaddr_in src, dst;
                src.sin_addr.s_addr = iph->saddr;
                dst.sin_addr.s_addr = iph->daddr;

                printf("\nIP Header\n");
                printf("   |-Source IP        : %s\n", inet_ntoa(src.sin_addr));
                printf("   |-Destination IP   : %s\n", inet_ntoa(dst.sin_addr));

                // TCP Info
                printf("\nTCP Header\n");
                printf("   |-Source Port      : %u\n", ntohs(tcph->source));
                printf("   |-Destination Port : %u\n", ntohs(tcph->dest));
                printf("   |-Sequence Number  : %u\n", ntohl(tcph->seq));
                printf("   |-Acknowledgment No: %u\n", ntohl(tcph->ack_seq));

                printf("====================================\n");
            }
        }
    }

    close(sock_raw);
    return 0;
}

