#ifndef CONTROLLER_H
#define CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "csdk.h"

typedef struct {
    char *user;
    char *pass;
    char *host;
    char *mnemonic;
    char *key;
} user_options_t;


static void json_logger(const char *message, int level, void *handle);

static char *get_home_dir();

static const char *get_filename_separator(const char *file_path);

static int get_user_auth_location(char *host, char **root_dir, char **user_file);

static void get_input(char *line);

static int get_password(char *password, int mask);

void close_signal(uv_handle_t *handle);

static void file_progress(double progress,
                          uint64_t downloaded_bytes,
                          uint64_t total_bytes,
                          void *handle);


static void fileUploaded(int status, file_meta_t *file, void *handle);

void upload_signal_handler(uv_signal_t *req, int signum);

static int upload_file(s_env_t *env, char *bucket_id, const char *file_path);

static void fileDownloaded(int status, FILE *fd, void *handle);

void download_signal_handler(uv_signal_t *req, int signum);

static int download_file(s_env_t *env, char *bucket_id,
                         char *file_id, char *path);

int do_action(int argument_number, char **arguments_array);

int start_upload_file(char* bucketId, char *path);
int start_download_file(char* bucketId, char *fileId, char *path);


void clean_uploading_data();
void clean_downloading_data();


#ifdef __cplusplus
}
#endif

#endif CONTROLLER_H
