/*
 * FTPcode.h
 *
 *  Created on: May 6, 2012
 *      Author: randy
 */

#ifndef FTPCODE_H_
#define FTPCODE_H_

/*this header define every response code of FTP protocol*/
#define OPEN_FILE_ERROR_MSG "550 Failed to open file" //when server failed to open a file the client required
#define OPEN_FILE_ERROR_CODE 550
#define SERVICE_READY_FOR_NEW_USER 220
#define CANNOT_OPEN_DATA_CONNECTION_MSG "425 Can't open data connection"
#define DATA_CONNECTION_ERROR_CODE 425
#define QUIT_MSG "Goodbye. Have a good time"

#endif /* FTPCODE_H_ */
