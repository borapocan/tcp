#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define PORT 8080
#define MAX_SIZE 2048

void signal_interrupt_handler(int signal_number);
int connection = 1, client_socket;

int main(){

	int ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		printf("[-] Error in connection.\n");
		exit(1);
	}
	printf("[+] Client Socket is created.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ret = connect(client_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (ret < 0) {
		printf("[-] Error in connection.\n");
		exit(1);
	}
	printf("[+] Connected to Server.\n");
	signal(SIGINT, signal_interrupt_handler);
	while (connection) {
		main_menu:
		do {
			printf("\n\nCLIENT MENU\n");
			printf("Please select one of the following options:\n");
			printf("1 - Sign In\n");
			printf("2 - Sign Up\n");
			printf("3 - Exit Program\n\n");
			printf("\nSelect an option (1-3): ");
			gets(buffer);
		}while ((strcmp(buffer, "1")) && (strcmp(buffer, "2")) && (strcmp(buffer, "3")));
		send(client_socket, buffer, sizeof(buffer), 0);
		switch (buffer[0]) {
		// Sign In
		case '1':
			bzero(buffer, sizeof(buffer));
			printf("\nSIGN IN:\n");
			sign_in:
			printf("\nUsername: ");
			gets(buffer);
			// If code is not 430, if 430 go back to username
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));
			printf("\nPassword: ");
			gets(buffer);
			send(client_socket, buffer, strlen(buffer), 0);
			// If credentials are correct wait for 230
			// If not wait for 431
			bzero(buffer, sizeof(buffer));
			recv(client_socket, buffer, 1024, 0);
			if (strcmp(buffer, "431") == 0) {
				printf("Wrong credentials. Please try again.\n");
				bzero(buffer, sizeof(buffer));
				goto sign_in;
			}
			if (strcmp(buffer, "230") == 0) {
				printf("Credentials are correct. Welcome.\n");
				bzero(buffer, sizeof(buffer));
			}
			/*
				1: View user information
    				2: View all users
    				3: View rooms information
    				4: Booking
    				5: Canceling
    				6: Pass day
    				7: Edit information
    				8: Leaving rooms
    				9: Rooms
    				0: Logout
			*/
			client_menu:
			do {
				printf("\n\nCLIENT MENU\n");
				printf("Please select one of the following options:\n");
				printf("1 - View user information\n");
				printf("2 - View all users\n");
				printf("3 - View rooms information\n");
				printf("4 - Booking\n");
				printf("5 - Canceling\n");
				printf("6 - Pass day\n");
				printf("7 - Edit information\n");
				printf("8 - Leaving rooms\n");
				printf("9 - Rooms\n");
				printf("0 - Logout\n\n");
				printf("\nSelect an option (0-9): ");
				gets(buffer);
			} while ((strcmp(buffer, "1")) && (strcmp(buffer, "2")) && (strcmp(buffer, "3")) \
				 && (strcmp(buffer, "4")) && (strcmp(buffer, "5")) && (strcmp(buffer, "6")) \
				 && (strcmp(buffer, "7")) && (strcmp(buffer, "8")) && (strcmp(buffer, "9")) \
				 && (strcmp(buffer, "0")));
			send(client_socket, buffer, sizeof(buffer), 0);
			switch (buffer[0]) {
			// View User Information
			case '1':
				//TODO
				goto client_menu;
			case '2':
				//TODO
				goto client_menu;
			case '3':
				// TODO
				goto client_menu;
			case '4':
				//TODO
				goto client_menu;
			case '5':
				//TODO
				goto client_menu;
			case '6':
				// TODO
				goto client_menu;
			case '7':
				//TODO
				goto client_menu;
			case '8':
				//TODO
				goto client_menu;
			case '9':
				// TODO
				goto client_menu;
			default:
				goto client_menu;
			}



			goto main_menu;
		// Sign Up
		case '2':
			bzero(buffer, sizeof(buffer));
			sign_up:
			printf("\nSIGN UP:\n");
			printf("\nUsername: ");
			gets(buffer);
			// Check type for code 503
			// Check another user exist with same name
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, strlen(buffer));
			recv(client_socket, buffer, 1024, 0);
			if (strcmp(buffer, "451") == 0) {
				printf("SIGN IN CODE 451: User already exists. Please try again.\n");
				bzero(buffer, sizeof(buffer));
				goto sign_up;
			}
			if (strcmp(buffer, "311") == 0) {
				printf("SIGN IN CODE 311: User name acceptable.\n");
				bzero(buffer, sizeof(buffer));
			}
			printf("\nPassword: ");
			gets(buffer);
			// Check the type for code 503
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, strlen(buffer));
			// Initialize false to admin for new user
			//printf("\nAdmin: ");
			//gets(buffer);
			//// Check the type for code 503
			//send(client_socket, buffer, strlen(buffer), 0);
			//bzero(buffer, strlen(buffer));
			printf("\nPurse: ");
			gets(buffer);
			// Check the type for code 503
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, strlen(buffer));
			printf("\nPhone Number: ");
			gets(buffer);
			// Check the type for code 503
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, strlen(buffer));
			printf("\nAddress: ");
			gets(buffer);
			// Check the type for code 503
			send(client_socket, buffer, strlen(buffer), 0);
			bzero(buffer, strlen(buffer));
			// Wait for confirmation code 231
			recv(client_socket, buffer, 1024, 0);
			if (strcmp(buffer, "231") == 0) {
				printf("User has been successfully created.\n");
				bzero(buffer, sizeof(buffer));
			}
			goto main_menu;
		// Exit Program
		case '3':
			close(client_socket);
			printf("Program terminated.\n");
			printf("[-] Disconnected from server.\n");
			exit(1);
			return 0;
		// Invalid Option
		default:
			printf("Invalid option.\n");
			goto main_menu;
		}
		//printf("Client: \t");
		//scanf("%s", &buffer[0]);
		//send(clientSocket, buffer, strlen(buffer), 0);

		//if (strcmp(buffer, ":exit") == 0){
		//	close(clientSocket);
		//	printf("[-] Disconnected from server.\n");
		//	exit(1);
		//}

		//bzero(buffer, sizeof(buffer));
		//if (recv(clientSocket, buffer, 1024, 0) < 0){
		//	printf("[-] Error in receiving data.\n");
		//} else{
		//	printf("Server: \t%s\n", buffer);
		//}
	}

	return 0;
}

void signal_interrupt_handler(int signal_number)
{
	// Reset handler to catch SIGINT next time.
	signal(SIGINT, signal_interrupt_handler);
	close(client_socket);
	printf("[-] Disconnected from server.\n");
	exit(1);
}
