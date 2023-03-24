#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "yyjson.c"
#include "yyjson.h"
#include "log.c"
#include "log.h"

#define PORT 8080
#define TRUE 1
#define FALSE 0
#define CHECK_JSON_STR(s) s ? yyjson_get_str(s) : "\0"

typedef struct UserInfo UserInfo;
struct UserInfo {
	int id;
	char *user;
	char *password;
	char *admin;
	char *purse;
	char *phone_number;
	char *address;
	UserInfo *next;
};

UserInfo *create_userinfo_node(yyjson_val *val);
void free_userinfo_node(UserInfo *user);
void free_userinfo_list(UserInfo *head);
void delete_userinfo_node(UserInfo **head, int id);
void append_userinfo_node(UserInfo *head, yyjson_val *val);
void print_userinfo_list(UserInfo *head);
int check_userinfo_user(UserInfo *head, char *user);
int check_userinfo_password(UserInfo *head, char *user, char *password);
int last_userinfo_id(UserInfo *head);

int main() {
	int sockfd, newSocket, ret;
	struct sockaddr_in serverAddr, newAddr;
	struct UserInfo new_user;
	char buffer[1024];
	socklen_t addr_size;
	pid_t childpid;
	//Read JSON file, allowing comments and trailing commas
	yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
	yyjson_read_err err;
	yyjson_doc *doc = yyjson_read_file("./Config/UsersInfo.json", flg, NULL, &err);
	yyjson_val *obj = yyjson_doc_get_root(doc);
	UserInfo *head = NULL, *list_iter = NULL;
	// Iterate over the root object
	yyjson_val *arr = yyjson_obj_get(obj, "users");
	yyjson_arr_iter iter;
	yyjson_arr_iter_init(arr, &iter);
	yyjson_val *val;
	if (doc) {
		while ((val = yyjson_arr_iter_next(&iter))) {
			if (!head) {
				head = create_userinfo_node(val);
				list_iter = head;
			} else {
				head->next = create_userinfo_node(val);
				head = head->next;
			}
		}
		head = list_iter;
	} else {
	    printf("read error (%u): %s at position: %ld\n", err.code, err.msg, err.pos);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		printf("[-] Error in connection.\n");
		exit(1);
	}
	printf("[+] Server Socket is created.\n");
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0) {
		printf("[-] Error in binding.\n");
		exit(1);
	}
	printf("[+] Bind to port %d\n", 8080);
	if (listen(sockfd, 10) == 0) {
		printf("[+] Listening....\n");
	} else {
		printf("[-] Error in binding.\n");
	}
	while (TRUE) {
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if (newSocket < 0){
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		if ((childpid = fork()) == 0) {
			close(sockfd);
			while (TRUE) {
				recv(newSocket, buffer, 1024, 0);
				if (strcmp(buffer, ":exit") == 0) {
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				} else {
					printf("Client %d: Option %s\n", newSocket, buffer);
					if (strcmp(buffer, "1") == 0) {
						bzero(buffer, sizeof(buffer));
						sign_in:
						recv(newSocket, buffer, 1024, 0);
						printf("Username: %s\n", buffer);
						// Check if user exists, if so
						// go on.
						// If not go to sign in
						// Check the type for code 430
						// If not go on
						if (check_userinfo_user(head, buffer)) {
							char *sign_in_user;
							sprintf(sign_in_user, buffer);
							bzero(buffer, sizeof(buffer));
							recv(newSocket, buffer, 1024, 0);
							printf("Password: %s\n");
							// Check the type for
							// code 430
							// Check if password is
							// correct for user
							if (check_userinfo_password(head, sign_in_user, buffer)) {
								// Send code 230
								bzero(buffer, sizeof(buffer));
								sprintf(buffer, "230");
								send(newSocket, buffer, strlen(buffer), 0);
								bzero(buffer, sizeof(buffer));
								printf("User signed in.\n");
							} else {
								// Send code 431
								bzero(buffer, sizeof(buffer));
								sprintf(buffer, "431");
								send(newSocket, buffer, strlen(buffer), 0);
								bzero(buffer, sizeof(buffer));
								goto sign_in;
							}


						}
						bzero(buffer, sizeof(buffer));
					}
					if (strcmp(buffer, "2") == 0) {
						bzero(buffer, sizeof(buffer));
						user_exists:
						recv(newSocket, buffer, 1024, 0);
						printf("Username: %s\n", buffer);
						// Check the type for code 503
						// Check user already exists
						if (check_userinfo_user(head, buffer)) {
							// Sign In Code 451
							bzero(buffer, sizeof(buffer));
							sprintf(buffer, "451");
							send(newSocket, buffer, strlen(buffer), 0);
							bzero(buffer, sizeof(buffer));
							goto user_exists;
						}
						// Initialize username for new user
						//
						bzero(buffer, sizeof(buffer));
						sprintf(buffer, "311");
						send(newSocket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
						// Initalize password for new user
						//
						recv(newSocket, buffer, 1024, 0);
						printf("Password: %s\n", buffer);
						// Check the type for code 503
						bzero(buffer, sizeof(buffer));
						// Initialize admin to false to
						// new user
						//
						// Initalize purse for new user
						//
						recv(newSocket, buffer, 1024, 0);
						printf("Purse: %s\n", buffer);
						// Check the type for code 503
						bzero(buffer, sizeof(buffer));
						// Initalize phone number for new user
						//
						recv(newSocket, buffer, 1024, 0);
						printf("Phone Number: %s\n", buffer);
						// Check the type for code 503
						bzero(buffer, sizeof(buffer));
						// Initalize address for new user
						//
						recv(newSocket, buffer, 1024, 0);
						printf("Address: %s\n", buffer);
						// Check the type for code 503
						bzero(buffer, sizeof(buffer));
						// Send confirmation code 231
						sprintf(buffer, "231");
						send(newSocket, buffer, strlen(buffer), 0);
						bzero(buffer, sizeof(buffer));
						printf("User has been successfully created.\n");
					}
					if (strcmp(buffer, "3") == 0) {
						printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
						bzero(buffer, sizeof(buffer));
						break;
					}
					//printf("Server to Client %d: ", newSocket);
					bzero(buffer, sizeof(buffer));
					//scanf("%s", &buffer[0]);
					//send(newSocket, buffer, strlen(buffer), 0);
					//bzero(buffer, sizeof(buffer));
				}
			}
		}
	}
	// Free the doc
	free_userinfo_list(head);
	yyjson_doc_free(doc);
	// Close the socket
	close(newSocket);
	return 0;
}

UserInfo *create_userinfo_node(yyjson_val *val)
{
	UserInfo *node = malloc(sizeof(UserInfo));
	if (!node) {
		log_warn("Can't allocate memory for userinfo node");
		return NULL;
	}
	yyjson_val *tmp = yyjson_obj_get(val, "id");
	node->id = yyjson_get_int(tmp);
	tmp = yyjson_obj_get(val, "user");
	node->user = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "password");
	node->password = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "admin");
	node->admin = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "purse");
	node->purse = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "phoneNumber");
	node->phone_number = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_obj_get(val, "address");
	node->address = strdup(CHECK_JSON_STR(tmp));
	return node;
}

void free_userinfo_node(UserInfo *user)
{
	free(user->user);
	free(user->password);
	free(user->admin);
	free(user->purse);
	free(user->phone_number);
	free(user->address);
	free(user);
}

void free_userinfo_list(UserInfo *head)
{
	if (!head)
		return;
	free_userinfo_list(head->next);
	free_userinfo_node(head);
}

void append_userinfo_node(UserInfo *head, yyjson_val *val)
{
	while (head->next) {
		head = head->next;
	}
	head->next = create_userinfo_node(val);
}

void delete_userinfo_node(UserInfo **head, int id)
{
	UserInfo **indirect = head, *tmp = NULL;
	while ((*indirect)->id != id) {
		indirect = &(*indirect)->next;
	}

	tmp = *indirect;
	if ((*head)->id != id)
		*indirect = (*indirect)->next;
	else
		*head = (*indirect)->next;
	free_userinfo_node(tmp);
}

void print_userinfo_list(UserInfo *head)
{
	if (!head)
		return;
	do {
		printf("%d ", head->id);
	} while((head = head->next));
	puts("\n");
}

int check_userinfo_user(UserInfo *head, char *user)
{
	if (!head)
		return FALSE;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0)
			return TRUE;
		head = head->next;
	}
	return FALSE;
}

int check_userinfo_password(UserInfo *head, char *user, char *password)
{
	if (!head)
		return FALSE;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0) {
			if (strcmp(head->password, password) == 0)
				return TRUE;
		}
		head = head->next;
	}
	return FALSE;
}

int last_userinfo_id(UserInfo *head)
{
	int id;
	if (!head)
		return -1;
	while (head != NULL) {
		id = head->id;
		head = head->next;
	}
	return id;
}

//With OS Commands
int check_user_exist(char *username)
{
	int size, idx = 0;
	char *usernames[1024], buffer[1024];
	FILE *process = popen("grep '\"user\":' ./Config/UsersInfo.json | awk '{print $2}' | cut -d '\"' -f2", "r");
	while (fgets(buffer, sizeof(buffer), process) != NULL) {
		char *p = strchr(buffer, '\n');
		if (p) *p = '\0';
		usernames[idx] = strdup(buffer);
		if (strcmp(usernames[idx], username) == 0) {
			return 1;
		}
		++idx;
	}
	pclose(process);
	return 0;
}
