#include <jni.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>

#define TAG "SIPV_Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

#define VAULT_SIZE 1024 * 512
uint8_t *oracle_vault;

void roast_protocol(int sig) {
    LOGI("!!! SIGUSR1 RECEIVED: EXECUTING ROAST PROTOCOL !!!");
    if (oracle_vault) {
        memset(oracle_vault, 0, VAULT_SIZE);
        __builtin___clear_cache((char*)oracle_vault, (char*)oracle_vault + VAULT_SIZE);
    }
    LOGI("Vault purged. Ghosting...");
    _exit(0);
}

JNIEXPORT void JNICALL
Java_com_sipv_OracleWrapper_nativeStartEngine(JNIEnv *env, jobject thiz) {
    struct sigaction sa;
    sa.sa_handler = roast_protocol;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    oracle_vault = (uint8_t *)malloc(VAULT_SIZE);
    LOGI("Oracle Engine started. PID: %d", getpid());
    
    while(1) { sleep(10); }
}

JNIEXPORT void JNICALL
Java_com_sipv_OracleWrapper_nativeStopEngine(JNIEnv *env, jobject thiz) {
    if (oracle_vault) free(oracle_vault);
}
