DESCRIPTION
    We have created a shell program, written in C, that runs an interactive prompt taking commands.

    - The shell shows the users current working directory at all times.

    - The shell has the ability to execute any program placed in the /bin/ folder on Linux systems, such as ls, wc, pwd with zero or more arguments.

    - The shell support the 'cd' command (Change Diretory). 'cd' supports absolute and relative pathing. Absolute paths have to start with '/', whereas relative does not.
      The 'cd' command also supports using '~' (HOME path) to refere to the homepath enviroment variable. Using 'cd' with no arguments, changes by default the users working directory to HOME.

    - The shell support the 'history' command. All commands are logged to a .shell_history file in the home directory. The history command prints the contents of the .shell_history file. 
    
    - The shell supports piping between proceses, using the pipe operator '|'. The shell supports pipe-chaining, meaning you can create pipes between multiple processes in a single command to the shell. 


MANUAL for setup
    - The setup can only be compiled if GCC is supported.

    1. Extract the zip file.
    2. Navigate to the extracted directory with a terminal.
    3. Type 'make'.
    4. To start the program, type './shell' in the terminal.

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
    - You can use all commands that are in your PATH.
    - This means that the shell supports: "wc", "pwd", "ls", "cat", "curl" and many more. All theses commands accept zero or more arguments.
    - If you need to specify path or a sentence that includes spaces, you can enclose the token in quotes like so "Hello World" to let the parser interpret it as a single token.

    Process Piping
    - You can pipe an output from a process as an input for another. You can do this using the pipe operator '|' between commands.
    - This syntax "ls -la | wc -l", starts a process "ls" that takes a argument "-la", this process outputs a lists of all elements in a directory and sends it as an input for the command "wc -l", which counts the number of lines of the input.
    - This can be chained further such as "ls -la | wc -l | cat".


CONCEPTS
    System Calls
       An interface between the operating system and program. The operation system provides services(or System Calls) that enables programs to to interact with the operating system. Some of these system calls have been used to create the shell.

       - fork() is a system call to create a new child process that is identical to the parent process ie. the process that created the new process. fork() returns negative when unsuccessful, zero in the child when succesfull and a positive value equal to the child PID in the parent. [4]

       - exec() is a family of functions that replaces the current process image with a new process image. We have used execvpe(const char *file, char *const argv[], char *const envp[]) where the first argument is the name of the binary file to be executed. These files can be found in the users PATH  and is named by the command "ls", "rm", "mkdir", etc. The second argument char *const argv[] is an array of pointers to null-terminated strings that represent the argument list available to the new program. This list start with the current command at hand and end with a NULL pointer to mark the end. The last argument to execvpe is our environment and in our case the PATH variable which is use to tell where to look for the binary files. Since the current process is replaced, there is only a return value if it was unsuccessful. [3]

       - waitpid() are used to wait for a child to be terminated of the parent process. [5]

       - pipe(int pipefd[2]) is used to transfer the output from one process to another process as input. The array pipefd is used to return two file descriptors referring to the ends of the pipe. pipefd[0] refers to the read end of the pipe.  pipefd[1] refers to the write end of the pipe. [2]

       - dup2(int oldfd, int newfd) creates a copy of the old file descriptor and dublicates it to the new file descriptor. Closing and reusing these file descriptors are done automatically, which makes is easier to use, than dub(). [1]

       - chdir(const char *path) takes a path as parameter to change the working directory. To take absolute paths into account, as well as ‘cd ~’, ‘cd .’ , and no path ‘cd’, we have to process and manipulate the path string. [7]

       - getenv(char* name) is used to fetch a pointer to the value where the enviroment variable's path is held. Manipulating the value returned from this function would alter the actual value at this memmory address. This however is only process specific and would not result in corruption of ones system. No copy of the value is made, that has to be done by ourselves. [11]

    I/O Redirection
        We have only implemented I/O redirection through the use of pipes (or the pipe operator '|'). Use of the pipe operator in the shell is described in the 'Manual for use' section.
        The way we redirect streams are by using the file descriptor '0' and '1'. '0' is the standard input, and '1' is the standard output stream. What we do is that we basically map the file descriptor created for the pipe to a respective standard input or output stream. Lets take an example:
        Lets say we want the output for a process to go into the file descriptor opened with pipe(). We then duplicate the file descriptor for the read-end of the pipe to stdout (Standard Output) as so: dup2(fd: int, 1), where the '1' is the number for the file descriptor for stdout.
        This can be done reversly for the read-end, to be mapped to the stdin (Standard Input). [2]

        We have not had time to support the 'greater than' operator '>', which normally allows the user to redirect the output of a program, like pipe, but to a file instead.

    Program Environment
        The Shell is to be executed from within a terminal (another shell). The shell has no window or UI, its purely text-based. This means that the shell has to be ran within another shell. The Shell will be running as a process within the hosted shell. Its a shell running a shell. This means, that whenever our shell exits, the hosting shell will still be running. This is because the hosting shell will be waiting for our shell's process to finish before moving on. 
        
BIBLIOGRAPHY
    [1]     IEEE/The Open Group (2017, bash dup2 manual page)
    [2]     IEEE/The Open Group (2017, bash pipe manual page)
    [3]     GNU (2021, bash execvpe manual page)
    [4]     Free Software Foundation (2018, bash fork manual page)
    [5]     IEEE/The Open Group (2017, bash waitpid manual page)
    [6]     IEEE/The Open Group (2017, bash wait manual page)
    [7]     IEEE/The Open Group (2017, bash chdir manual page)
    [8]     GNU (2021, bash getline manual page)
    [9]     GNU (2021, bash strtok manual page)
    [10]    IEEE/The Open Group (2017, bash environ manual page)
    [11]    GNU (2021, bash getenv manual page)
    [12]    https://stackoverflow.com/questions/26187037/in-c-split-char-on-spaces-with-strtok-function-except-if-between-quotes 
