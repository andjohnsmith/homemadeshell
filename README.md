# Homemade Shell

This program is a reproduction of the shell. It has five built-in commands:
wait, fg, history, jobs, and exit. Anything else put into the command line is
treated as an executable. File redirection is implemented with '<' and '>'.

## How to build the software

Run make.

## How to use the software

Start the program with ./mysh for interactive mode. Then, the program can be
used just as any other shell. The wait command waits until all jobs are
complete. The fg command brings the specified job to the foreground or the most
recent if none was specified. History lists all the jobs run. Jobs lists
currently running jobs, and exit waits for all current jobs to finish before
exiting the program. Batch mode is also available if one or more files are
indicated along the initial ./mysh command.

## How the software was tested

Tested variety of commands and executables against the behavior of a MacBook's
Terminal.

To run the file redirection tests the test programs must be built

Test1 - Tests basic commands as well as the built in wait command
Test2 - Tests fg and file redirection that outputs input into a file
Test3 - Tests file redirection for a program that takes input
Test4 - Tests multiple commands, the jobs command, and the exit command
Test5 - Tests a large amount of commands back to back as well as the history command

inputFile.txt - is for file redirection and for reading a value and printing it out
test.txt - is for file redirection and for displaying a value
input.c - was a test program for taking input
hello.c - was a test program to print out "hello"
