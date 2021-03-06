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

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../logger/logger.h"
#include "SocketServer.h"

using namespace std;
using namespace logger;


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

void SocketServer::setServer(std::string clientName, int portNumber)
{
    this->serverName = serverName;
    this->portNumber = portNumber;
    disconnectFromClient();
    connectToClient();
}

int SocketServer::connectToClient()
{
    unlink(this->serverName.c_str());
    serversocketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serversocketfd < 0){
    	perror("Socket Client: error opening socket.\n");
    	return 1;
    }
    serverAddress.sun_family = AF_UNIX;
    strcpy(serverAddress.sun_path,this->serverName.c_str());

    if(bind(serversocketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        Logger::Write(Logger::ERROR,"Socket Server: error starting the server");
    	perror("Socket Server: error starting the server.\n");
    	return 1;
    }
    listen(serversocketfd, 5);
    clientlen = sizeof(clientAddress);
    cout << "wait for client connection...";
    Logger::Write(Logger::INFO,"wait for client connection...");
    clientsocketfd = accept(serversocketfd, (sockaddr*) &clientAddress, (socklen_t*) &clientlen);
    fcntl(clientsocketfd, F_SETFL, O_NONBLOCK);
    this->isConnected    = true;
    Logger::Write(Logger::INFO,"success -> client is connected");
    cout << "success -> client is connected!" << endl;
    return 0;
}

// muss noch verfeinert werden
int SocketServer::sendout(std::string message)
{
const char *writeBuffer = message.data();
int length = message.length();
    if(!this->isConnected) return -1;

    if(send(clientsocketfd, writeBuffer, length, 0) == -1) disconnectFromClient();
    return 0;
}

// muss noch verfeinert werden
string SocketServer::receive(int size=1024)
{
int erg;

    char readBuffer[size];
    erg = recv(this->clientsocketfd, readBuffer, sizeof(readBuffer), 0);
    if(erg == -1) cout << "not_received_data_from_client" << endl;
    else if(erg == 0) printf("client_shutdowned\n");
    else {
        return string(readBuffer);
    }
    return "";
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



