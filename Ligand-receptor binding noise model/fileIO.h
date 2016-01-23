/*
 * This header file provides functions for file input and output operations.
 * Enhance the functionality of getValue().
 * 
 * Date: 2007/11/27
 * -----------------------------------------------------------------------------
 *
 * Change the input data type from char* to const char*
 * Dev cpp 4.9.9.9 requires this (new compiler)
 * Date: 2011/8/22
 * -----------------------------------------------------------------------------
 *
 * Check for file open error
 * Date: 2011/11/16
 * -----------------------------------------------------------------------------
 *
 * Author: Ling-San Meng
 * Email: f95942117@gmail.com
 */

#ifndef FILEIO_H
#define FILEIO_H

// Return the value(int) for the passing item in the configuration file.
double getValue(const char *itemName, const char *filename, const char *option = "dec");

// Print the system time and the passing message to a file
void outputMessage(const char *message, const char *filename);

#endif
