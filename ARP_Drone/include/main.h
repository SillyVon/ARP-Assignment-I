#include <ncurses.h>
#include <string.h>
#include <unistd.h> 
#include <math.h>
#include <time.h>
#include <stdlib.h>
#define M 1.0
#define K 1.0
#define T 0.1

FILE *log_file;
time_t current_time;
struct tm *time_info;
char time_string[20];

static float xi_minus_2 ; // the 2.previous postion on x
static float xi_minus_1 ; // the previous postion on x
static float yi_minus_2 ;// the 2.previous postion on Y
static float yi_minus_1 ; // the  previous positon on Y

//Declration 

//Drone coordinates 
float ee_y; // the current position 
float ee_x;  // the current position 

char direction_input;
int motion_direction = 0; // 0: No motion, 1: North, 2: South, 3: West, 4: East, 5: Northeast, 6: Northwest, 7: Southwest, 8: Southeast

//forces increments 
const int force_increment = 1; //incrementing by 1 N
const int  force_decrement =-1;// decrementing by 1 N

typedef struct{
    int on_x;
    int on_y;
}Forces; 

Forces forces;

//Functions 

// resetting function 
void reset_program(){
    // Reset all relevant variables to their initial values
    xi_minus_2 = 0;
    xi_minus_1 = 0;
    yi_minus_2 = 0;
    yi_minus_1 = 0;
    ee_x = 0;
    ee_y = 0;
    motion_direction = 0;
    forces.on_x = 0;
    forces.on_y = 0;
}

//function to recieve the direction from the user 
int direction_tracker (char direction){

    switch (direction) {
    
        case 'E':
        case 'e':
           return 1; // move North
            break;
        case 'C':
        case 'c':
            return 2; // move South
            break;
        case 'S':
        case 's':
            return 3; // move West
            break;
        case 'F':
        case 'f':
            return 4; // move East
            break;
        case 'R':
        case 'r':
            return 5; // move Northeast
            break;
        case 'W':
        case 'w':
            return 6; // move Northwest
            break;
        case 'X':
        case 'x':
            return 7; // move Southwest
            break;
        case 'V':
        case 'v':
            return 8; // move Southeast
            break;
        case 'Q':
        case 'q':
            // pressing 'Q' terminates the program
            endwin();
            exit(EXIT_SUCCESS);

        case 'D':
        case 'd':
             return 0; //pressing 'D' stops the drone 
             break;
        case 'Z':
        case 'z':
             reset_program();
             break;
        default:
            break;
    }  

	}


typedef struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}DRONE_BORDER;

typedef struct {
	int startx, starty;
	int height, width;
	DRONE_BORDER border;
}DRONE;

// limits for the field dimenstions.
int DRONE_X_LIM = 100;
int DRONE_Y_LIM = 20;

// size of the buttons.
int BTN_SIZE = 7;

// Drone structure variable
DRONE drone;

// Pointers to button windows
WINDOW *stp_button, *rst_button , *quit_button;

// Initialize the drone structure and parameters :

void make_drone_field() {

    // limits for the field dimenstions.
    drone.height = DRONE_Y_LIM;
    drone.width  = DRONE_X_LIM;

    // postion of the borders
    drone.starty = 6;
    drone.startx = (COLS - drone.width) / 2;

    drone.border.tl = ACS_ULCORNER;
    drone.border.tr = ACS_URCORNER;
    drone.border.bl = ACS_LLCORNER;
    drone.border.br = ACS_LRCORNER;
}

// drawing the drone and the structure. 

void draw_drone_field() {
    int x, y, w, h;

    x = drone.startx;
    y = drone.starty;
    w = drone.width;
    h = drone.height;

    // Draw the entire drone structure

    mvaddch(y - 1, x - 1, drone.border.tl);
    mvaddch(y - 1, x + w, drone.border.tr);
    mvaddch(y + h - 1, x - 1, drone.border.bl);
    mvaddch(y + h - 1, x + w, drone.border.br);

    refresh();
}


// Place a title  with the drones coordinates on top

void draw_drone_msg(float x, float y) {
// clearing the line for the title to be printed
    for(int j = 0; j < COLS; j++) {
        mvaddch(drone.starty - 2, j, ' ');
    }
// printing the drones coordinates
    char msg[100];
    sprintf(msg, "The Drones Coordinates: (%05.2f, %.2f)", x, y);

    attron(A_BOLD);
    mvprintw(drone.starty - 2, (COLS - strlen(msg)) / 2 + 1, msg);
    attroff(A_BOLD);
}

// Draw drone within the structure
void draw_drone_at(float ee_x, float ee_y) {
    // Clear the drone area
    for (int j = 0; j < drone.width; j++) {
        for (int i = 0; i < drone.height; i++) {
            mvaddch(drone.starty + i, drone.startx + j, ' ');
        }
    }

    // Convert real coordinates to integer
    int ee_x_int = floor(ee_x);
    int ee_y_int = floor(ee_y);

    // Draw the Drone
    attron(A_BOLD | COLOR_PAIR(4));
    mvaddch(drone.starty + ee_y_int, drone.startx + ee_x_int, '+');
    attroff(A_BOLD | COLOR_PAIR(4));

    
    refresh();
}


// Utility method to check for drone within limits
void check_ee_within_limits(float* ee_x, float* ee_y) {

    // Checks for horizontal axis
    if(*ee_x <= 0) {
        *ee_x = 0;
    }
    else if(*ee_x >= DRONE_X_LIM) {
        *ee_x = DRONE_X_LIM - 1;
    }
   
    // Checks for vertical axis
    if(*ee_y <= 0) {
        *ee_y = 0;
    }
    else if(*ee_y >= DRONE_Y_LIM) {
        *ee_y = DRONE_Y_LIM - 1;
    }
}


// Create button windows
 void make_buttons() {

    int stp_button_startx = (COLS - 2 * BTN_SIZE - 5) / 2 + 5;
    int rst_button_startx = (COLS - BTN_SIZE + 11) / 2 + 5;
    int quit_button_startx = (COLS - 6* BTN_SIZE)/2 + 5 ;
    int buttons_starty = 35;

    stp_button = newwin(BTN_SIZE / 2, BTN_SIZE, buttons_starty, stp_button_startx);
    rst_button = newwin(BTN_SIZE / 2, BTN_SIZE, buttons_starty, rst_button_startx);
    quit_button= newwin(BTN_SIZE / 2, BTN_SIZE, buttons_starty,quit_button_startx);
}


// Draw button with colored background and custom color
void draw_btn(WINDOW *btn, char label, int color, int box_color) {

    wbkgd(btn, COLOR_PAIR(box_color));
    wmove(btn, BTN_SIZE / 4, BTN_SIZE / 2);

    attron(A_BOLD | COLOR_PAIR(color));
    waddch(btn, label);
    attroff(A_BOLD | COLOR_PAIR(color));

    wrefresh(btn);
}

// Draw all buttons, prepending label message
void draw_buttons() {

    char* msg = "Drone Simulator";
    move(2, (COLS - strlen(msg)) / 2);
    attron(A_BOLD);
    printw(msg);
    attroff(A_BOLD);

    draw_btn(stp_button, 'Z', 2, 2);      // 'S' with color pair 2 for text, 2 for box
    draw_btn(rst_button, 'D', 3, 3);      // 'R' with color pair 3 for text, 3 for box
    draw_btn(quit_button, 'Q', 2, 2);     // 'Q' with color pair 1 for text, 4 for box
}

void init_console_ui() {

    // Initialize curses mode
    initscr();		
	start_color();

    // Disable line buffering...
	cbreak();

    // Disable char echoing and blinking cursos
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLUE , COLOR_BLACK);

    // Initialize UI elements
    make_buttons();
    make_drone_field();

    
    // Set the initial position of the drone (replace these values with your desired initial position)
    float initial_ee_x = 0;
    float initial_ee_y = 1;
   
    // draw UI elements
    draw_drone_field();
    draw_buttons();
    draw_drone_msg(initial_ee_x, initial_ee_y);
    draw_drone_at (initial_ee_x, initial_ee_y);
    refresh();
    
    // Activate input listening (keybord  ...)
    keypad(stdscr, TRUE);
    
    
}

void update_console_ui(float *ee_x, float *ee_y) {

    // check if next drone position is within limits
    check_ee_within_limits(ee_x, ee_y);

    // Draw updated drone position
    draw_drone_at(*ee_x,*ee_y);

    // Update string message for drone position
    draw_drone_msg(*ee_x, *ee_y);

    refresh();

}

void reset_console_ui() {

    // Free resources
    delwin(stp_button);
    delwin(rst_button);
    delwin(quit_button);

    // Clear screen
    erase();

    // Re-create UI elements
    make_drone_field();
    make_buttons();

    // draw UI elements
    draw_drone_field();
    draw_drone_msg(0, 0);
    draw_buttons();

    refresh();
}

