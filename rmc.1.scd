RMC(1)

# NAME
rmc - a remote command runner

# SYNOPSIS
rmc [*OPTIONS*...] [*COMMAND*]

# DESCRIPTION
rmc is a "remote command runner". You can start the rmc server in one terminal
with *rmc [SERVER OPTIONS...]*, and send commands to it with *rmc [CLIENT OPTIONS...]
COMMAND*. The server will run the command in the same environment and working
directory as the client that sent it. The server may be controlled further with
the options *--cancel* and *--kill*. Multiple servers can be running at once
using the *--name* option.

# SERVER OPTIONS
- -v, --verbose        Print working directory and command string before each run.
- -c, --clear          Clear the terminal in between each run.
- -h, --help           Show this help message.
- -r, --reset          Interrupt the running command if a new one is received.
- -q, --quiet          Don't print exit status. Cancels out *--verbose*.
- -d, --daemon         Start in daemon mode.
- -N, --notify         Send a desktop notification when command starts/finishes (requires rmc to be built with WITH_LIBNOTIFY=1).
- -n, --name=*NAME*      Start server with name *NAME*. The default name is 'default'.

# CLIENT OPTIONS
- -p, --pid            Get process ID of server.
- -C, --cancel         Cancel running command remotely (shorthand for *kill -SIGUSR1 $(rmc --pid)*).
- -k, --kill           Kill the server (shorthand for *kill -SIGTERM $(rmc --pid)*).
- -l, --list           List active servers.
- -n, --name=*NAME*      Select server with name *NAME*. The default name is 'default'.

# EXAMPLES
- rmc                  Start the default server.
- rmc echo hello       Send a command to the default server.
- rmc -cn myserver     Start a server with name 'myserver', and clear the terminal between each run.
- rmc -kn myserver     Kill server with name 'myserver'.
- rmc -C               Interrupt a command running on the default server remotely.
