# revshell_c2
Multi-revshell c2 server without the need for a gui
<img width="1607" height="1039" alt="image" src="https://github.com/user-attachments/assets/82f777d4-2bad-481e-ba72-0b0bda4c21cd" />

The application will open a new tmux pane for each recieved connection as shown in the image above.

# Features implemented
- Server can run continuously and always accept revshell connections
- Server can handle multiple revshell connections at once with a usable interface


# Features for the future
- Interactive shell (ability to use arrow keys and edit text)
- Non-blocking read/write (read and write don't have to be in a loop but rather happen asyncronously)
- Macros. E.G.: Automatic persistance, upgrading to 2-way interactive shell (up arrow, tab, and vim works)
- Screensaves: Each revshell interaction is saved as text automatically
- Optional notification API?
