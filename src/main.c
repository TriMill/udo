#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include "main.h"
#include <pwd.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

int main(int argc, char** argv) {
    if(argc <= 1) {
        printf("insufficient\n");
        return 0;
    }
    uid_t uid = getuid();
    Config config = load_config();

    struct passwd* pwd = getpwuid(uid);
    char* user = pwd->pw_name;

    int result = validate(user);
    if(result) {
        return 2;
    }

    char* cmd = argv[1];
    char* args[argc];

    for(int i = 0; i < argc-1; i++) {
        args[i] = malloc(strlen(argv[i+1]));
        strcpy(args[i], argv[i+1]);        
    }
    args[argc-1] = NULL;

    if(allowed(uid, &config)) {
        setuid(geteuid());
        setgid(getegid());
        int x = execvp(cmd, args);
        return x;
    } else {
        fprintf(stderr, "nope\n");
        return 1;
    }
}

int validate(char* user) {
    pam_handle_t *pamh = NULL;
    struct pam_conv conv = {misc_conv, NULL};

    int status = pam_start("udo", user, &conv, &pamh);
    if(status != PAM_SUCCESS) {
        printf("bad: %d\n", status);
        return 1;
    }

    status = pam_authenticate(pamh, 0);
    if(status != PAM_SUCCESS) {
        printf("wrong: %d\n", status);
        return 1;
    }
    status = pam_acct_mgmt(pamh, 0);

    if (pam_end(pamh,status) != PAM_SUCCESS) { 
        pamh = NULL;
        exit(2);
    }

    if(status != PAM_SUCCESS) {
        printf("don't: %d\n", status);
        return 1;
    }
    return 0;
}

int allowed(uid_t uid, Config* config) {
    for(int i = 0; i < config->len; i++) {
        if(config->uids[i] == uid) {
            return 1;
        }
    }
    return 0;
}

Config load_config() {
    FILE* fp = fopen("/etc/udo.conf", "r");
    char* contents = malloc(CHUNK_SIZE + 1);
    int offset = 0;
    char buf[CHUNK_SIZE];
    int read = CHUNK_SIZE;
    while(read == CHUNK_SIZE) {
        read = fread(buf, 1, CHUNK_SIZE, fp);
        memcpy(contents + offset, buf, read);
        offset += read;
        if(read == CHUNK_SIZE) {
            contents = realloc(contents, offset + CHUNK_SIZE + 1);
        }
    }
    fclose(fp);
    contents[offset] = '\0';

    Config config;
    config.uids = malloc(1 * sizeof(uid_t));
    config.len = 0;

    char* ctx;
    char* word = strtok_r(contents, " ", &ctx);
    while(word != NULL) {
        uid_t uid = atoi(word);
        config.uids = realloc(config.uids, (config.len+1) * sizeof(uid_t));
        config.uids[config.len++] = uid;
        word = strtok_r(NULL, "\n", &ctx);
    }

    return config;
}
