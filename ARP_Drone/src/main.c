#include "./../include/main.h"
  
int main (int argc, char const *argv[]){

// Utility variable to avoid trigger resize event on launch
int first_resize = TRUE;

// Initialize User Interface and 
init_console_ui();

//open the log for writing 
log_file = fopen("Logs/main_log.txt","w");
if (log_file==NULL){
    fprintf(stderr,"Error opening the file for writing.\n");
   return -1;
}

// Infinite loop
    while (TRUE) {

    //Get keyboard commands in non-blocking mode
    direction_input = getch();
    
    // determine the direction
    motion_direction = direction_tracker (direction_input);
  
    // Switch case for the 8 directions:
    switch (motion_direction){
        case 1:  // North
          forces.on_y += force_decrement;
            break;
        case 2:  // South
            forces.on_y += force_increment;
            break;
        case 3:  // West
            forces.on_x += force_decrement;
            break;
        case 4:  // East
            forces.on_x += force_increment;
            break;
        case 5:  // Northeast
           forces.on_x += force_increment;
           forces.on_y += force_decrement;
            break;
        case 6:  // Northwest
            forces.on_x += force_decrement;
            forces.on_y += force_decrement;
            break;
        case 7:  // Southwest
            forces.on_x += force_decrement;
            forces.on_y += force_increment;
            break;
        case 8:  // Southeast
            forces.on_x += force_increment;
            forces.on_y += force_increment;
            break;
        case 0 :
            forces.on_x=0;
            forces.on_y=0;
            break;
        
        default:
        // Handle invalid motion direction
        
        break;
    }

    // Euler's method for position calculation
    ee_x = ((2 * M + K * T) * xi_minus_1 - M * xi_minus_2 + T * T * forces.on_x) / (K * T + M);
    ee_y = ((2 * M + K * T) * yi_minus_1 - M * yi_minus_2 + T * T * forces.on_y) / (K * T + M);

    xi_minus_2 = xi_minus_1;
    xi_minus_1 = ee_x;
    yi_minus_2 = yi_minus_1;
    yi_minus_1 = ee_y;

    update_console_ui(&ee_x,&ee_y);
    // Logging

    // Get current time
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", time_info);
    //logging the position and forces 
    fprintf(log_file, "[%s] Position: (%.2f, %.2f)\n", time_string, ee_x, ee_y);
    fprintf(log_file, "[%s] Forces on X : %d,on Y : %d\n",time_string,forces.on_x,forces.on_y);
    // Sleep or delay for visualization purposes
    usleep(100000);
}
    // closing the file
    fclose(log_file);
    // Terminate
    endwin();
    return 0;
}









