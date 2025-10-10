# Odium
Reverse shell handling C2 platform.
Designed as a replacement for `nc -nlvp`, Odium can handle multiple reverse shells on the same port without the need to reset the listener. All this from within the terminal.

<img width="1820" height="1039" alt="image" src="https://github.com/user-attachments/assets/504fc9ad-f38b-4d6d-87b0-4b56a03aca65" />



Odium builds upon tmux to automatically create a new terminal window for each incoming connection.

# Features implemented
- Server can run continuously and always accept revshell connections
- Server can handle multiple revshell connections at once with a usable interface


# Features for the future
- Interactive shell (ability to use arrow keys and edit text)
- Screensaves: Each revshell interaction is saved as text automatically
- Optional notification API?
- Single binary: The whole application is just one binary (combining server and client)
