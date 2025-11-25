#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "tmux_config.h"
#include "tmux_handler.h"


int windows_opened = 0;

int safer_exec(const char *path, char *const argv[])
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Failed to create child process while trying to create an external command");
        return -1;
    }

    if (pid == 0)
    {
        execvp(path, argv);
        perror("Failed to execute external command");
        return -1;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1)
    {
        perror("An error occurred while waiting for subprocess to join");
        return -1;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else
        return -2;
}


/*
 * This function gets called if the application
 * detects that it is running outside of tmux.
 *
 * This function replaces itself with the same
 * process running under tmux.
 */
void tmux_relaunch(int argc, char *argv[])
{

    char **tmux_args = malloc((argc + 9) * sizeof(char *));
    if (!tmux_args)
    {
        perror("Malloc failed while attempting to launch tmux");
        exit(-6);
    }

    int i = 0;
    tmux_args[i++] = "tmux";
    tmux_args[i++] = "-L";
    tmux_args[i++] = "odium";
    tmux_args[i++] = "new-session";
    tmux_args[i++] = "-s";
    tmux_args[i++] = (char *)TMUX_SESSION_NAME;

    for (int j = 0; j < argc; j++)
    {
        tmux_args[i++] = argv[j];
    }

    tmux_args[i] = NULL;

    /* pid_t pid = fork(); */
    /* if (pid == 0) */
        // Replace oneself with a better version
        execvp("tmux", tmux_args);
    /* else */
        /* tmux_decorate(); */

    perror("Failed to launch with tmux");
    puts("Are you sure tmux is installed?\n");
    exit(-7);
}

void tmux_decorate()
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if (pid == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execlp("tmux", "tmux", "-L", "odium", "source-file", "-", NULL);
    }
    else
    {
        close(fd[0]);
        write(fd[1], odium_tmux_conf, strlen(odium_tmux_conf));
        close(fd[1]);
    }
}


/*
 * Return the name of the current tmux session
 */
void tmux_get_name(char *buffer, int expected_name_size)
{
    if (getenv("TMUX") == NULL)
    {
        puts("No active tmux session");
        return;
    }

    FILE *fp;

    fp = popen("tmux display-message -p '#S'", "r");
    if (fp == NULL)
    {
        perror("Failed to get tmux session name");
        return;
    }

    if (fgets(buffer, expected_name_size, fp) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = 0;
    }

    pclose(fp);
    return;
}


bool tmux_change_name()
{
    if (getenv("TMUX") == NULL)
    {
        puts("No active tmux session");
        return false;
    }

    FILE *fp;

    fp = popen((char *)TMUX_RENAME_COMMAND, "r");
    if (fp == NULL)
    {
        perror("Failed to change tmux session name");
        return false;
    }

    pclose(fp);
    return true;
}

/*
 * Every other pane should be opened
 * vertically so tmux doesn't look weird
 */
void tmux_new_pane(char *argv0)
{
    int ret;
    char command_buffer[512] = "";

    // This sort of limits the maximum filename, but I don't think its a problem
    if (windows_opened % 3 == 0)
        snprintf(command_buffer, 512, TMUX_PANE_COMMAND_H, argv0);
    else
        snprintf(command_buffer, 512, TMUX_PANE_COMMAND_H, argv0);

    ret = system(command_buffer);

    if (ret < 0)
        perror("Something went wrong when opening tmux panes");

    windows_opened++;
}

void tmux_set_pane_name(char *name)
{
    // tmux select-pane -T <name> \0
    char **tmux_args = malloc(5 *sizeof(char *));
    if (!tmux_args)
    {
        perror("Malloc failed during session rename");
        return;
    }

    int i = 0;
    tmux_args[i++] = "tmux";
    tmux_args[i++] = "select-pane";
    tmux_args[i++] = "-T";
    tmux_args[i++] = name;
    tmux_args[i] = NULL;

    if ((safer_exec("tmux", tmux_args)) == 0)
        return;
    fprintf(stderr, "Error occured while trying to rename tmux pane...");
}
