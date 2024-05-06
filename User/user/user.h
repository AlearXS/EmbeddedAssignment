#ifndef USER_H
#define USER_H


#include "FATFS/ff.h"
#include "./flash/bsp_spi_flash.h"
//最大用户数量
#define MAX_USERS 20
//用户名、密码字段最大长度
#define STRING_MAX_LENGTH 20
// 用户表状态字段值
#define USER_STATUS_UNREGISTERED 0
#define USER_STATUS_REGISTERED 1
struct user_item{
	int id;
	char username[STRING_MAX_LENGTH];
	char password[STRING_MAX_LENGTH];
	int max_score;
	int status;// 0 未注册 1 已注册
};
extern FATFS fs;
extern struct user_item users[MAX_USERS];
int load_user_table();
int write_user_table();
int check(int id, char *password);
int Register(int id, char* username, char* password);
int delete_user(int id);

#endif /*USER_H*/
