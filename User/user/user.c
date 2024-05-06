#include "stm32f4xx.h"
#include "user.h"
#include <stdlib.h>
#include <string.h>

int user_table_loaded;
int user_count;

struct user_item users[MAX_USERS];


/**
  * @brief  д���û���
  * @param  ��
  * @retval bool, 0 �ɹ����� 0 ʧ��
  */
int write_user_table(){
	FRESULT res;
	FIL file;
	UINT bytes_written, bytes_read;
	res = f_open(&file, "user_table.bin", FA_CREATE_ALWAYS | FA_WRITE);
	if(res == FR_OK){
		res = f_write(&file, users, sizeof(users), &bytes_written);

		if (res != FR_OK) {
			// ��ȡ����ʧ�ܣ��������
			puts("����д��ʧ��");
			return 1;
		}
	}
	f_close(&file);
	user_table_loaded = 1;
	return 0;
}

/**
  * @brief  �����û���
  * @param  ��
  * @retval bool, 0 �ɹ����� 0 ʧ��
  */
int load_user_table(){
	if(!user_table_loaded){
		puts("�����û���");
		FRESULT res;
		FIL file;
		UINT bytes_written, bytes_read;
		res = f_open(&file, "user_table.bin", FA_READ);
		if(res == FR_OK){
			res = f_read(&file, users, sizeof(users), &bytes_read);
			if (res != FR_OK) {
				// ��ȡ����ʧ�ܣ��������
				puts("���ݶ�ȡʧ��");
				return 1;
			}
		}else{
			// ����û���ļ��򴴽����ļ���
			write_user_table();
		}
		user_table_loaded = 1;
		f_close(&file);
	}
	return 0;
}



/**
  * @brief  �����֤
  * @param  int id, char *password
  * @retval bool, 0 �ɹ����� 0 ʧ��
  */
int check(int id, char *password){
	// У�����
	puts("check");
	if(id < 0 || id >= MAX_USERS){
		puts("id������Χ");
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
		puts("���볤�ȳ�����Χ");
		return 1;
	}
	load_user_table();
	return strcmp(password, users[id].password) != 0;
}

/**
  * @brief  ע��
  * @param  int id, char *password
  * @retval bool, 0 �ɹ�, 1 �������Ϸ���2 �û��Ѵ���
  */
int Register(int id, char* username, char* password){
	// У�����
	if(id < 0 || id >= MAX_USERS){
		puts("id������Χ");
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
		puts("���볤�ȳ�����Χ");
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
		puts("���볤�ȳ�����Χ");
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
		puts("�û��Ѵ���");
		return 2;
	}
}


/**
  * @brief  ɾ���û�/ע��
  * @param  int id, char *password
  * @retval bool, 0 �ɹ�, 1 ʧ��
  */
int delete_user(int id){
	// У�����
	if(id < 0 || id >= MAX_USERS){
		puts("id������Χ");
		return 1;
	}

	load_user_table();
	if(users[id].status == USER_STATUS_UNREGISTERED){
		puts("�û�������");
		return 1;
	}
	memset(&users[id], 0, sizeof(users[id]));
	return write_user_table();
}
