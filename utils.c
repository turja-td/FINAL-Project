#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "common.h"
#include "utils.h"


// Reads one line of text from the keyboard and removes the trailing newline character that fgets leaves behind. 
void readLine(char *buffer, int size) {
    
    if(fgets(buffer, size, stdin) == NULL){ 
        printf("\nInput ended, closing the program.\n");
        exit(0);
    }

    int len = strlen(buffer);
    
    if (len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
    }

    else{
        // The line was longer than our buffer, throw away the rest 
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
    }
}


int readInt(const char *prompt) {
    char line[50];
    int value;
    int ok;

    while (1) {
        printf("%s", prompt);
        readLine(line, sizeof(line));
        ok = sscanf(line, "%d", &value); // Converts the input string into an int
        if (ok == 1) {
            return value;
        }
        printf("That is not a valid number, try again.\n");
    }
}


int readIntRange(const char *prompt, int low, int high) {
    int value;
    while (1) {
        value = readInt(prompt);
        if (value >= low && value <= high) {
            return value;
        }
        printf("Please enter a number between %d and %d.\n", low, high);
    }
}

void readString(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    readLine(buffer, size);
}

void pressEnterToContinue(void) {
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Stops when Enter is pressed
    }
}


void getToday(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, DATE_LEN, "%Y-%m-%d", t);
}

int isValidDateFormat(const char *date) {
    // must look exactly like YYYY-MM-DD
    if (strlen(date) != 10) {
        return 0;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return 0;
        } else {
            if (date[i] < '0' || date[i] > '9') return 0;
        }
    }
    return 1;
}

int isValidTimeFormat(const char *time) {
    // must look exactly like HH:MM, 5 characters
    if (strlen(time) != 5) {
        return 0;
    }
    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            if (time[i] != ':') return 0;
        } else {
            if (time[i] < '0' || time[i] > '9') return 0;
        }
    }
    return 1;
}

int containsIgnoreCase(const char *text, const char *search) {
    char textCopy[200];
    char searchCopy[200];
    int i;

    // make lowercase copies so the comparison ignores upper/lower case
    for (i = 0; text[i] != '\0' && i < 199; i++) {
        textCopy[i] = tolower((unsigned char) text[i]);
    }
    textCopy[i] = '\0';

    for (i = 0; search[i] != '\0' && i < 199; i++) {
        searchCopy[i] = tolower((unsigned char) search[i]);
    }
    searchCopy[i] = '\0';

    if (strstr(textCopy, searchCopy) != NULL) {
        return 1;
    }
    return 0;
}

int bloodGroupIndex(const char *bg) {
    char groups[8][BLOODGRP_LEN] = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    int i;
    for (i = 0; i < 8; i++) {
        if (strcmp(bg, groups[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int isValidBloodGroup(const char *bg) {
    if (bloodGroupIndex(bg) == -1) {
        return 0;
    }
    return 1;
}

