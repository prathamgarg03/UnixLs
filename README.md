# UnixLs

This project is a simple implementation of the Unix `ls` command in C. It provides a way to list files and directories in a given path, with options for long listing and showing inode numbers.

## Features

- List files and directories in a given path.
- Option to show inode numbers (`-i`).
- Option for long listing (`-l`).

## Usage

Compile the `main.c` file using a C compiler:

```bash
gcc main.c -o unixls
```

Run the compiled program with the desired options and paths:

```bash
./unixls -l -i /path/to/directory
```

## Options

- `-i`: Show inode numbers.
- `-l`: Use a long listing format to display file/directory details.

## Code Structure

The code is structured into several functions, each responsible for a specific task:

- `initializeArray`: Initializes an array of strings.
- `addWhereEmpty`: Adds a string to the first empty position in an array.
- `printInode`: Prints the inode number of a file/directory.
- `printLongList`: Prints detailed information about a file/directory in long listing format.
- `listFiles`: Lists files and directories in a given path, with options for showing inode numbers and long listing.
- `main`: Parses command line arguments and calls the `listFiles` function.

## Limitations

- The maximum path length is defined as 1024 characters.
- The maximum name length is defined as 1024 characters.
- The program does not handle symbolic links in the same way as the original `ls` command.

## Contributing

This project is open for improvements and bug fixes. Feel free to submit a pull request if you have any changes to propose.
