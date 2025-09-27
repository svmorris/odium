#ifndef TMUX_HANDLER_H
#define TMUX_HANDLER_H

#define TMUX_SESSION_NAME "c2server\0"
#define TMUX_RENAME_COMMAND "tmux rename-session c2server\0"


bool tmux_change_name();
void tmux_relaunch(int argc, char *argv[]);
void tmux_get_name(char *buffer, int expected_name_size);


//c2server
#endif
