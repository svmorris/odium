#ifndef TMUX_HANDLER_H
#define TMUX_HANDLER_H

#define TMUX_SESSION_NAME "odium_c2\0"

#define SOCK_PATH "/tmp/odiumsocket"



#define SPLASH "\n"\
               "\033[38;5;52m\n"\
               "\t\t    ███████    ██████████   █████ █████  █████ ██████   ██████   \n"\
               "\t\t  ███░░░░░███ ░░███░░░░███ ░░███ ░░███  ░░███ ░░██████ ██████    \n"\
               "\t\t ███     ░░███ ░███   ░░███ ░███  ░███   ░███  ░███░█████░███    \n"\
               "\t\t░███      ░███ ░███    ░███ ░███  ░███   ░███  ░███░░███ ░███    \n"\
               "\t\t░███      ░███ ░███    ░███ ░███  ░███   ░███  ░███ ░░░  ░███    \n"\
               "\t\t░░███     ███  ░███    ███  ░███  ░███   ░███  ░███      ░███    \n"\
               "\t\t ░░░███████░   ██████████   █████ ░░████████   █████     █████ ██\n"\
               "\t\t   ░░░░░░░    ░░░░░░░░░░   ░░░░░   ░░░░░░░░   ░░░░░     ░░░░░ ░░ \n"\
               "\033[0m\n"
// I prefer commands getting defined as a static
// string as it leaves less room for accidentally
// introduced vulnerabilities in the future.
#define TMUX_RENAME_COMMAND "tmux rename-session odium_c2\0"
#define TMUX_PANE_COMMAND_H "tmux split-window -h './odium-client-internal "  SOCK_PATH  "'"
#define TMUX_PANE_COMMAND_V "tmux split-window -v './odium-client-internal "  SOCK_PATH  "'"


bool tmux_change_name();
void tmux_new_pane();
void tmux_relaunch(int argc, char *argv[]);
void tmux_get_name(char *buffer, int expected_name_size);

#endif
