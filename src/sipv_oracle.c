#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define ORACLE_PORT 50505
void execute_cmd(const char *c) {
    if (strcmp(c, "SEAL") == 0) system("bash $HOME/oracle_engine.sh seal");
    else if (strcmp(c, "ROAST") == 0) { system("bash $HOME/oracle_engine.sh roast"); exit(0); }
}
int main() {
    unlink("/data/local/tmp/sipv_oracle");
    int s, n; struct sockaddr_in a; int o=1, l=sizeof(a);
    s = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &o, sizeof(o));
    a.sin_family = AF_INET; a.sin_addr.s_addr = inet_addr("127.0.0.1"); a.sin_port = htons(ORACLE_PORT);
    bind(s, (struct sockaddr *)&a, sizeof(a)); listen(s, 3);
    while(1) {
        n = accept(s, (struct sockaddr *)&a, (socklen_t*)&l);
        char b[1024] = {0}; read(n, b, 1023);
        b[strcspn(b, "\r\n")] = 0; execute_cmd(b); close(n);
    }
    return 0;
}
