#pragma once

void readLine(char *buffer, int size);
int readInt(const char *prompt);
int readIntRange(const char *prompt, int lo, int hi);
void readString(const char *prompt, char *buffer, int size);
void pressEnterToContinue(void);
void getToday(char *buffer); 
int isValidDateFormat(const char *date);
int isValidTimeFormat(const char *time);  
int containsIgnoreCase(const char *text, const char *search);
int isValidBloodGroup(const char *bg);
int bloodGroupIndex(const char *bg);


