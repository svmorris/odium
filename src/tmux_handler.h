#ifndef TMUX_HANDLER_H
#define TMUX_HANDLER_H

#define TMUX_SESSION_NAME "c2server\0"

#define SOCK_PATH "/tmp/c2socket"

// I prefer commands getting defined as a static
// string as it leaves less room for accidentally
// introduced vulnerabilities in the future.
#define TMUX_RENAME_COMMAND "tmux rename-session c2server\0"
#define TMUX_PANE_COMMAND_H "tmux split-window -h './client "  SOCK_PATH  "'"
#define TMUX_PANE_COMMAND_V "tmux split-window -v './client "  SOCK_PATH  "'"


bool tmux_change_name();
void tmux_new_pane();
void tmux_relaunch(int argc, char *argv[]);
void tmux_get_name(char *buffer, int expected_name_size);


//c2server
#endif
