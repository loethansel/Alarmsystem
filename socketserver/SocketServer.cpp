/*
 * SocketClient.cpp  Created on: 2 Jul 2014
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

#include "SocketServer.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


SocketServer::SocketServer(std::string serverName, int portNumber)
{
    this->serversocketfd = -1;
    this->clientsocketfd = -1;
    this->clientlen      = 0;
	this->server         = NULL;
	this->serverName     = serverName;
	this->portNumber     = portNumber;
	this->isConnected    = false;
}

void SocketServer::setClient(std::string clientName, int portNumber)
{
    this->serverName = serverName;
    this->portNumber = portNumber;
}

int SocketServer::connectToClient(){

    unlink("/tmp/alarmpipe");
    serversocketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serversocketfd < 0){
    	perror("Socket Client: error opening socket.\n");
    	return 1;
    }
//    server = gethostbyname(clientName.data());
//    if (server == NULL) {
//        perror("Socket Client: error - no such host.\n");
//        return 1;
//    }
    //bzero((char *) &serverAddress, sizeof(serverAddress));


    serverAddress.sun_family = AF_UNIX;


   // bcopy((char *)server->h_addr,(char *)&clientAddress.sin_addr.s_addr, server->h_length);

    //!!
//    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
   // serverAddress.sun_addr.s_addr = "alarmpipe";
    strcpy(serverAddress.sun_path, "/tmp/alarmpipe");
    //portNumber = 8421;
    //!! end


    //clientAddress.sin_port = htons(portNumber);
    if(bind(serversocketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    	perror("Socket Server: error starting the server.\n");
    	return 1;
    }
    this->isConnected = true;

    listen(serversocketfd, 5);

    clientlen = sizeof(clientAddress);
    clientsocketfd = accept(serversocketfd, (sockaddr*) &clientAddress, (socklen_t*) &clientlen);
    fcntl(clientsocketfd, F_SETFL, O_NONBLOCK);
    char chr[5];
    chr[0] = 't';
    chr[1] = 'e';
    chr[2] = 's';
    chr[3] = 't';
    chr[4] = 0;
    int erg;
    send(clientsocketfd, &chr[0], 5, 0);
    erg = recv(clientsocketfd, &chr, 5, 0);

//    int erg;
//    char chr[5];
//    erg = read(clientsocketfd, &chr, 5);

//    erg = recv(clientsocketfd, &chr, 5, 0);
/*
    while(1)
    {
        char ch[8];
        int res;
        res = recv(clientsocketfd, &ch, 5, 0);
        if (res == -1) {
            sleep(1);
            cout << "not_received_data_from_client" << endl;
        }
        else if(res == 0) {
            sleep(1);
            printf("client_shutdowned\n");
        }
        else {
            cout << "received:" << endl;
            send(clientsocketfd, &ch[0], 5, 0);
        }
    }
*/
    return 0;
}
/*
int SocketServer::send(std::string message){
	const char *writeBuffer = message.data();
	int length = message.length();
    int n = write(this->serversocketfd, writeBuffer, length);
    if (n < 0){
       perror("Socket Client: error writing to socket");
       return 1;
    }
    return 0;
}
*/
string SocketServer::receive(int size=1024){
    char readBuffer[size];
    int n = read(this->clientsocketfd, readBuffer, sizeof(readBuffer));
    if (n < 0){
       perror("Socket Client: error reading from socket");
    }
    return string(readBuffer);
}

int SocketServer::disconnectFromClient(){
	this->isConnected = false;
	close(this->serversocketfd);
	return 0;
}

SocketServer::~SocketServer() {
	if (this->isConnected == true){
		disconnectFromClient();
	}
}

