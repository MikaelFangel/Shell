DESCRIPTION
    We have created a shell program, writing in C, that runs a interactive prompt taking commands.

    - The shell shows the users current working directory at all times.

    - The shell has the ability to execute any program placed in the /bin/ folder on Linux systems, such as ls, wc, pwd with zero or more arguments.

    - The shell support the cd command (Change Diretory). cd supports absolute and relative pathing. Absolute paths have to start with '/', whereas relative does not.
      the cd command also supports using '~' (HOME path) to refere to the homepath enviroment variable. Using cd with no arguments, defaults the users working directory to HOME.

    - The shell supports piping between proceses, using the pipe operator '|'. The shell supports pipe-chaining, meaning you can create pipes between multiple         processes in a single command to the shell. 


MANUAL for setup
    1. Extract the zip file.
    2. Navigate to the extracted directory with a terminal.
    3. Type 'make'.
    4. To start the program, type '.\shell' in the terminal.

MANUAL for use
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
       An interface between the operating system and program. The operation system provides services(or System Calls) that enables programs to to interact with the operating system. Some of these system calls have been used to create the shell.

       - fork() is a system call to create a new child process that is identical to the parent process ie. the process that created the new process. fork() returns negative when unsuccessful, zero in the child when succesfull and a positive value equal to the child PID in the parent. 

       - exec() is a family of functions that replaces the current process image with a new process image. We have used execvp(const char *file, char *const argv[]) where the first argument is the name of the binary file to be executed. These files can in Ubuntu be found at '/usr/bin' and is named by the command "ls", "rm", "mkdir", etc. The second argument char *const argv[] is an array of pointers to null-terminated strings that represent the argument list available to the new program. Since the current process is replaced, there is only a return value if it was unsuccessful. 

       - waitpid() are used to wait for a child to be terminated of the parent process. 

       - pipe(int pipefd[2]) is used to transfer the output from one process to another process as input. The array pipefd is used to return two file descriptors referring to the ends of the pipe. pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end of the pipe.

       - dup2(int oldfd, int newfd) creates a copy of the old file descriptor and dublicates it to the new file descriptor. Closing and reusing these file descriptors are done automatically, which makes is easier to use, than dub(). 

       - chdir(const char *path) takes a path as parameter to change the working directory. To take absolute paths into account, as well as ‘cd ~’, ‘cd .’ , and no path ‘cd’, we have to process and manipulate the path string.

       - getenv(char* name) is used to fetch a pointer to the value where the enviroment variable's path is held. Manipulating the value returned from this function would alter the actual value at this memmory address. This however is only process specific and would not result in corruption of ones system. No copy of the value is made, that has to be done by ourselves.

    I/O Redirection
        We have only implemented I/O redirection through the use of pipes (or the pipe operator '|'). Use of the pipe operator in the shell is described in the 'Manual for use' section.
        We have not had time to support the 'greater than' operator '>', which normally allows the user to redirect the output of a program, like pipe, but to a file instead.

    Program Environment
        
    Background Program Execution
        
