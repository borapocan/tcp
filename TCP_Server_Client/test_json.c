// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <strings.h> // bzero()
#include <unistd.h> // read(), write(), close()
#include "yyjson.h"
#include "yyjson.c"
//#include "userinfo.h"
#include "log.h"
#include "log.c"
//#include "userinfo.c"
#define MAX 80
#define PORT 8080
#define CHECK_JSON_STR(s) s ? yyjson_mut_get_str(s) : "\0"

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

UserInfo *create_userinfo_node(yyjson_mut_val *val);
void free_userinfo_node(UserInfo *user);
void free_userinfo_list(UserInfo *head);
void delete_userinfo_node(UserInfo **head, int id);
void append_userinfo_node(UserInfo *head, yyjson_mut_val *val);
void print_userinfo_list(UserInfo *head);
int check_userinfo_exist(UserInfo *head, char *user);
char* get_userinfo_password(UserInfo *head, char *user);
int last_userinfo_id(UserInfo *head);

int main(int argc, char const* argv[])
{
	//Read JSON file, allowing comments and trailing commas
	yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS
				| YYJSON_READ_ALLOW_TRAILING_COMMAS;
	yyjson_read_err err;
	yyjson_doc *idoc = yyjson_read_file("./Config/UsersInfo.json",
					   flg, NULL, &err);
	yyjson_mut_doc *doc = yyjson_doc_mut_copy(idoc, NULL);
	yyjson_mut_val *obj = yyjson_mut_doc_get_root(doc);
	UserInfo *head = NULL, *list_iter = NULL;
	// Iterate over the root object
	yyjson_mut_val *arr = yyjson_mut_obj_get(obj, "users");
	yyjson_mut_arr_iter iter;
	yyjson_mut_arr_iter_init(arr, &iter);
	yyjson_mut_val *val;
	if (doc) {
		while ((val = yyjson_mut_arr_iter_next(&iter))) {
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


	//const char *json = yyjson_mut_mut_write(doc, 0, NULL);
	//if (json) {
	//	printf("json: %s\n", json); // {"name":"Mash","star":4,"hits":[2,2,1,3]}
	//	free((void *)json);
	//}
	//const char *pairs[] = {"id", "5", "user", "Harry", "password", "passw", "admin", "false",
	//			"purse", "1000", "phoneNumber", "0542", "address", "TR"};
	//yyjson_mut_obj_with_kv(doc, pairs, 7);
	//yyjson_mut_val *tmp = yyjson_mut_obj_get(val, "id");
	//const char *pairs[] = {"name", "Harry", "type", "student", "id", "123456"};
	//yyjson_mut_obj_with_kv(doc, pairs, 3);
	//yyjson_mut_arr_append(arr, tmp);

	//yyjson_mut_arr_append(yyjson_mut_val *arr, yyjson_mut_val *val);
	//yyjson_mut_val *root = yyjson_mut_obj(doc);
	//yyjson_mut_doc_set_root(doc, root);
	// Set root["name"] and root["star"]
	//yyjson_mut_obj_add_str(doc, root, "user", "Mash");
	//yyjson_mut_obj_add_str(doc, root, "password", "pass");
	// Set root["hits"] with an array
	//int hits_arr[] = {2, 2, 1, 3};
	//yyjson_mut_val *hits = yyjson_mut_arr_with_sint32(doc, hits_arr, 4);
	//yyjson_mut_obj_add_val(doc, root, "hits", hits);

	//char *keys[] = {"name", "type", "id"};
	//char *vals[] = {"Harry", "student", "123456"};
	//yyjson_mut_obj_with_str(doc, *keys, *vals, 3);


	//yyjson_mut_obj_add(obj, *keys, *vals);
	//yyjson_mut_val *users = yyjson_mut_str(doc, "users");
	yyjson_mut_val *userinfo = yyjson_mut_arr(doc);
	//yyjson_mut_val *id = yyjson_mut_int(doc, 5);
	//yyjson_mut_arr_append(userinfo, id);
	yyjson_mut_val *key = yyjson_mut_str(doc, "id");
	yyjson_mut_val *num = yyjson_mut_int(doc, 123);
	yyjson_mut_obj_add(obj, key, num);
	//yyjson_mut_arr_append(, num);
	//yyjson_mut_obj_add(obj, key, num);
	//yyjson_mut_val *key = yyjson_mut_str(doc, "page");
	//yyjson_mut_val *num = yyjson_mut_int(doc, 123);
	//yyjson_mut_obj_add(arr, users, userinfo);

	// Create 3 string value, add to the array object.
	//yyjson_mut_val *names = yyjson_mut_arr(doc);
	//yyjson_mut_val *name1 = yyjson_mut_str(doc, "Harry");
	//yyjson_mut_val *name2 = yyjson_mut_str(doc, "Ron");
	//yyjson_mut_val *name3 = yyjson_mut_str(doc, "Hermione");
	//yyjson_mut_arr_append(names, name1);
	//yyjson_mut_arr_append(names, name2);
	//yyjson_mut_arr_append(names, name3);


	// Set the document's root value.
	yyjson_mut_doc_set_root(doc, obj);

	// Write to JSON string
	char *json = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, NULL);
	printf("%s\n", json);
	free(json);
	//Free the doc
	free_userinfo_list(head);
	yyjson_mut_doc_free(doc);
	//Close the socket
	return 0;
}

UserInfo *create_userinfo_node(yyjson_mut_val *val)
{
	UserInfo *node = malloc(sizeof(UserInfo));
	if (!node) {
		log_warn("Can't allocate memory for userinfo node");
		return NULL;
	}
	yyjson_mut_val *tmp = yyjson_mut_obj_get(val, "id");
	node->id = yyjson_mut_get_int(tmp);
	tmp = yyjson_mut_obj_get(val, "user");
	node->user = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_mut_obj_get(val, "password");
	node->password = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_mut_obj_get(val, "admin");
	node->admin = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_mut_obj_get(val, "purse");
	node->purse = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_mut_obj_get(val, "phoneNumber");
	node->phone_number = strdup(CHECK_JSON_STR(tmp));
	tmp = yyjson_mut_obj_get(val, "address");
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

void append_userinfo_node(UserInfo *head, yyjson_mut_val *val)
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

int check_userinfo_exist(UserInfo *head, char *user)
{
	if (!head)
		return 0;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0)
			return 1;
		head = head->next;
	}
	return 0;
}

char* get_userinfo_password(UserInfo *head, char *user)
{
	if (!head)
		return 0;
	while (head != NULL) {
		if (strcmp(head->user, user) == 0)
			return head->password;
		head = head->next;
	}
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
