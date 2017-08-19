A simple program that takes commands from the user at the command line and perform read/write/append operations on the user specified file.

Example command:

read < readFile | write > writeFile | append >> appFile

Would read the contents of readFile, then pipe the contents to the write command which would write them to writeFile and then pipe the contents to the append command which would append the contents to the appFile.
