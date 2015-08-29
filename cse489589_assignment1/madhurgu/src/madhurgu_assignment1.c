/**
 * @madhurgu_assignment1
 * @author  madhur gupta <madhurgu@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <limits.h>
#include <sys/select.h>
#include <sys/types.h>


/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
#define STDIN 0
#define SIZE 1024
#define maximumPeersAllowed 4



struct Server_IP_List {
	char hostname[50];
	char ipaddress[INET_ADDRSTRLEN];
	int portnumber;
	int connectionID;
	int socketvalue;


};

struct Client_List{
	int id;
	char hostname[50];
	char ipaddress[INET_ADDRSTRLEN];
	int port;
	int socketvalue;
};




char myHostName[50];
char myaddress[50];


int main(int argc, char* argv[]) {
	int result;
	if (argc < 3) {
		printf("In-sufficient Parameters. Please refer to command. \n");
		printf("Usage : ./assignement1 s <PORT_NUMBER>\n");
		printf("            OR          \n");
		printf("Usage : ./assignement1 c <PORT_NUMBER>\n");
		return 0;
	}
	if (strcmp(argv[1], "s") == 0 && atoi(argv[2])) {
		printf("Starting  the Program as server on Port %s \n", argv[2]);
		result = runServerCommands(argv[2]);
	} else if ((strcmp(argv[1], "c") == 0) && atoi(argv[2])) {
		printf("Starting the program as client on Port %s \n", argv[2]);
		result = runClientCommands(argv[2]);
	} else {
		printf("Invalid Command : See the Usage\n.");
		printf("Usage : ./assignement1 s <PORT_NUMBER>\n");
		printf("            OR          \n");
		printf("Usage : ./assignement1 c <PORT_NUMBER>\n");
		return 0;
	}

	return 0;
}

int runServerCommands(char* serverPort) {
	struct Server_IP_List serveriplist[5];
	char command[SIZE];
	char *p;
	char *argv[6];
	struct addrinfo hints, *res, cli_addr;
	struct hostent *he;
	struct in_addr ipv4addr;
	socklen_t clilen;
	int s = 0;
	int sock_fd;
    int newsockfd;
	int selret;
	int j,k;
	int sock_index;
	int numberOfBytes;
	char buff[SIZE];
	char buff1[SIZE];
	char clientData[SIZE];
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET_ADDRSTRLEN];
	int port;
	char remoteHostName[50];
    char remoteIP[50];
	int clientCount = 0;
	/*master file descriptor list */

	fd_set master_list;

	/* temp file descriptor list for select() */

	fd_set watch_list;

	/* maximum file descriptor number */
	int head_socket;

	bzero(buff1,sizeof(buff1));
	bzero(&serveriplist,sizeof(serveriplist));
	bzero(buff,1023);
	bzero(buff1,1023);
	bzero(clientData,1023);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	// This part of the code has been taken from beej guide.
	s = getaddrinfo(NULL, serverPort, &hints, &res);
	if (s != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		perror("Error in socket creation");
		return -1;
	}


	if (bind(sock_fd, res->ai_addr, res->ai_addrlen) < 0)
	{
		perror("Error in bind");
		return -1;
	}
	if (listen(sock_fd, 4) < 0)
	{
		perror("Error in listen");
		return -1;
	}

	/* add the sock_fd to the master set */

	/* keep track of the highest file descriptor */

	head_socket = sock_fd; /* so far, it's this one*/

	FD_ZERO(&master_list);
	FD_SET(sock_fd, &master_list);
	FD_SET(STDIN, &master_list);

    // logic of this part has been taken from recitation slides.
	while (1)
	{

		FD_ZERO(&watch_list);

		watch_list = master_list;
		printf("assignment_bash$");
		fflush(stdout);
		selret = select(head_socket + 1, &watch_list, NULL, NULL, 0);
		if (selret < 0)
		{
			perror("select failed");
			return -1;
		}
 		if (selret > 0)
		{
			for (sock_index = 0; sock_index <= head_socket; sock_index++)
			{
				if (FD_ISSET(sock_index, &watch_list))
				{
					if (sock_index == STDIN)
					{
						fgets(command, 256, stdin);
						if ((p = strchr(command, '\n')) != NULL)
							*p = '\0';

						//Tokenize the command
						char *pch;

						int argcount = 0;
						pch = strtok(command, " ");
						while (pch != NULL)
						{
							argv[argcount] = malloc(strlen(pch));
							strcpy(argv[argcount], pch);
							argcount = argcount + 1;
							pch = strtok(NULL, " ");
						}

						if ((strcasecmp(argv[0], "creator") == 0) && (argcount==1))
						{
							printf("Name: Madhur Gupta \n");
							printf("UBIT Name: MADHURGU \n");
							printf("UB-email: madhurgu@buffalo.edu \n");
							printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity \n");

						}
						else if ((strcasecmp(argv[0], "help") == 0) && (argcount==1))
						{
							printf("Available commands For Server are (commands are case-insensitive) : \n");
							printf("\t creator\t-- displays information about the author of the program and department integrity policy\n");
							printf("\t myip\t-- displays the IP address of the machine on which Program is running\n");
							printf("\t myport\t-- displays the port on which the this program is listening for connections\n");
							printf("\t exit\t -- closes all connection and terminates this process\n");
						}
						else if ((strcasecmp(argv[0], "myport") == 0) && (argcount==1))
						{
							printf("Port number:%s\n", serverPort);
						}
						else if ((strcasecmp(argv[0], "myip") == 0) && (argcount==1))
						{
							int result = myIP();
						}
						else if ((strcasecmp(argv[0], "exit") == 0) && (argcount==1))
						{
						      for(k=0;k<=head_socket;k++)
						      {
						          if(FD_ISSET(k,&watch_list))
						             close(k);
						      }
						      FD_ZERO(&watch_list);
						      FD_ZERO(&master_list);
						      exit(0);
						}
						else
						{
							printf("Invalid Command. Please check the usage by typing help\n");
							printf("I have not implemented 'UPLOAD' 'DOWNLOAD' and 'STATISTICS' commands of this assignment\n");
						}


					}
					else if (sock_index == sock_fd)
					{
						newsockfd = accept(sock_fd,(struct sockaddr *) &cli_addr, &clilen);
						if (newsockfd == -1)
						{
							perror("Error on Socket Accept\n");
							return -1;
						}

						// add the new socket fd to master list
						FD_SET(newsockfd, &master_list);
						if (newsockfd > head_socket)
						{
							head_socket = newsockfd;
						}


						len = sizeof addr;
						getpeername(newsockfd, (struct sockaddr*) &addr, &len);

						if (addr.ss_family == AF_INET)
						{
							struct sockaddr_in *s = (struct sockaddr_in *) &addr;
							port = ntohs(s->sin_port);
							inet_ntop(AF_INET, &s->sin_addr, ipstr,
									sizeof ipstr);
						}
                        strcpy(remoteIP,ipstr);

				        inet_pton(AF_INET, ipstr, &ipv4addr);
						he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
						strcpy(remoteHostName,he->h_name);
						printf("Remote client %s is connected to this Server\n",remoteHostName);

					}
					else
					{
						// receive  newly joined client's port
						//if number of bytes received are zero, this means this newest client has exited.
						// otherwise it is just sending the data to the sever.
						if(sock_index==newsockfd)
						{
						   if ((numberOfBytes = recv(newsockfd,buff, sizeof(buff),0)) <= 0)
						   {
							   if (numberOfBytes == 0)
							   {
								   bzero(clientData,1023);
								   bzero(buff1,1023);
								   for (j=0;j<clientCount;j++)
								   {
										 if(serveriplist[j].socketvalue == sock_index)
										 {
											 printf("Client %s has  exited.\n",serveriplist[j].hostname);
											 FD_CLR(sock_index,&master_list);
											 for (k=j;k<clientCount;k++)
											 {
												 serveriplist[k].connectionID=serveriplist[k+1].connectionID;
												 strcpy(serveriplist[k].hostname,serveriplist[k+1].hostname);
												 strcpy(serveriplist[k].ipaddress,serveriplist[k+1].ipaddress);
												 serveriplist[k].portnumber=serveriplist[k+1].portnumber;
												 serveriplist[k].socketvalue=serveriplist[k+1].socketvalue;

											 }
											 clientCount=clientCount-1;
											 break;
										 }
									 }
								     //create a buffer to send the registered clients list.
								     // Each client's details are separated by ":"
								     // One client is separated from the other  by "#".
									 for(k=0;k<clientCount;k++)
									 {
										 snprintf(clientData,sizeof(clientData),"%d%s%s%s%s%s%d%s",serveriplist[k].connectionID,":",serveriplist[k].hostname,":",serveriplist[k].ipaddress,":",serveriplist[k].portnumber,"#");
										 strcat(buff1,clientData);
									 }
									 bzero(clientData,1023);
									 for(j=0;j<=head_socket;j++)
									 {
										 if(FD_ISSET(j,&master_list) && j != sock_fd && j!=STDIN)
										 {
											if(send(j,buff1,strlen(buff1),0)==-1)
											   perror("error on sending Server IP List\n");
										 }

									 }
									 bzero(buff1,1023);
									 // Print server  IP List.
									 printf("!!!! Server IP List !!!!\n");
									 printf("ID\tHost Name\t\t\tIP Address\t Port Number\n");
									 for(j=0;j<clientCount;j++)
									 {
							             printf("%-5d%-35s%-20s%-8d\n",serveriplist[j].connectionID,serveriplist[j].hostname,serveriplist[j].ipaddress,serveriplist[j].portnumber);
									 }

							   }
							   else
								   perror("error on receive\n");

							     //close(sock_index);
							    FD_CLR(newsockfd, &master_list);

						   }
						   else
						   {
							    int clientPort=atoi(buff);
						     	bzero(buff,sizeof(buff));
						    	//Make a new entry for newly registered client.
                                serveriplist[clientCount].connectionID=newsockfd;
							    strcpy(serveriplist[clientCount].hostname,remoteHostName);
                                strcpy(serveriplist[clientCount].ipaddress,remoteIP);
							    serveriplist[clientCount].portnumber=clientPort;
							    serveriplist[clientCount].socketvalue=newsockfd;
							    clientCount=clientCount+1;
							    // create a Server IP List of all registered clients and send it to each client.
							    // Info related to each client is separated by ":" and one client's info is separated from other by #.
							    for(k=0;k<clientCount;k++)
							    {
							 	    snprintf(clientData,sizeof(clientData),"%d%s%s%s%s%s%d%s",serveriplist[k].connectionID,":",serveriplist[k].hostname,":",serveriplist[k].ipaddress,":",serveriplist[k].portnumber,"#");
							        strcat(buff1,clientData);
							    }
                                bzero(clientData,1023);
							    for(j=0;j<=head_socket;j++)
							    {
								    if(FD_ISSET(j,&master_list) && j != sock_fd && j!=STDIN)
								    {
							           if(send(j,buff1,strlen(buff1),0)==-1)
										        perror("error on sending Server IP List\n");
								    }

							    }
							    //print the Server IP List
							    bzero(buff1,1023);
                                printf("!!!! Server IP List !!!!\n");
                                printf("ID\tHost Name\t\t\tIP Address\t Port Number\n");
							    for(j=0;j<clientCount;j++)
							    {
								    printf("%-5d%-35s%-20s%-8d\n",serveriplist[j].connectionID,serveriplist[j].hostname,serveriplist[j].ipaddress,serveriplist[j].portnumber);
							    }
						   }
						}
						else
						{
							if ((numberOfBytes = recv(sock_index,buff, sizeof(buff),0)) <= 0)
						    {
								 if (numberOfBytes == 0)
								 {
									 // no. of bytes zero means that client has "exit"ed the connection with the server.
									 // So we need to update the Central Server IP List.
									 //And send this list to each registered client.
									 bzero(clientData,1023);
									 bzero(buff1,1023);
									 for (j=0;j<clientCount;j++)
									 {
										 if(serveriplist[j].socketvalue == sock_index)
										 {
											 printf("Client %s has  exited.\n",serveriplist[j].hostname);
											 FD_CLR(sock_index,&master_list);
											 for (k=j;k<clientCount;k++)
											 {
												 serveriplist[k].connectionID=serveriplist[k+1].connectionID;
												 strcpy(serveriplist[k].hostname,serveriplist[k+1].hostname);
												 strcpy(serveriplist[k].ipaddress,serveriplist[k+1].ipaddress);
												 serveriplist[k].portnumber=serveriplist[k+1].portnumber;
												 serveriplist[k].socketvalue=serveriplist[k+1].socketvalue;

											 }
											 clientCount=clientCount-1;
											 break;
										 }
									 }
									 for(k=0;k<clientCount;k++)
									 {
										 snprintf(clientData,sizeof(clientData),"%d%s%s%s%s%s%d%s",serveriplist[k].connectionID,":",serveriplist[k].hostname,":",serveriplist[k].ipaddress,":",serveriplist[k].portnumber,"#");
										 strcat(buff1,clientData);
									 }
									 bzero(clientData,1023);
									 for(j=0;j<=head_socket;j++)
									 {
										 // Send the Server IP List to each registered client.
										 if(FD_ISSET(j,&master_list) && j != sock_fd && j!=STDIN)
										 {
											if(send(j,buff1,strlen(buff1),0)==-1)
											   perror("error on sending Server IP List\n");
										 }

									 }
									 //print the server ip list
									 bzero(buff1,1023);
									 printf("!!!! Server IP List !!!!\n");
									 printf("ID\tHost Name\t\t\tIP Address\t Port Number\n");
									 for(j=0;j<clientCount;j++)
									 {
							             printf("%-5d%-35s%-20s%-8d\n",serveriplist[j].connectionID,serveriplist[j].hostname,serveriplist[j].ipaddress,serveriplist[j].portnumber);
									 }
								 }
								 else
									 perror("error on receive\n");

								 //close(sock_index);
								 FD_CLR(sock_index, &master_list);

							}

						}
					}
				}
			}
		}
	}





	return 0;
}
int runClientCommands(char* clientPort) {
	    struct Server_IP_List serveriplist[5];
	    struct Client_List clientlist[4],tempClient;
	    char command[1024];
		char *p;
		char *argv[6],*argv1[5],*argv2[4];
		struct addrinfo hints,*servinfo, *res, cli_addr;
		struct hostent *he;
		struct in_addr ipv4addr;
		socklen_t clilen;
		int s = 0;
		int socket_fd,socket_fd1,socket_fd2[3];
		int head_socket;
		int newsockfd;
		int selret;
		int i,j,k;
		int sock_index;
		int numberOfBytes;
		int connectedClientCount=0;
		char buff[SIZE];
		char remoteHostName[256];
		char remoteIP[256];
		int serverIPListSize=0;
		socklen_t len;
		struct sockaddr_storage addr;
		char ipstr[INET_ADDRSTRLEN];
		int port;
		int result;

		/*master file descriptor list */
		fd_set master_list;

		/* temp file descriptor list for select() */

		fd_set watch_list;
        bzero(&serveriplist,sizeof(serveriplist));
        bzero(&clientlist,sizeof(clientlist));
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

		//this piece of code is taken from beej guide.
		s = getaddrinfo(NULL, clientPort, &hints, &res);
		if (s != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
			return -1;
		}
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0) {
			perror("Error in socket creation\n");
			return -1;

		}


		if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0)
		{
			perror("bind");
			return -1;
		}
		if (listen(socket_fd, 4) < 0)
		{
			perror("listen");
			return -1;
		}
		//get the host name and IP for the machine. This will be used for checking self connection
		result=getLocalHostNameandIP();
		if (result<0)
		{
			printf("Error in getting the host name and IP\n.");
			exit(0);
		}
		/* add the sock_fd to the master set */
		/* keep track of the highest file descriptor */

		head_socket = socket_fd; /* so far, it's this one*/

		// this piece of code is taken from recitation slides.
		FD_ZERO(&master_list);
		FD_SET(socket_fd, &master_list);
		FD_SET(STDIN, &master_list);
        // this piece of code is taken from recitation slides.
		while(1)
		{
			FD_ZERO(&watch_list);

					watch_list = master_list;
					printf("assignment_bash$");
					fflush(stdout);
					selret = select(head_socket + 1, &watch_list, NULL, NULL, 0);

					if (selret < 0)
					{
						perror("select failed\n");
						return -1;
					}
					if (selret > 0)
					{
						for (sock_index = 0; sock_index <= head_socket; sock_index++)
						{
							if (FD_ISSET(sock_index, &watch_list))
							{
								if (sock_index == STDIN)
								{
									fgets(command, 256, stdin);
									if ((p = strchr(command, '\n')) != NULL)
										*p = '\0';

									//Tokenize the command
									char *pch;

									int argcount = 0;
									pch = strtok(command, " ");
									while (pch != NULL)
									{
										argv[argcount] = malloc(strlen(pch));
										strcpy(argv[argcount], pch);

										argcount = argcount + 1;
										pch = strtok(NULL, " ");
									}

									if ((strcasecmp(argv[0], "creator") == 0) && (argcount==1))
									{
										printf("Name: Madhur Gupta \n");
										printf("UBIT Name: MADHURGU \n");
										printf("UB-email: madhurgu@buffalo.edu \n");
										printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity \n");

									}
									else if ((strcasecmp(argv[0], "help") == 0) && (argcount==1))
									{
										printf("Available commands For clients are (commands are case-insensitive) : \n");
										printf("\t creator\t-- displays information about the author of the program and department integrity policy\n");
										printf("\t myip\t-- displays the IP address of the machine on which Program is running\n");
										printf("\t myport\t-- displays the port on which the this program is listening for connections\n");
										printf("\t register <server IP> <port>\t -- registers this client with <server IP> which is listening on <port>.\n");
										printf("\t connect <destination> <port>\t -- connects this client to a <destination> which is listening on <port>.\n");
										printf("\t list\t -- lists all connection this process is part of\n");
										printf("\t terminate <ID>\t -- terminate the connection listed under the specified number when LIST is used to display all connections\n");
										printf("\t exit\t -- closes all connection and terminates this process\n");
									}
									else if ((strcasecmp(argv[0], "myport") == 0) && (argcount==1))
									{
										printf("Port number:%s\n", clientPort);
									}
									else if ((strcasecmp(argv[0], "myip") == 0) && (argcount==1))
									{
										int result = myIP();
									}
									else if ((strcasecmp(argv[0], "register") == 0) && (argcount==3) && atoi(argv[2]))
									{
												if ((s = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
												{
													perror("Error in getaddressinfo\n");
													return -1;
												}

												socket_fd1 = socket(AF_INET, SOCK_STREAM, 0);
												if (socket_fd1 < 0) {
													perror("socket");
													return -1;

												}

												if (connect(socket_fd1, servinfo->ai_addr, servinfo->ai_addrlen)
														== -1) {
													close(socket_fd1);
													perror("client: connect\n");
													return -1;
												}
												FD_SET(socket_fd1, &master_list);
											    if (socket_fd1 > head_socket)
												{
												   head_socket = socket_fd1;
												}
											    //fill the first entry as server in the client's connected peer list
											    inet_pton(AF_INET, argv[1], &ipv4addr);
											    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
											    strcpy(remoteHostName,he->h_name);
											    printf("This client is registered with server %s\n",remoteHostName);
											    clientlist[0].id=1;
											    strcpy(clientlist[0].hostname,remoteHostName);
											    strcpy(clientlist[0].ipaddress,argv[1]);
											    clientlist[0].port=atoi(argv[2]);
											    clientlist[0].socketvalue=socket_fd1;
											    connectedClientCount++;

												//send the port number to server
												int n = send(socket_fd1,clientPort , strlen(clientPort),0);
									            if (n < 0)
													perror("ERROR writing to socket\n");

									   }
									   else if ((strcasecmp(argv[0], "connect") == 0) && (argcount==3) && (atoi(argv[2])))
									   {
										   int clientExists=0;
										   if(connectedClientCount >= maximumPeersAllowed)
										   {
											   printf(" You are already connected to maximum number (3) of peers. You are not allowed to connect more than 3 peers\n");
										   }
										   else if ((strcasecmp(argv[1],myHostName)==0) || (strcasecmp(argv[1],myaddress)==0))
										   {
											   printf(" Self connections are not allowed\n");
										   }
										   else
										   {
                                               for(j=0;j<serverIPListSize;j++)
                                               {
                                        	       if(((strcasecmp(argv[1],serveriplist[j].hostname)==0) || (strcasecmp(argv[1],serveriplist[j].ipaddress)==0)) && (atoi(argv[2])==serveriplist[j].portnumber) )
                                        	       {
                                        		        strcpy(remoteHostName,serveriplist[j].hostname);
                                        		        strcpy(remoteIP,serveriplist[j].ipaddress);
                                        		        clientExists=1;
                                        		        break;
                                        	       }
                                                }
                                                if(clientExists==0)
                                                {
                                        	        printf("Either Client Doesn't exist in Server IP List or  entered data is incorrect \n");
                                                }
                                                else
                                                {
                                                	int alreadyConnected=0;
                                                	// check if you are already connected to the client.
                                                	for (j=0;j<connectedClientCount;j++)
                                                	{
                             		                     if ((strcasecmp(argv[1],clientlist[j].hostname)==0) || (strcasecmp(argv[1],clientlist[j].ipaddress)==0))
                                                		{
                             		                    	alreadyConnected=1;
                             		                    	break;
                                                		}
                                                	}
                                                	if(alreadyConnected==1)
                                                	{
                                                		printf(" Client is already connected to the client %s\n",argv[1]);
                                                	}
                                                	else
                                                	{

										                if ((s = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
										                {
										   	                 perror("Error in getaddressinfo\n");
										   		             return -1;
										                }

                                                        socket_fd2[connectedClientCount-1]= socket(AF_INET, SOCK_STREAM, 0);
                                                        if (socket_fd1 < 0)
                                                        {
                                                       	    perror("Error in socket creation\n");
                                               	            return -1;

                                                        }
                                                        if (connect(socket_fd2[connectedClientCount-1], servinfo->ai_addr, servinfo->ai_addrlen)== -1)
                                                        {
                                                    	    close(socket_fd2[connectedClientCount-1]);
                                               	            perror("Could not connect to the peer client\n");
                                               	            return -1;
                                                         }
                                                         //update the master list
                                                         FD_SET(socket_fd2[connectedClientCount-1], &master_list);
                                                        //update the head count
                                                        if (socket_fd2[connectedClientCount-1] > head_socket)
                                                        {
                                               	           head_socket = socket_fd2[connectedClientCount-1];
                                                        }
                                                        printf("Remote Host Name: %s is connected to this client\n",remoteHostName);
                                                        //update the client's connected peer list.
                                                        clientlist[connectedClientCount].id=socket_fd2[connectedClientCount-1];
                                                        strcpy(clientlist[connectedClientCount].hostname,remoteHostName);
                                                        strcpy(clientlist[connectedClientCount].ipaddress,remoteIP);
                                                        clientlist[connectedClientCount].port=atoi(argv[2]);
                                                        clientlist[connectedClientCount].socketvalue=socket_fd2[connectedClientCount-1];
                                                        connectedClientCount++;
                                                	}
                                                }
										   }
									   }
									   else if ((strcasecmp(argv[0], "list") == 0) && (argcount==1))
									   {
										      printf("ID\tHost Name\t\t\tIP Address\t Port Number\n");
										      for (j=0;j<connectedClientCount;j++)
										      {
										    	  printf("%-5d%-35s%-20s%-8d\n",clientlist[j].id,clientlist[j].hostname,clientlist[j].ipaddress,clientlist[j].port);
										      }
									   }
									   else if ((strcasecmp(argv[0], "terminate") == 0) && (argcount==2) && (atoi(argv[1])))
									   {
										   int clientExists=0;
										   for (j=0;j<connectedClientCount;j++)
										   {
											   // check if ID is valid
											   if (clientlist[j].id==atoi(argv[1]))
											   {
												          //update the client's list.
												          FD_CLR(clientlist[j].socketvalue,&master_list);
												          close(clientlist[j].socketvalue);
												          for(k=j;k<connectedClientCount;k++)
												          {
												        	  clientlist[k].id=clientlist[k+1].id;
												        	  strcpy(clientlist[k].hostname,clientlist[k+1].hostname);
												        	  strcpy(clientlist[k].ipaddress,clientlist[k+1].ipaddress);
												        	  clientlist[k].port=clientlist[k+1].port;
												        	  clientlist[k].socketvalue=clientlist[k+1].socketvalue;
												          }
												           clientExists=1;
												           break;

											    }

										    }
										    if(clientExists)
										    {
										       printf("Client %s has been removed from this client's peer list\n",clientlist[j].hostname);
											   connectedClientCount--;
										    }
										    else
											   printf("Client with the ID %d doesn't exist in this client's peer list\n",atoi(argv[1]));


									   }
									   else if ((strcasecmp(argv[0], "exit") == 0) && (argcount==1))
									   {
                                               for(k=0;k<=head_socket;k++)
                                               {
                                            	   if(FD_ISSET(k,&watch_list))
                                            	      close(k);
                                               }
                                               FD_ZERO(&watch_list);
                                               FD_ZERO(&master_list);
                                               exit(0);
									   }
									   else
									   {
										   printf("Invalid Command. Please check the usage by typing help\n");
										   printf("I have not implemented 'UPLOAD' 'DOWNLOAD' and 'STATISTICS' commands of this assignment\n");
									   }

								}
								else if (sock_index == socket_fd)
								{

									  newsockfd = accept(socket_fd,(struct sockaddr *) &cli_addr, &clilen);
                                      if (newsockfd == -1)
									  {
									      perror("Error on Socket Accept\n");
									      return -1;
									  }

									  // add the new socket fd to master list
									  FD_SET(newsockfd, &master_list);
									  if (newsockfd > head_socket)
									  {
										    head_socket = newsockfd;
									   }
                                       len = sizeof addr;
									   getpeername(newsockfd, (struct sockaddr*) &addr, &len);
									   if (addr.ss_family == AF_INET)
								       {
										   struct sockaddr_in *s = (struct sockaddr_in *) &addr;
										   port = ntohs(s->sin_port);
										   inet_ntop(AF_INET, &s->sin_addr, ipstr,
												sizeof ipstr);
									    }
									    inet_pton(AF_INET, ipstr, &ipv4addr);
									    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
									    strcpy(remoteHostName,he->h_name);
									    strcpy(remoteIP,ipstr);
									    printf("Remote client %s is connected to this client\n",remoteHostName);
									    //update the client's connected peer list.
								        // fill the remote client data using server IP list.
									    // We need not to lookup using the socket API as this data is already available in server ip list.
									    for(i=0;i<serverIPListSize;i++)
									    {
										    if(strcmp(serveriplist[i].ipaddress,remoteIP)==0)
										    {
											   clientlist[connectedClientCount].id=newsockfd;
											   strcpy(clientlist[connectedClientCount].hostname,serveriplist[i].hostname);
											   strcpy(clientlist[connectedClientCount].ipaddress,serveriplist[i].ipaddress);
											   clientlist[connectedClientCount].port=serveriplist[i].portnumber;
											   clientlist[connectedClientCount].socketvalue=newsockfd;
											   connectedClientCount++;
											   break;
										    }
									    }


								}
								else
								{
									if (sock_index == socket_fd1) // for receiving the Server IP List.
									{
									    bzero(buff,1023);
									    if ((numberOfBytes = recv(socket_fd1,buff, sizeof(buff),0)) <= 0)
									    {
										     if (numberOfBytes == 0)
											     printf("connection is closed\n");
										     else
											     perror("error on receive\n");

			                                  printf("%s",buff);
										    //close(sock_index);
										   FD_CLR(sock_index, &master_list);

									     }
									     else
									     {
									    	 // parse the Server IP List that we got from the server and
									    	 // Update this client's local copy of Server IP List.
                                              bzero(&serveriplist,sizeof(serveriplist));
										      int count = 0;
										      char *pch1;
										      pch1 = strtok(buff, "#");
										      while (pch1 != NULL)
									          {
										             argv1[count] = malloc(strlen(pch1));
											         strcpy(argv1[count], pch1);
											         count = count + 1;
										             pch1 = strtok(NULL, "#");
									          }
                                              // fill the server ip list in client
									          for(i=0;i<count;i++)
									          {
									    	      int count1= 0;
									    	      char *pch2;
									    	      pch2 = strtok(argv1[i], ":");
									    	      while (pch2 != NULL)
									    	      {
                                                         argv2[count1] = malloc(strlen(pch2));
									    		         strcpy(argv2[count1], pch2);
									    		         count1 = count1 + 1;
									    		         pch2 = strtok(NULL, ":");
									    	       }
									    	       serveriplist[i].connectionID=atoi(argv2[0]);
                                                   strcpy(serveriplist[i].hostname,argv2[1]);
									    	       strcpy(serveriplist[i].ipaddress,argv2[2]);
									    	       serveriplist[i].portnumber=atoi(argv2[3]);

									            }
									            serverIPListSize=0;
									            printf("!!!! Server IP List !!!!\n");
									            printf("ID\tHost Name\t\t\tIP Address\t Port Number\n");
									            for(i=0;i<count;i++)
									            {
									             	serverIPListSize=serverIPListSize+1;
									                printf("%-5d%-35s%-20s%-8d\n",serveriplist[i].connectionID,serveriplist[i].hostname,serveriplist[i].ipaddress,serveriplist[i].portnumber);
									             }
									         }

								        }
									    else
									    {
									    	  bzero(buff,1023);
									    	  if ((numberOfBytes = recv(sock_index,buff, sizeof(buff),0)) <= 0)
									    	  {
									    		  // if no of bytes are zero , that means a client has terminated the connection.
									    		  // so we need to update it's list.
									    		   if (numberOfBytes == 0)
									    		   {

									    			   for (j=0;j<connectedClientCount;j++)
									    			   {
									    			   	    if ( clientlist[j].socketvalue == sock_index )
									    			   		{
									    			   	    	 printf("Connection has been closed by client %s\n",clientlist[j].hostname);
									    			   			 FD_CLR(clientlist[j].socketvalue,&master_list);
									    			   			 close(clientlist[j].socketvalue);
									    			   			 for( k=j; k<connectedClientCount; k++)
									    			   			 {
									    			   				 clientlist[k].id=clientlist[k+1].id;
									    			   				 strcpy(clientlist[k].hostname,clientlist[k+1].hostname);
									    			   				 strcpy(clientlist[k].ipaddress,clientlist[k+1].ipaddress);
									    			   				 clientlist[k].port=clientlist[k+1].port;
									    			   				 clientlist[k].socketvalue=clientlist[k+1].socketvalue;
									    			   			  }
									    			   			   connectedClientCount=connectedClientCount-1;
									    			   			   break;

									    			   		 }
									    			   }
									    		 }
									    		 else
									    		   {
									    			   perror("error on receive\n");
									    		   }
                                                   //close(sock_index);
									               FD_CLR(sock_index, &master_list);


									    	  }
									    }

							      }
							}
						}
					}
		}
	return 0;
}

/*
 * This method prints the IP address of the machine.
 * This method is called when user types in "myip" command.
 */
int myIP()
{
	struct addrinfo hints;
	struct addrinfo *result;
	int s = 0;
	char *google = "8.8.8.8";
	char *googleport = "53";


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	s = getaddrinfo(google, googleport, &hints, &result);
	if (s < 0) {
		perror("getaddrinfo");
		return -1;
	}
	int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (connect(socket_fd, result->ai_addr, result->ai_addrlen) < 0) {
		perror("connect");
		return -1;
	}

	struct sockaddr_in local_addr;

	socklen_t addr_len = sizeof(local_addr);
	if (getsockname(socket_fd, (struct sockaddr*) &local_addr, &addr_len) < 0) {
		perror("getsockname");
		close(socket_fd);
		return -1;
	}

	char myip[INET_ADDRSTRLEN];
	if (inet_ntop(local_addr.sin_family, &(local_addr.sin_addr), myip,sizeof(myip))== NULL) {
		perror("inet_ntop");
		return -1;
	} else {
		printf("IP address:%s\n", myip);
		close(socket_fd);
		return 0;
	}

}

/*
 This method prints at beginning the Host name and IP address  of the machine  on which it running.
 This helps is determining on which host this program is running on.
 This also helps in checking for self connection for connect command of the assignment..
 */
int getLocalHostNameandIP()
{
	struct addrinfo hints;
		struct addrinfo *result;
		int s = 0;
		char *google = "8.8.8.8";
		char *googleport = "53";
		struct hostent *he;

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		s = getaddrinfo(google, googleport, &hints, &result);
		if (s < 0) {
			perror("getaddrinfo");
			return -1;
		}
		int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (connect(socket_fd, result->ai_addr, result->ai_addrlen) < 0) {
			perror("connect");
			return -1;
		}

		struct sockaddr_in local_addr;

		socklen_t addr_len = sizeof(local_addr);
		if (getsockname(socket_fd, (struct sockaddr*) &local_addr, &addr_len) < 0) {
			perror("getsockname");
			close(socket_fd);
			return -1;
		}

		char myip[INET_ADDRSTRLEN];
		if (inet_ntop(local_addr.sin_family, &(local_addr.sin_addr), myip,sizeof(myip)) == NULL) {
			perror("inet_ntop");
			return -1;
		}
		else
		{
			close(socket_fd);
			inet_pton(AF_INET, myip, &local_addr);
		    he = gethostbyaddr(&local_addr, sizeof local_addr, AF_INET);
		    strcpy(myHostName,he->h_name);
		    strcpy(myaddress,myip);
		    printf("!!!! Host Info !!!!\n");
		    printf("Host Name = %s Host IP Address = %s\n",myHostName,myaddress);
			return 0;
		}

	}


