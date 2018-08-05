#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>

int main()
{ 
    MYSQL* mycnl = mysql_init(NULL);
    mycnl = mysql_real_connect(mycnl, "localhost", "root", "153276", "student",0,NULL,0);    
    if(mycnl == NULL)
	{
		perror("mysql_real_connect");
		return 1;
	}
	mysql_query(mycnl, "select * from student;");  
    
    MYSQL_RES* ret = mysql_store_result(mycnl);
    my_ulonglong row = mysql_num_rows(ret);
   unsigned int col = mysql_num_fields(ret);
	MYSQL_FIELD* field = mysql_fetch_fields(ret);
    int i = 0;
	printf("<table border=1><tr>\n");
	for(; i < col; ++i)
	{

	   printf("<td>%-10s</td>", field[i]);
	}
	printf("</tr>\n");
    i = 0;
	int j = 0;
	MYSQL_ROW line;

	for(; i < row; ++i)
	{
	   line = mysql_fetch_row(ret);
	   printf("<tr>");
	   for(j = 0; j < col; ++j)
	   {
	      printf("<td>%-10s</td>", line[j]);
	   }
	   printf("<tr>\n");
	}
    printf("</table>\n");

	free(ret);
    	

	return 0;
}
