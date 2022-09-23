DESCRIPTION
    We have created a shell program, writing in C, that runs a interactive prompt taking commands.

    - The shell shows the users current working directory at all times.

    - The shell has the ability to execute any program placed in the /bin/ folder on Linux systems, such as ls, wc, pwd with zero or more arguments.

    - The shell support the cd command (Change Diretory). cd supports absolute and relative pathing. Absolute paths have to start with '/', whereas relative does not.
      the cd command also supports using '~' (HOME path) to refere to the homepath enviroment variable. Using cd with no arguments, defaults the users working directory to HOME.

    - The shell supports piping between proceses, using the pipe operator '|'. The shell supports pipe-chaining, meaning you can create pipes between multiple         processes in a single command to the shell. 


MANUAL
    Starting the shell
    1. Navigate to the directory the "shell" file is located.
    2. Use the BASH command: "./shell", this starts the shell within your OS's shell.

    Safety First!
    - When using the shell, you can escape the process by pressing CTRL+C. You can close the shell by writing "exit" as a command.

    Changing directory
    - To change directory start the command with "cd" followed by a path.

    - You can parse "cd /" to change the working directory to root.
    - Alternatively can you use "cd ~" or "cd" to change directory to HOME.

    - When changing directory you can specify a relative or absolute path.
    - Changing to an absolute path, the path argument must start with a front-slash character '/'. An example: "cd /usr/bin/.
    - By not starting the path argument with a slash, the change of directory will be relative to the current working directory (cwd). So if your cwd is "/usr", and you use the command "cd bin", your cwd will be "/usr/bin.
    - To move back a directory use "cd .." to chain these do "cd ../../..".

    Using commands
    - You can use all commands that are in the "/bin" directory on Linux.
    - This means that the shell supports: "wc", "pwd", "ls", "cat", "curl" and many more. All theses commands accept zero or more arguments.
    - Use "ls /bin/" from any directory to get a complete list of all commands supported.

    Process Piping
    - You can pipe an output from a process as an input for another. You can do this using the pipe operator '|' between commands.
    - This syntax "ls -la | wc -l", starts a process "ls" that takes a argument "-la", this process outputs a lists of all elements in a directory and sends it as an input for the command "wc -l", which counts the number of lines of the input.
    - This can be chained further such as "ls -la | wc -l | cat".


CONCEPTS
    System Calls
        
    I/O Redirection
        
    Program Environment
        
    Background Program Execution
        
