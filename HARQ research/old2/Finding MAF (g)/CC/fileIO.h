#ifndef FILEIO_H
#define FILEIO_H

/*
 * This header file provides functions for file output
 *
 */

// Return the value(int) for the passing item in the configuration file.
float getValue(char *itemName, char *filename);

// Print the system time and the passing message to a file
void outputMessage(char *message, char *filename);

#endif
