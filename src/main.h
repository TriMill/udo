#include <stddef.h>
#include <sys/types.h>

#define CHUNK_SIZE 10

typedef struct {
    uid_t* uids;
    int len;
} Config;

int main(int argc, char** argv);
int allowed(uid_t uid, Config* config);
int validate(char* user);
Config load_config();
