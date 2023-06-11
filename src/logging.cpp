#include <logging.h>
#include <filesystem.h>

/* Prints error message to the serial port. 
*  -@param msg: string describing error message
*  -@param error: error number (integer)
*/
void serialError(const char * msg, int error ) {
    Serial.printf( "ERROR [%d]: %s \n", error, msg );
}