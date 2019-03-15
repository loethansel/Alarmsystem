/*
 * SocketClient.h  Created on: 2 Jul 2014
 * Copyright (c) 2014 Derek Molloy (www.derekmolloy.ie)
 * Made available for the book "Exploring BeagleBone" 
 * See: www.exploringbeaglebone.com
 * Licensed under the EUPL V.1.1
 *
 * This Software is provided to You under the terms of the European 
 * Union Public License (the "EUPL") version 1.1 as published by the 
 * European Union. Any use of this Software, other than as authorized 
 * under this License is strictly prohibited (to the extent such use 
 * is covered by a right of the copyright holder of this Software).
 * 
 * This Software is provided under the License on an "AS IS" basis and 
 * without warranties of any kind concerning the Software, including 
 * without limitation merchantability, fitness for a particular purpose, 
 * absence of defects or errors, accuracy, and non-infringement of 
 * intellectual property rights other than copyright. This disclaimer 
 * of warranty is an essential part of the License and a condition for 
 * the grant of any rights to this Software.
 * 
 * For more details, see http://www.derekmolloy.ie/
 */

#ifndef SOCKETSERVER_H_
#define SOCKETSERVER_H_

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

class SocketServer
{
private:
    int         serversocketfd;
    int         clientsocketfd;
    int         clientlen;
    struct      sockaddr_un   serverAddress;
    struct      sockaddr_un   clientAddress;
    struct 	    hostent       *server;
    std::string serverName;
    int         portNumber;
    bool        isConnected;
    char ioBuff[32];


public:
	SocketServer(std::string serverName, int portNumber);
	void setClient(std::string serverName, int portNumber);
	virtual int connectToClient();
	virtual int disconnectFromClient();
//	virtual int send(std::string message);
	virtual std::string receive(int size);
	bool isServerConnected() { return this->isConnected; }
	virtual ~SocketServer();
};

#endif /* SOCKETCLIENT_H_ */
