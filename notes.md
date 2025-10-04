Some notes for myself or anyone else contributing to the repository
===================================================================
These are going to be mostly random.


## Debugging the window client
Tmux will close the window by default when the client exits. This is good for usability, but bad for understanding why the client crashed. If you're debugging the client, the best thing to do is to run the following in the tmux shell after the server opens.

```
set-option -g remain-on-exit on
```

If you don't know how to open the shell:
```
Ctrl+b :
```

NOTE: Sometimes you have to scroll up on the exited client to find the crash output.


This same effect could be achieved in the c code and some arguments, but I don't think its worth it.
