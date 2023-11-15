#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LENGTH_COMMAND 300
#define MAX_NO_TASKS 100
#define MAX_LENGTH_TITLE 100
#define MAX_LENGTH_DESCRIPTION 200
#define MAX_LENGTH_TIME 33
#define WEEK_CELL_FIRST_COL_WIDTH 10
#define WEEK_CELL_OTHER_COL_WIDTH 20

enum Status {IN_PROGRESS, DONE, ARCHIVED};
char * status_name[] = {"In Progress", "Done", "Archived"};
enum CommandType {ADD, EDIT, SHOW, DELETE, QUIT, INVALID};
char * command_name[] = {"ADD", "EDIT", "SHOW", "DELETE", "QUIT", "INVALID"};

struct Task {
    int num;
    char title[MAX_LENGTH_TITLE+1];
    char description[MAX_LENGTH_DESCRIPTION+1];
    char start_time[MAX_LENGTH_TIME+1];
    char end_time[MAX_LENGTH_TIME+1];
    enum Status status;
    
};

bool isLeapYear(int year) {
    return year % 400 == 0 || (year % 100 != 0 && year % 4 == 0);
}

int getMaxDay(int month, int year) {
    int maxDay;
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            maxDay = 31;
            break;
        case 4: case 6: case 9: case 11:
            maxDay = 30;
            break;
        case 2:
            maxDay = isLeapYear(year) ? 29 : 28;
            break;
        default:
            maxDay = -1; // invalid month
    }
    return maxDay;
}
bool isDateTimeValid(const char *dateTime) {
    int hh, mm, dd, mo, yyyy;
    char sep1, sep2, sep3;

    int itemsScanned = sscanf(dateTime, "%2d%[:]%2d%[|]%2d%[/]%2d%[/]%4d", 
                              &hh, &sep1, &mm, &sep2, &dd, &sep3, &mo, &yyyy);

    bool formatIsValid = itemsScanned == 7
        && sep1 == ':' && sep2 == '|'
        && sep3 == '/'
        && hh >= 0 && hh <= 23
        && mm >= 0 && mm <= 59
        && dd >= 1 && dd <= getMaxDay(mo, yyyy)
        && mo >= 1 && mo <= 12
        && yyyy >= 1900;

    return 1;
}
void getTitleFromAdd(char * command, char * out_title) {
    char* start = strchr(command, '[');
    char* end = strchr(command, ']');
    if (start && end && end>start) {
        strncpy(out_title, start+1, end-start-1);
        out_title[end-start-1] = '\0';
    } else {
        out_title[0] = '\0';
    }
}

void getDescriptionFromAdd(char * command, char * out_description) {
    char* start = strchr(strchr(command, '[')+1, '[');
    char* end = strchr(start+1, ']');
    if (start && end && end>start) {
        strncpy(out_description, start+1, end-start-1);
        out_description[end-start-1] = '\0';
    } else {
        out_description[0] = '\0';
    }
}
    void getTimeFromAdd(const char *command, char *out_startTime, char *out_endTime) {
        char* start = strchr(command, '[');
        if (start) {
            start = strchr(start+1, '[');
        }
        if (start) {
            start = strchr(start+1, '[');
        }
        char* middle = start ? strchr(start+1, '-') : NULL;
        char* end = middle ? strchr(middle+1, ']') : NULL;

        if (start && middle && end && end > middle && middle > start) {
            // Find the '|' symbol in the start time string
            char* start_pipe = strchr(start, '|');
            if (start_pipe && start_pipe < middle) {
                // Copy the time part before the '|' symbol
                strncpy(out_startTime, start+1, start_pipe-start-1);
                out_startTime[start_pipe-start-1] = '\0';
            } else {
                out_startTime[0] = '\0';
            }

            // Find the '|' symbol in the end time string
            char* end_pipe = strchr(middle, '|');
            if (end_pipe && end_pipe < end) {
                // Copy the time part before the '|' symbol
                strncpy(out_endTime, middle+1, end_pipe-middle-1);
                out_endTime[end_pipe-middle-1] = '\0';
            } else {
                out_endTime[0] = '\0';
            }
        } else {
            out_startTime[0] = '\0';
            out_endTime[0] = '\0';
        }
    }
void printTask(struct Task * task) {
    printf("--------------------------------------------\n");
    printf("Num: #%d. Title: %s\n", task->num, task->title);
    printf("Description: %s\n", task->description);
    printf("Start Time: %s\n", task->start_time);  // Corrected line
    printf("End Time: %s\n", task->end_time);  // Corrected line
    printf("Status: %s\n", status_name[task->status]);
    printf("--------------------------------------------\n");
}

void printUnsupportedTime(struct Task * task) {
    printf("----- Show week view -----\n");
    printf("Error: Unsupported time with non-zero minutes: %s\n", task->start_time); // Corrected line
    printf("In Task:\n");
    printTask(task);
}

enum CommandType getCommandType(char * command) {
    if (strncmp(command, "ADD", 3) == 0)
        return ADD;
    else if (strncmp(command, "EDIT", 4) == 0)
        return EDIT;
    else if (strncmp(command, "SHOW", 4) == 0)
        return SHOW;
    else if (strncmp(command, "DELETE", 6) == 0)
        return DELETE;
    else if (strncmp(command, "QUIT", 4) == 0)
        return QUIT;
    else 
        return INVALID;
}

struct Task tasks[MAX_NO_TASKS];
int num_tasks = 0;

void addTask(char * title, char * description, char * start_time, char * end_time) {
    struct Task new_task;
    new_task.num = num_tasks;
    strncpy(new_task.title, title, MAX_LENGTH_TITLE);
    new_task.title[MAX_LENGTH_TITLE] = '\0'; // Ensure null termination
    strncpy(new_task.description, description, MAX_LENGTH_DESCRIPTION);
    new_task.description[MAX_LENGTH_DESCRIPTION] = '\0'; // Ensure null termination
    strncpy(new_task.start_time, start_time, MAX_LENGTH_TIME); // Corrected line
    new_task.start_time[MAX_LENGTH_TIME] = '\0'; // Ensure null termination
    strncpy(new_task.end_time, end_time, MAX_LENGTH_TIME); // Corrected line
    new_task.end_time[MAX_LENGTH_TIME] = '\0'; // Ensure null termination
    new_task.status = IN_PROGRESS;
    tasks[num_tasks++] = new_task;
    
    // Print the task after it has been added.
    printTask(&new_task);
}
int checkTitle(char * raw_title) {
    int len = strlen(raw_title);
    
    // Check if the title exceeds the maximum length
    if (len > MAX_LENGTH_TITLE) {
        return len;
    }

    // Check for other conditions. Here, check if the title contains any special characters
    for (int i = 0; i < len; i++) {
        if (!(raw_title[i] >= 'A' && raw_title[i] <= 'Z') && 
            !(raw_title[i] >= 'a' && raw_title[i] <= 'z') && 
            !(raw_title[i] >= '0' && raw_title[i] <= '9') &&
            raw_title[i] != ' ') {
            return i;  // Return the position of the first character that violates the conditions
        }
    }

    return -1;  // If the title is valid, return -1
}
int checkDescription(char * raw_description) {
    int len = strlen(raw_description);
    
    // Check if the description exceeds the maximum length
    if (len > MAX_LENGTH_DESCRIPTION) {
        return len;
    }

    // Check for other conditions. Here, let's check if the description contains any special characters
    for (int i = 0; i < len; i++) {
        if (!(raw_description[i] >= 'A' && raw_description[i] <= 'Z') && 
            !(raw_description[i] >= 'a' && raw_description[i] <= 'z') && 
            !(raw_description[i] >= '0' && raw_description[i] <= '9') &&
            raw_description[i] != ' ') {
            return i;  // Return the position of the first character that violates the conditions
        }
    }

    return -1;  // If the description is valid, return -1
}
int compareDateTimes(const char *dateTime1, const char *dateTime2) {
    // Returns -1 if dateTime1 < dateTime2, 0 if they're equal, 1 if dateTime1 > dateTime2
    return strcmp(dateTime1, dateTime2);
}
void runTodoApp() {
    char command[MAX_LENGTH_COMMAND+1];
    char title[MAX_LENGTH_TITLE+1];
    char description[MAX_LENGTH_DESCRIPTION+1];
    char start_time[MAX_LENGTH_TIME+1];
    char end_time[MAX_LENGTH_TIME+1];
    
    while (true) {
        fgets(command, MAX_LENGTH_COMMAND+1, stdin);

        // Remove the trailing newline if it exists
        if (command[strlen(command) - 1] == '\n') {
            command[strlen(command) - 1] = '\0';
        }

        enum CommandType commandType = getCommandType(command);
        printf("Command     : %s\n", command);
        printf("Command type: %s\n", command_name[commandType]);
        
        if (commandType == ADD) {
            getTitleFromAdd(command, title);
            getDescriptionFromAdd(command, description);
            getTimeFromAdd(command, start_time, end_time); // This function should now extract two times
            
            int titleCheck = checkTitle(title);
            int descCheck = checkDescription(description);
            bool startIsValid = isDateTimeValid(start_time);
            bool endIsValid = isDateTimeValid(end_time);
            int timeOrder = compareDateTimes(start_time, end_time);
            
            if (titleCheck == -1 && descCheck == -1 && startIsValid && endIsValid && timeOrder < 0) {
                addTask(title, description, start_time, end_time); // This function should now accept two times
                printf("Task added: %s\n", title);
            } else {
                if (titleCheck > MAX_LENGTH_TITLE) {
            printf("Title is too long. Current length: %d\n", titleCheck);
        } else if (titleCheck != -1) {
            printf("Invalid character in title at position: %d\n", titleCheck);
        }

        if (descCheck > MAX_LENGTH_DESCRIPTION) {
            printf("Description is too long. Current length: %d\n", descCheck);
        } else if (descCheck != -1) {
            printf("Invalid character in description at position: %d\n", descCheck);
        }
                if (!startIsValid) {
                    printf("Invalid start time format.\n");
                }
                
                if (!endIsValid) {
                    printf("Invalid end time format.\n");
                }
                
                if (startIsValid && endIsValid && timeOrder >= 0) {
                    printf("End time must be later than start time.\n");
                }
            }
    
        } else if (commandType == QUIT) {
            break;
        }
    }
}
int main() {
    runTodoApp();
    return 0;
}