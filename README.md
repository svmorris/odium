# Odium
Reverse shell handling C2 platform.
Designed as a replacement for `nc -nlvp`, Odium can handle multiple reverse shells on the same port without the need to reset the listener. All this from within the terminal.

<img width="1552" height="1046" alt="image" src="https://github.com/user-attachments/assets/f7b1d26b-546d-444f-a067-7fb67b6fc0fa" />


Odium builds upon tmux to automatically create a new terminal window for each incoming connection.

# Features implemented
- Server can run continuously and always accept revshell connections
- Server can handle multiple revshell connections at once with a usable interface


# Features for the future
- Interactive shell (ability to use arrow keys and edit text)
- Non-blocking read/write (read and write don't have to be in a loop but rather happen asyncronously)
- Macros. E.G.: Automatic persistance, upgrading to 2-way interactive shell (up arrow, tab, and vim works)
- Screensaves: Each revshell interaction is saved as text automatically
- Optional notification API?
- Single binary: The whole application is just one binary (combining server and client)
- optional client that works with more features than standard revshelss
