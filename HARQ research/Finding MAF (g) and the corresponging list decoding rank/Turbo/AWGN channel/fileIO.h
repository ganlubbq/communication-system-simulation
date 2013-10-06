/* This header file provides functions for file input and output operations.
 * Enhance the functionality of getValue().
 * 
 * Date: 2007/11/27
 */

#ifndef FILEIO_H
#define FILEIO_H

// Return the value(int) for the passing item in the configuration file.
double getValue(char *itemName, char *filename, char *option = "dec");

// Print the system time and the passing message to a file
void outputMessage(char *message, char *filename);

#endif
