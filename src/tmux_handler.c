#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "tmux_handler.h"


int windows_opened = 0;

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
    tmux_args[i++] = "-f";
    tmux_args[i++] = "./configs/tmux.conf";
    tmux_args[i++] = "new-session";
    tmux_args[i++] = "-s";
    tmux_args[i++] = (char *)TMUX_SESSION_NAME;

    for (int j = 0; j < argc; j++)
    {
        tmux_args[i++] = argv[j];
    }

    tmux_args[i] = NULL;

    // Replace oneself with a better version
    execvp("tmux", tmux_args);

    /* for (int x = 0; x < (argc+5); x++) */
    /*     printf("%s ", tmux_args[x]); */

    perror("Failed to launch with tmux");
    puts("Are you sure tmux is installed?\n");
    exit(-7);
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
void tmux_new_pane()
{
    int ret;

    if (windows_opened % 3 == 0)
        ret = system(TMUX_PANE_COMMAND_H);
    else
        ret = system(TMUX_PANE_COMMAND_V);

    if (ret < 0)
        perror("Something went wrong when opening tmux panes");

    windows_opened++;
}


