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
#define DIR_CHANGE_OK_MSG "250 Directory successfully changed"
#define FILE_REQUEST_ERROR_MSG "550 File action not taken"
#define ARG_SYNTEX_ERROR_MSG "501 Syntax error in parameters or arguments"
#define ILLEGAL_COMMAND_MSG "500 Illegal command."
#endif /* FTPCODE_H_ */
