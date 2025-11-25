# Odium
Reverse shell handling C2 platform.
Designed as a replacement for `nc -nlvp`, Odium can handle multiple reverse shells on the same port without the need to reset the listener. All this from within the terminal.

<img width="1820" height="1039" alt="image" src="https://github.com/user-attachments/assets/504fc9ad-f38b-4d6d-87b0-4b56a03aca65" />



Odium builds upon tmux to automatically create a new terminal window for each incoming connection.

# Usage
Odium takes only one argument for a port number. Simply run:
```
odium 8080
```

NOTE: This will only work if the following two conditions are met:
1. There is no other instance of odium running under your user.
2. Odium is run from outside of a tmux session.


# Installation
1. Compile odium by running `make`.
2. Copy the odium binary to an executable path.

Example:
```
make
sudo cp bin/odium /usr/local/bin/
```

# Features for the future
- Interactive shell (ability to use arrow keys and edit text).
- Screensaves: Each revshell interaction is saved as text automatically.
