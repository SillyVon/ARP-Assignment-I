
# ARP ASSIGNMENT I
The Drone Operation Interactive Simulator is a C-based software application designed to provide a realistic simulation of drone operation in a 2-degree-of-freedom environment. Users can control the drone using keyboard inputs, experiencing dynamic movements with mass and viscous resistance.

## Dependencies

This project utilizes the following libraries:

### Standard C Libraries

- `stdio.h`
- `stdlib.h`
- `sys/types.h`
- `unistd.h`
- `sys/wait.h`
- ...

### POSIX Libraries

- `sys/mman.h`
- `fcntl.h`
- `semaphore.h`
- `time.h`
- ...

### Third-Party Libraries

-`ncurses.h`: Used for handling terminal input and output in a more advanced manner.
#### ncurses installation 

```console 
sudo apt-get install libncurses-dev

```
Please ensure that you have the necessary versions of these libraries installed on your system before attempting to compile and run the project.
## Key Features

Controls:

Use the following keys for navigation:
- W: Move North
- E: Move Northeast
- R: Move Northwest
- S: Move South
- D: Move East
- F: Move West
- X: Move Southwest
- C: Move Southeast

Additional controls:
- Q: Quit the program
- Z: Reset the drone
- D: Stop the drone

Drone Control: Navigate the drone in 8 directions, adjust velocity, stop, reset, suspend, and quit the simulation.

Realistic Dynamics: Modelled as a 2-degree-of-freedom dot with mass and viscous resistance for authentic movement.

Software Architecture: Includes a server managing a blackboard with the world map's geometrical state, facilitating interaction with targets and obstacles.

Configuration: All parameters, including drone characteristics, are stored in a configurable file, allowing real-time adjustments.

Inspection Window and Logging: A small window provides real-time insights. Values can be logged for further analysis.

Watchdog Process: Monitors system activity, sending notifications and stopping the system if no computation is detected within a configurable time.
## Installation

Ensure that you have the required dependencies installed on your system (details in the Dependencies section).

### Clone the repository
```console
git clone https://github.com/SillyVon/ARP-Assignment-/tree/main
```
### Navigate to the project directory
```console
cd your-repository
```

## System Components
This section provides a brief overview of the active components in the project, including their functionalities, the primitives they use, and the algorithms implemented.

### Server Component

#### Functionality:

Forking a Process:
The spawn function is used to fork a child process, and if successful, the child process executes a specified program using execvp.
The main function forks a process that opens a new terminal (konsole) and runs the main program.

Logging:
he server logs information to a file (Logs/server_log.txt) about the forking process and writes timestamps along with relevant messages.

Shared Memory:
Shared memory is created using shm_open to store process IDs (PIDs) of forked processes.
The server maps the shared memory using mmap to obtain a pointer (pid_list) for reading and writing PIDs.

Semaphores:
Two semaphores (sem_id1 and sem_id2) are created using sem_open to synchronize access to the shared memory.
sem_id1 is initialized to 1, and sem_id2 is initialized to 0.

Process Management:
the server forks a child process to run the main program and waits for it to finish using waitpid.

Clean-up:
Shared memory and semaphores are cleaned up after the processes have finished executing.

Primitives Used:

Forking:
fork() is used to create a new process.

Shared Memory:
shm_open, mmap, munmap, and shm_unlink functions are used for shared memory management.

Semaphores:
sem_open, sem_init, sem_wait, sem_post, and sem_close functions are used for semaphore management.

#### Algorithms:

No specific algorithms are implemented in this code; it mainly involves process forking, shared memory handling, semaphore usage, and logging.

#### Additional Notes:

The server uses a file (Logs/server_log.txt) for logging activities.
It opens a new terminal using konsole to run the main program.

### Main Component

#### Functionality:

Initialization:
The init_console_ui function initializes the ncurses-based console user interface, including creating buttons and initializing the drone's field.

User Input and Direction Tracking:
The program listens for user keyboard input in a non-blocking mode (getch()).
The direction_tracker function interprets the user input and determines the motion direction for the drone.

Drone Motion Control:
The program uses a switch-case structure to control the drone's motion based on the specified direction.
Forces on the X and Y axes are updated according to the selected motion direction.

Euler's method is applied for position calculation based on the forces and previous positions.

User Interface Update:
The update_console_ui function redraws the console UI, including the drone's position, buttons, and messages.
The draw_drone_at function is responsible for drawing the drone within the structure.

check_ee_within_limits function ensures that the drone stays within specified limits.

Logging:
The program logs the drone's position and forces in a log file (Logs/main_log.txt) with timestamps.

Button Handling:
The program defines and handles buttons for stopping the drone (Z), resetting the program (D),and quitting the program (Q).

Primitives Used:

ncurses Library:
Functions from the ncurses library, such as initscr, cbreak, noecho, nodelay, curs_set, keypad, init_pair, newwin, wbkgd, wmove, waddch, delwin, erase, and refresh are used for console-based user interface management.

C Standard Libraries:
Standard C libraries like stdio.h, string.h, unistd.h, math.h, and stdlib.h are used.

#### Algorithms:

Euler's Method:
Euler's method is used for position calculation based on the forces acting on the drone in the update_console_ui function.

#### Additional Notes:

The program uses a file (Logs/main_log.txt) for logging activities.
The drone's position is visually represented in the console using ncurses.
The program continuously listens for user input in a non-blocking manner.
Buttons (Z, D, Q) provide control over the drone's behavior.

### Watchdog Component
#### Functionality:

Shared Memory and Semaphores Initialization:
Opens shared memory (SHM_ID) and semaphores (SEM_PATH_1 and SEM_PATH_2) created by the server.

Waiting for Master to Write:
Uses sem_wait on sem_id_2 to wait for the server (Master) to write data to the shared memory.

Reading PIDs from Shared Memory:
Maps the shared memory into the address space for PIDs using mmap.
Displays the received PIDs and restarts the server process by signaling (SIGUSR1) it.

Watchdog Loop:
Periodically checks the status of the target processes (PIDs received from the server).
Sends a SIGUSR1 signal to each target process to check if it is responsive.

Logging:
Logs the status of the target processes, successful signals, and any unresponsive processes to a log file (Logs/watchdog_log.txt).
If a process is not responding, it sends a SIGKILL signal to terminate all processes and logs the event.

Cleanup:
Unmaps the shared memory for PIDs and unlinks the shared memory object.
Closes semaphores (sem_id1 and sem_id2).

Primitives Used:

POSIX Shared Memory:
Functions like shm_open, mmap, munmap, and shm_unlink are used for managing shared memory.

POSIX Semaphores:
Functions like sem_open, sem_wait, sem_post, and sem_close are used for semaphore management.

System Calls:
System calls like kill are used to send signals to processes.

#### Algorithms:

Watchdog Monitoring:
The watchdog periodically checks the status of target processes by sending a SIGUSR1 signal.
If a process does not respond, it sends a SIGKILL signal to terminate all processes.

#### Additional Notes:

The program opens existing shared memory and semaphores created by the server.
The watchdog periodically checks the responsiveness of the target processes and logs the status.
If any target process is unresponsive, it terminates all processes and logs the event.


## Quick Architecture Sketch
![Alt text](ARP_Drone/Sketch/sketch.png)


## Getting Started 

To compile and run the project, open a terminal and execute the following command:
```console
bash run.sh
```
This script compiles the source files (server.c, main.c, and watchdog.c) using GCC with the necessary flags and then executes the compiled server binary.

Using the Makefile

Alternatively, you can use the Makefile located in the root directory of the project. Open a terminal and run the following commands:
```console
make
```
This will compile all source files and generate the corresponding binaries in the bin directory. After compilation, you can run the server using:
```console
./bin/server
```

