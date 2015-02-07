/* 
 * Support dec, oct, and hex format.
 * 
 * Date: 2007/11/27
 * -----------------------------------------------------------------------------
 * Ignore the whitespaces before and after the specified itemName.
 * Require the extracted string to match the specified itemName exactly.
 * 
 * Date: 2008/7/5
 * ----------------------------------------------------------------------------- 
 * Rewrite getValue()
 * Add getArray()
 * 
 * Date: 2008/8/8
 * ----------------------------------------------------------------------------- 
 * Add getString() for use of modulation format selection
 * Terminate the program upon file read error.
 *
 * Date: 2009/5/20
 */

#ifndef FILEIO_H
#define FILEIO_H

// Return the value associated with the specified "itemName" in the file
// "fileName"
double getValue(char *itemName, char *fileName, char *option = "dec");

// Return an array pointer for the values associated with the specified 
// "itemName" in the file "fileName"
double *getArray(char *itemName, int arraySize, char *fileName);

// Return a char pointer to the string associated with the specified "itemName"
// in the file "fileName"
char *getString(char *itemName, char *fileName);

// Print the system time and the specified message to a file.
// If the specified file does not exist, it will be generated correspondingly.
void outputMessage(char *message, char *fileName);

#endif
