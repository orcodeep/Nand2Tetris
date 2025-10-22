#include "../parser/parser.h"
#include "../codewriter/codewriter.h"
#include <sys/stat.h>
#include <dirent.h>

static size_t filepathBuffSize = 50;
static char* filepathBuff = NULL;

bool checkargv1(char* arg);
DIR* opendirectory(const char* dirpath);
char* getvmfn(DIR* dir, char* dirpath);

