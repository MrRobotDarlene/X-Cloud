#include "controller.h"
#include "eecallbackcontroller.h"


#define THREADPOOL_SIZE "64"


#ifndef errno
extern int errno;
#endif

static inline void noop() {};

#define CLI_VERSION "libinternxt-2.0.0-beta2"

static void json_logger(const char *message, int level, void *handle)
{
    printf("{\"message\": \"%s\", \"level\": %i, \"timestamp\": %" PRIu64 "}\n",
           message, level, util_timestamp());
}

static char *get_home_dir()
{
#ifdef _WIN32
    return getenv("USERPROFILE");
#else
    return getenv("HOME");
#endif
}


static const char *get_filename_separator(const char *file_path)
{
    const char *file_name = NULL;
#ifdef _WIN32
    file_name = strrchr(file_path, '\\');
    if (!file_name) {
        file_name = strrchr(file_path, '/');
    }
    if (!file_name && file_path) {
        file_name = file_path;
    }
    if (!file_name) {
        return NULL;
    }
    if (file_name[0] == '\\' || file_name[0] == '/') {
        file_name++;
    }
#else
    file_name = strrchr(file_path, '/');
    if (!file_name && file_path) {
        file_name = file_path;
    }
    if (!file_name) {
        return NULL;
    }
    if (file_name[0] == '/') {
        file_name++;
    }
#endif
    return file_name;
}

static int get_user_auth_location(char *host, char **root_dir, char **user_file)
{
    char *home_dir = get_home_dir();
    if (home_dir == NULL) {
        return 1;
    }

    int len = strlen(home_dir) + strlen("/.internxt/");
    *root_dir = (char*)calloc(len + 1, sizeof(char));
    if (!*root_dir) {
        return 1;
    }

    strcpy(*root_dir, home_dir);
    strcat(*root_dir, "/.internxt/");

    len = strlen(*root_dir) + strlen(host) + strlen(".json");
    *user_file = (char*)calloc(len + 1, sizeof(char));
    if (!*user_file) {
        return 1;
    }

    strcpy(*user_file, *root_dir);
    strcat(*user_file, host);
    strcat(*user_file, ".json");

    return 0;
}

static void get_input(char *line)
{
    if (fgets(line, BUFSIZ, stdin) == NULL) {
        line[0] = '\0';
    } else {
        int len = strlen(line);
        if (len > 0) {
            char *last = strrchr(line, '\n');
            if (last) {
                last[0] = '\0';
            }
            last = strrchr(line, '\r');
            if (last) {
                last[0] = '\0';
            }
        }
    }
}



static int get_password(char *password, int mask)
{
    int max_pass_len = 512;

#ifdef _WIN32
    HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    DWORD prev_mode = 0;
    GetConsoleMode(hstdin, &mode);
    GetConsoleMode(hstdin, &prev_mode);
    SetConsoleMode(hstdin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
#else
    static struct termios prev_terminal;
    static struct termios terminal;

    tcgetattr(STDIN_FILENO, &prev_terminal);

    memcpy (&terminal, &prev_terminal, sizeof(struct termios));
    terminal.c_lflag &= ~(ICANON | ECHO);
    terminal.c_cc[VTIME] = 0;
    terminal.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
#endif

    size_t idx = 0;         /* index, number of chars in read   */
    int c = 0;

    const char BACKSPACE = 8;
    const char RETURN = 13;

    /* read chars from fp, mask if valid char specified */
#ifdef _WIN32
    long unsigned int char_read = 0;
    while ((ReadConsole(hstdin, &c, 1, &char_read, NULL) && c != '\n' && c != RETURN && c != EOF && idx < max_pass_len - 1) ||
            (idx == max_pass_len - 1 && c == BACKSPACE))
#else
    while (((c = fgetc(stdin)) != '\n' && c != EOF && idx < max_pass_len - 1) ||
            (idx == max_pass_len - 1 && c == 127))
#endif
    {
        if (c != 127 && c != BACKSPACE) {
            if (31 < mask && mask < 127)    /* valid ascii char */
                fputc(mask, stdout);
            password[idx++] = c;
        } else if (idx > 0) {         /* handle backspace (del)   */
            if (31 < mask && mask < 127) {
                fputc(0x8, stdout);
                fputc(' ', stdout);
                fputc(0x8, stdout);
            }
            password[--idx] = 0;
        }
    }
    password[idx] = 0; /* null-terminate   */

    // go back to the previous settings
#ifdef _WIN32
    SetConsoleMode(hstdin, prev_mode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &prev_terminal);
#endif

    return idx; /* number of chars in passwd    */
}


void close_signal(uv_handle_t *handle)
{
    ((void)0);
}

static void file_progress(double progress,
                          uint64_t downloaded_bytes,
                          uint64_t total_bytes,
                          void *handle)
{
    int bar_width = 70;

    if (progress == 0 && downloaded_bytes == 0) {
        printf("Preparing File...");
        fflush(stdout);
        return;
    }

    printf("\r[");
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) {
            printf("=");
        } else if (i == pos) {
            printf(">");
        } else {
            printf(" ");
        }
    }
    printf("] %.*f%%", 2, progress * 100);

    fflush(stdout);
}


void upload_signal_handler(uv_signal_t *req, int signum)
{
    upload_state_t *state = (upload_state_t *)req->data;
    bridge_store_file_cancel(state);
    if (uv_signal_stop(req)) {
        printf("Unable to stop signal\n");
    }
    uv_close((uv_handle_t *)req, close_signal);
}

static int upload_file(s_env_t *env, char *bucket_id, const char *file_path)
{
    FILE *fd = fopen(file_path, "r");

    if (!fd) {
        printf("Invalid file path: %s\n", file_path);
    }

    const char *file_name = get_filename_separator(file_path);

    if (!file_name) {
        file_name = file_path;
    }

    // Upload opts env variables:
    char *prepare_frame_limit = getenv("PREPARE_FRAME_LIMIT");
    char *push_frame_limit = getenv("PUSH_FRAME_LIMIT");
    char *push_shard_limit = getenv("PUSH_SHARD_LIMIT");
    char *rs = getenv("REED_SOLOMON");

    upload_opts_t upload_opts = {};
        upload_opts.prepare_frame_limit = (prepare_frame_limit) ? atoi(prepare_frame_limit) : 1;
        upload_opts.push_frame_limit = (push_frame_limit) ? atoi(push_frame_limit) : 64;
        upload_opts.push_shard_limit = (push_shard_limit) ? atoi(push_shard_limit) : 64;
        upload_opts.rs = (!rs) ? true : (strcmp(rs, "false") == 0) ? false : true;
        upload_opts.bucket_id = bucket_id;
        upload_opts.file_name = file_name;
        upload_opts.fd = fd;


    uv_signal_t *sig = (uv_signal_t *)malloc(sizeof(uv_signal_t));
    if (!sig) {
        return 1;
    }
    uv_signal_init(env->loop, sig);
    uv_signal_start(sig, upload_signal_handler, SIGINT);



    progress_cb progress = (progress_cb)noop;
    if (env->log_options->level == 0) {
        progress = file_progress;
    }


    upload_state_t *state = bridge_store_file(env,
                                                          &upload_opts,
                                                          NULL,
                                                          progress,
                                                          fileUploaded);

    if (!state) {
        return 1;
    }

    sig->data = state;

    return state->error_status;
}


void download_signal_handler(uv_signal_t *req, int signum)
{
    download_state_t *state = (download_state_t *)req->data;
    bridge_resolve_file_cancel(state);
    if (uv_signal_stop(req)) {
        printf("Unable to stop signal\n");
    }
    uv_close((uv_handle_t *)req, close_signal);
}

static int download_file(s_env_t *env, char *bucket_id,
                         char *file_id, char *path)
{
    FILE *fd = NULL;

    if (path) {
        char user_input[BUFSIZ];
        memset(user_input, '\0', BUFSIZ);

        if(access(path, F_OK) != -1 ) {
            printf("Warning: File already exists at path [%s].\n", path);
            while (strcmp(user_input, "y") != 0 && strcmp(user_input, "n") != 0)
            {
                memset(user_input, '\0', BUFSIZ);
                printf("Would you like to overwrite [%s]: [y/n] ", path);
                get_input(user_input);
            }

            if (strcmp(user_input, "n") == 0) {
                printf("\nCanceled overwriting of [%s].\n", path);
                return 1;
            }

            unlink(path);
        }

        fd = fopen(path, "w+");
    } else {
        fd = stdout;
    }

    if (fd == NULL) {
        // TODO send to stderr
        printf("Unable to open %s: %s\n", path, strerror(errno));
        return 1;
    }

    uv_signal_t *sig = (uv_signal_t*)malloc(sizeof(uv_signal_t));
    uv_signal_init(env->loop, sig);
    uv_signal_start(sig, download_signal_handler, SIGINT);

    progress_cb progress = (progress_cb)noop;
    if (path && env->log_options->level == 0) {
        progress = file_progress;
    }

    download_state_t *state = bridge_resolve_file(env, bucket_id,
                                                              file_id, fd, NULL,
                                                              progress,
                                                              fileDownloaded);

    if (!state) {
        return 1;
    }
    sig->data = state;

    return state->error_status;
}

// initialize event loop and environment
s_env_t *uploading_env = NULL;
s_env_t *downloading_env = NULL;

void fileUploaded(int status, file_meta_t *file, void *handle)
{
    printf("- fileUploaded()\n");
    free_uploaded_file_info(file);

    if (status != 0) {
        printf("Upload failure: %s\n", s_strerror(status));
        callBackFunctionFromC(ErrorCallBack);
    } else {
        printf("Upload Success! File ID: %s\n", file->id);


        //should be called after succesful uploading
        callBackFunctionFromC(FileUploaded);


        if (uploading_env) {
            int val = uv_loop_alive(uploading_env->loop);

            printf("Stop begin: ");
            printf("%d\n", val);
            uv_stop(uploading_env->loop);
            val = uv_loop_alive(uploading_env->loop);
            printf("Stop end : ");
            printf("%d\n", val);
        }


//        int val = uv_loop_alive(uploading_env->loop);

//        if (uploading_env) {
//            uv_stop(uploading_env->loop);
//            printf("Stopped uploading loop!!!\n");
//        }
//        val = uv_loop_alive(uploading_env->loop);


//        if (!uv_loop_alive(env->loop)) {
//            printf("Stop alive loop!\n");
//            uv_stop(env->loop);
//        } else {
//            printf("Loop has already stopped\n");
//        }
        //clean_values();
    }
}

void fileDownloaded(int status, FILE *fd, void *handle)
{
    printf("- fileDownloaded()");
    printf("\n");
    fclose(fd);
    if (status) {
        // TODO send to stderr
        switch(status) {
            case FILE_DECRYPTION_ERROR:
                printf("Unable to properly decrypt file, please check " \
                       "that the correct encryption key was " \
                       "imported correctly.\n\n");
                break;
            default:
                printf("Download failure: %s\n", s_strerror(status));
        }
        callBackFunctionFromC(ErrorCallBack);
    }
    printf("Download Success!\n");

    //should be called after succesful downloading
    callBackFunctionFromC(FileDownloaded);

    uv_stop(downloading_env->loop);
    printf("Stopped donwloading loop!!!\n");

}

int start_upload_file(char *bucketId, char *path)
{
    printf("- start_upload_file(char *bucketId, char *path)\n");
    //clean_downloading_data();
    char *user = (char*)"";
    char *pass = (char*)"";
    char *mnemonic = (char*)"";

    char *bridge = (char*)"http://api.internxt.io:6382";

    int log_level = 0;

    char *proxy = getenv("INTERNXT_PROXY");


    // Parse the host, part and proto from the bridge url
    char proto[6];
    char host[100];
    int port = 0;
    sscanf(bridge, "%5[^://]://%99[^:/]:%99d", proto, host, &port);

    if (port == 0) {
        if (strcmp(proto, "https") == 0) {
            port = 443;
        } else {
            port = 80;
        }
    }

    http_options_t http_options;
    http_options.cainfo_path = NULL;
    http_options.user_agent = CLI_VERSION;
    http_options.low_speed_limit = LOW_SPEED_LIMIT;
    http_options.low_speed_time = LOW_SPEED_TIME;
    http_options.timeout = HTTP_TIMEOUT;


    log_options_t log_options = {
        .logger = json_logger,
        .level = log_level
    };

    if (proxy) {
        http_options.proxy_url = proxy;
    } else {
        http_options.proxy_url = NULL;
    }

    bridge_options_t options = {
        .proto = proto,
        .host  = host,
        .port  = port,
        .user  = user,
        .pass  = pass
    };

    encrypt_options_t encrypt_options = {
        .mnemonic = mnemonic
    };

    uploading_env = init_env(&options, &encrypt_options,
                         &http_options, &log_options);
    if (!uploading_env) {
        printf("Environment wasn't initialized!\n");
        return 1;
    }

    if (!bucketId) {
        printf("Wrong bucket id!\n");
        return 1;
    }

    if (!path) {
        printf("Wrong path!\n");
        return 1;
    }

    if (upload_file(uploading_env, bucketId, path)) {
        printf("Upload file error!!!\n");
        return 1;
    } else {
        printf("Correct uploading status!\n");
    }


    // run all queued events
    if (uv_run(uploading_env->loop, UV_RUN_DEFAULT)) {
        printf("Uploading loop finished with error message!\n");
//        uv_loop_close(env->loop);
//        We aren't using root dir so free it
//        free(root_dir);
//        char *keypass = getenv("KEYPASS");

//        cleanup
//        destroy_env(env);
    } else {
        printf("Succesful uploading loop finish!\n");
    }

    return 0;
}

int start_download_file(char *bucketId, char *fileId, char *path)
{
    printf("- start_download_file(char *bucketId, char *fileId, char *path)\n");
    //clean_uploading_data();

    char *user = (char*)"";
    char *pass = (char*)"";
    char *mnemonic = (char*)"";

    char *bridge = (char*)"http://api.internxt.io:6382";

    int log_level = 0;

    char *proxy = getenv("PROXY");

    // Parse the host, part and proto from the bridge url
    char proto[6];
    char host[100];
    int port = 0;
    sscanf(bridge, "%5[^://]://%99[^:/]:%99d", proto, host, &port);

    if (port == 0) {
        if (strcmp(proto, "https") == 0) {
            port = 443;
        } else {
            port = 80;
        }
    }

    http_options_t http_options;
    http_options.cainfo_path = NULL;
    http_options.user_agent = CLI_VERSION;
    http_options.low_speed_limit = LOW_SPEED_LIMIT;
    http_options.low_speed_time = LOW_SPEED_TIME;
    http_options.timeout = HTTP_TIMEOUT;


    log_options_t log_options = {
        .logger = json_logger,
        .level = log_level
    };

    if (proxy) {
        http_options.proxy_url = proxy;
    } else {
        http_options.proxy_url = NULL;
    }

    bridge_options_t options = {
        .proto = proto,
        .host  = host,
        .port  = port,
        .user  = user,
        .pass  = pass
    };

    encrypt_options_t encrypt_options = {
        .mnemonic = mnemonic
    };


    downloading_env = init_env(&options, &encrypt_options,
                              &http_options, &log_options);


    if (!downloading_env) {
        printf("Environment wasn't initialized!\n");
        return 1;
    }

    if (!bucketId) {
        printf("Wrong bucket id!\n");
        return 1;
    }

    if (!fileId) {
        printf("Wrong file id!\n");
        return 1;
    }

    if (!path) {
        printf("Wrong path!\n");
        return 1;
    }

    if (download_file(downloading_env, bucketId, fileId, path)) {
        printf("Downloading file error!!!\n");
        return 1;
    } else {
        printf("Correct downloading status!\n");
    }

    // run all queued events
    if (uv_run(downloading_env->loop, UV_RUN_DEFAULT)) {
        printf("Downloading loop finished with error message!\n");
//        uv_loop_close(env->loop);

        // cleanup
//        destroy_env(env);
    } else {
        printf("Succesful downloading loop finish!\n");
    }

    return 0;
}

void clean_uploading_data()
{
    printf("- clean_uploading_info\n");
    if (uploading_env) {
        //if (!uv_loop_alive(uploading_env->loop)) {
        int val = uv_loop_alive(uploading_env->loop);
        printf("Stop begin: ");
        printf("%d\n", val);
        uv_stop(uploading_env->loop);
        uv_loop_close(uploading_env->loop);
        val = uv_loop_alive(uploading_env->loop);
        printf("Stop end : ");
        printf("%d\n", val);
        //}
        //uv_loop_close(env->loop);
        //destroy_env(uploading_env);
    }
    uploading_env = NULL;

}

void clean_downloading_data()
{
    printf("-  clean_downloading_info()\n");
    if (downloading_env) {
        if (!uv_loop_alive(downloading_env->loop)) {
            uv_stop(downloading_env->loop);
        }
        //uv_loop_close(env->loop);
        destroy_env(downloading_env);
    }
    downloading_env = NULL;
}
