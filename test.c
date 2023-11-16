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

enum CommandType { ADD, EDIT, SHOW, DELETE, QUIT, INVALID };
const char* command_name[] = { "ADD", "EDIT", "SHOW", "DELETE", "QUIT", "INVALID" };
typedef enum CommandType CommandType;
CommandType getCommandType(char* command) {
    char* space = strchr(command, ' ');
    int length = (space != NULL) ? space - command : strlen(command);
    char first_word[length + 1];
    strncpy(first_word, command, length);
    first_word[length] = '\0';

    for (int i = 0; i < sizeof(command_name) / sizeof(command_name[0]); i++) {
        if (strcmp(first_word, command_name[i]) == 0) {
            return i;
        }
    }

    return INVALID;
}

void getTitleFromAdd(char* command, char* out_title) {
    char* start = strstr(command, "[");
    char* end = strstr(command, "]");
    if (start && end && end > start) {
        strncpy(out_title, start + 1, end - start - 1);
        out_title[end - start - 1] = '\0';
    }
}

void getDescriptionFromAdd(char* command, char* out_description) {
    char* start = strstr(command, "] [");
    if (start) {
        start += 3; // To skip the "] ["
        char* end = strstr(start, "]");
        if (end && end > start) {
            strncpy(out_description, start, end - start);
            out_description[end - start] = '\0';
        }
    }
}

void getTimeFromAdd(char* command, char* out_time) {
    char* start = strrchr(command, '['); // To find the last occurrence of [
    if (start) {
        start += 1; // To skip the "["
        char* end = strstr(start, "]");
        if (end && end > start) {
            strncpy(out_time, start, end - start);
            out_time[end - start] = '\0';
        }
    }
}



int checkTitle(char * raw_title) {
    // Check if the title length is within the allowed limit
    if (strlen(raw_title) > MAX_LENGTH_TITLE) {
        return strlen(raw_title); // Return the current length of the title
    }
    // Check if the title starts or ends with a whitespace character
    if (raw_title[0] == ' ') {
        return 0; // Error: Title starts or ends with a whitespace character
    } else if (raw_title[strlen(raw_title) - 1] == ' ') {
        return strlen(raw_title);
    }
    
    // Define allowed characters for the title
    char allowedCharacters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,.-:|/";

    // Check if the title contains only allowed characters
    for (int i = 0; raw_title[i] != '\0'; i++) {
        int isValidChar = 0;
        for (int j = 0; allowedCharacters[j] != '\0'; j++) {
            if (raw_title[i] == allowedCharacters[j]) {
                isValidChar = 1;
                break;
            }
        }
        if (!isValidChar) {
            return i; // Return the position of the first invalid character
        }
    }

    return -1;
}
int checkDescription(char* raw_description) {
    int length = strlen(raw_description);

    // Check maximum length condition
    if (length > MAX_LENGTH_DESCRIPTION) {
        return length;
    }

    // Check other conditions
    for (int i = 0; i < length; i++) {
        char ch = raw_description[i];

        // Check if it's a valid character
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
              (ch >= '0' && ch <= '9') || ch == ' ' || ch == ',' ||
              ch == '.' || ch == '-' || ch == ':' ||
              ch == '|' || ch == '/')) {
            return i;
        }

        // Check for leading or trailing whitespace
        if (i == 0 && ch == ' ') {
            return i;
        }
        if (i == length - 1 && ch == ' ') {
            return i;
        }
    }

    return -1; // Description is valid
}
int checkTime(char* raw_time) {
    // Extract datetime1 and datetime2 from raw_time
    char datetime1[20];
    char datetime2[20];
    sscanf(raw_time, "%[^-]-%s", datetime1, datetime2);

    // Validate datetime1
    int hh1, mm1, dd1, mo1, yyyy1;
    if (sscanf(datetime1, "%d:%d|%d/%d/%d", &hh1, &mm1, &dd1, &mo1, &yyyy1) != 5) {
        return 11; // Invalid datetime1
    }

    // Validate datetime2
    int hh2, mm2, dd2, mo2, yyyy2;
    if (sscanf(datetime2, "%d:%d|%d/%d/%d", &hh2, &mm2, &dd2, &mo2, &yyyy2) != 5) {
        return 12; // Invalid datetime2
    }

    // Check if datetime2 is earlier than datetime1
    if (yyyy2 < yyyy1 || (yyyy2 == yyyy1 && (mo2 < mo1 || (mo2 == mo1 && (dd2 < dd1 || (dd2 == dd1 && (hh2 < hh1 || (hh2 == hh1 && mm2 < mm1)))))))) {
        return 0; // datetime2 is earlier than datetime1
    }

    return -1; // Time is valid
}
// Test the functions
int main() {
    char sample_input[] = "ADD [Course Intro to Programming] [Room 701-H6] [07:00|03/10/2023-12:00|01/10/2023]";
    char raw_title[50];
    char raw_description[50];
    char raw_time[50];

    getTitleFromAdd(sample_input, raw_title);
    getDescriptionFromAdd(sample_input, raw_description);
    getTimeFromAdd(sample_input, raw_time);

    printf("Title: %s\n", raw_title);
    printf("Description: %s\n", raw_description);
    printf("Time: %s\n", raw_time);

    // Test checkTitle function
    int titleCheck = checkTitle(raw_title);
    if (titleCheck == -1) {
        printf("Title is valid.\n");
    } else if (titleCheck == strlen(raw_title)) {
        printf("Title length exceeds the maximum limit.\n");
    } else {
        printf("Title is invalid at position %d.\n", titleCheck);
    }

    // Test checkDescription function
    int descriptionCheck = checkDescription(raw_description);
    if (descriptionCheck == -1) {
        printf("Description is valid.\n");
    } else if (descriptionCheck == strlen(raw_description)) {
        printf("Description length exceeds the maximum limit.\n");
    } else {
        printf("Description is invalid at position %d.\n", descriptionCheck);
    }

    // Test checkTime function
    int timeCheck = checkTime(raw_time);
    if (timeCheck == -1) {
        printf("Time is valid.\n");
    } else if (timeCheck == 0) {
        printf("Ending time is earlier than the starting time.\n");
    } else {
        printf("Time is invalid for datetime%d.\n", timeCheck);
    }

    return 0;
}