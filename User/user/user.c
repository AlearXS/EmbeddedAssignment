#include "stm32f4xx.h"
#include "user.h"
#include <stdlib.h>
#include <string.h>

int user_table_loaded;
int user_count;

struct user_item users[MAX_USERS];


/**
  * @brief  写入用户表
  * @param  无
  * @retval bool, 0 成功，非 0 失败
  */
int write_user_table(){
	FRESULT res;
	FIL file;
	UINT bytes_written, bytes_read;
	res = f_open(&file, "user_table.bin", FA_CREATE_ALWAYS | FA_WRITE);
	if(res == FR_OK){
		res = f_write(&file, users, sizeof(users), &bytes_written);

		if (res != FR_OK) {
			// 读取数据失败，处理错误
			puts("数据写入失败");
			return 1;
		}
	}
	f_close(&file);
	user_table_loaded = 1;
	return 0;
}

/**
  * @brief  加载用户表
  * @param  无
  * @retval bool, 0 成功，非 0 失败
  */
int load_user_table(){
	if(!user_table_loaded){
		puts("加载用户表");
		FRESULT res;
		FIL file;
		UINT bytes_written, bytes_read;
		res = f_open(&file, "user_table.bin", FA_READ);
		if(res == FR_OK){
			res = f_read(&file, users, sizeof(users), &bytes_read);
			if (res != FR_OK) {
				// 读取数据失败，处理错误
				puts("数据读取失败");
				return 1;
			}
		}else{
			// 假如没有文件则创建空文件表
			write_user_table();
		}
		user_table_loaded = 1;
		f_close(&file);
	}
	return 0;
}



/**
  * @brief  身份验证
  * @param  int id, char *password
  * @retval bool, 0 成功，非 0 失败
  */
int check(int id, char *password){
	// 校验参数
	puts("check");
	if(id < 0 || id >= MAX_USERS){
		puts("id超出范围");
		return 1;
	}
	int flag = 0;
	for(int i=0;i<STRING_MAX_LENGTH;i++){
		if(password[i] == '\0'){
			flag = 1;
			break;
		}
	}
	if(!flag){
		puts("输入长度超出范围");
		return 1;
	}
	load_user_table();
	return strcmp(password, users[id].password) != 0;
}

/**
  * @brief  注册
  * @param  int id, char *password
  * @retval bool, 0 成功, 1 参数不合法，2 用户已存在
  */
int Register(int id, char* username, char* password){
	// 校验参数
	if(id < 0 || id >= MAX_USERS){
		puts("id超出范围");
		return 1;
	}
	int flag = 0;
	for(int i=0;i<STRING_MAX_LENGTH;i++){
		if(password[i] == '\0'){
			flag = 1;
			break;
		}
	}
	if(!flag){
		puts("输入长度超出范围");
		return 1;
	}
	flag = 0;
	for(int i=0;i<STRING_MAX_LENGTH;i++){
		if(username[i] == '\0'){
			flag = 1;
			break;
		}
	}
	if(!flag){
		puts("输入长度超出范围");
		return 1;
	}

	
	load_user_table();
	if(users[id].status == USER_STATUS_UNREGISTERED){
		users[id].id = id;
		users[id].max_score = 0;
		strcpy(users[id].password, password);
		strcpy(users[id].username, username);
		users[id].status = USER_STATUS_REGISTERED;
		write_user_table();
		return 0;
	}else{
		puts("用户已存在");
		return 2;
	}
}


/**
  * @brief  删除用户/注销
  * @param  int id, char *password
  * @retval bool, 0 成功, 1 失败
  */
int delete_user(int id){
	// 校验参数
	if(id < 0 || id >= MAX_USERS){
		puts("id超出范围");
		return 1;
	}

	load_user_table();
	if(users[id].status == USER_STATUS_UNREGISTERED){
		puts("用户不存在");
		return 1;
	}
	memset(&users[id], 0, sizeof(users[id]));
	return write_user_table();
}
