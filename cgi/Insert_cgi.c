#include <stdio.h>
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    char data[1024] = {0};
    char sql[128] = {0};
   	char name[20] = {0};
	char sex[3] = { 0 };
    int age = 0;
	if(strcasecmp((char*)getenv("METHOD"), "GET") == 0)
	{
	   strcpy(data, (char*)getenv("QUERY_STRING"));
	}	
	else if(strcasecmp((char*)getenv("METHOD"), "POST") == 0)
	{
		int i = 0;
		int size = atoi((char*)getenv("CONTENT_LENGTH"));
		for(; i < size; ++i)
		{
			read(0, data+i, 1);
		}
	}
	else
	{
		exit(0);
	}
	MYSQL* mycnl = mysql_init(NULL);
    mycnl = mysql_real_connect(mycnl, "localhost", "root", "153276", "student",0,NULL,0);    
    if(mycnl == NULL)
	{
		return 1;
	}
//   	printf("data:%s\n", data);	
    strtok(data,"=&");
    strcpy(name, strtok(NULL, "=&"));
	strtok(NULL,"=&");
	age = atoi(strtok(NULL, "=&"));
	strtok(NULL,"=&");
	strcpy(sex, strtok(NULL, "=&"));
	sprintf(sql, "INSERT INTO student (name,age,sex) values ('%s', %d, '%s')", name, age, sex);
	
	mysql_query(mycnl, sql);
	//	mysql_query(mycnl,"INSERT INTO student (name,age,sex)values('zhangsan', 29, 'w')");
    printf("----OK !\n");
    return 0;
}
