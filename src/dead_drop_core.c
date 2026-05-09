#include <jni.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdatomic.h>
#include <android/log.h>

#define TAG "DeadDrop_Native"
#define VAULT_SIZE (1024 * 512)
#define SOCKET_PATH "/data/data/com.sipv/dead_drop.sock"

typedef struct {
    _Atomic uint32_t heartbeat;
    uint8_t vault[VAULT_SIZE];
} SharedVault;

static SharedVault *g_vault = NULL;

static void secure_wipe(void *v, size_t n) {
    volatile uint8_t *p = v;
    size_t total = n; 
    while (n--) *p++ = 0;
    __builtin___clear_cache((char*)v, (char*)v + total);
    __asm__ __volatile__("dsb sy" : : : "memory");
}

void roast_protocol(int sig) {
    if (g_vault) {
        secure_wipe(g_vault->vault, VAULT_SIZE);
        munlock(g_vault, sizeof(SharedVault));
    }
    unlink(SOCKET_PATH);
    _exit(0);
}

void spawn_watchdog(pid_t parent_pid) {
    if (fork() == 0) {
        setsid(); 
        prctl(PR_SET_PDEATHSIG, SIGUSR1); 
        uint32_t last_beat = 0;
        while(1) {
            sleep(2);
            uint32_t current_beat = atomic_load(&g_vault->heartbeat);
            if (current_beat == last_beat || kill(parent_pid, 0) != 0) {
                roast_protocol(SIGUSR1);
            }
            last_beat = current_beat;
        }
    }
}

JNIEXPORT void JNICALL
Java_com_sipv_OracleWrapper_nativeStartEngine(JNIEnv *env, jobject thiz) {
    g_vault = mmap(NULL, sizeof(SharedVault), PROT_READ | PROT_WRITE, 
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mlock(g_vault, sizeof(SharedVault));
    madvise(g_vault, sizeof(SharedVault), MADV_DONTDUMP);

    int server_fd, client_fd;
    struct sockaddr_un addr;

    unlink(SOCKET_PATH);
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    spawn_watchdog(getpid());
    
    uint32_t beat = 0;
    while(1) {
        atomic_store(&g_vault->heartbeat, ++beat);
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd > 0) {
            read(client_fd, g_vault->vault, VAULT_SIZE);
            close(client_fd);
        }
    }
}
