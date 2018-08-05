#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
void cal(int left, int right)
{
    printf("%d + %d = %d\n", left, right, left+right);
    printf("%d - %d = %d\n", left, right, left-right);
    printf("%d x %d = %d\n", left, right, left*right);
    printf("%d / %d = %d\n", left, right, left/right);

}


int main()
{
    char data[1024] = { 0 };
//    printf("-------cal--------\n");    
    if(strcasecmp((char*)getenv("METHOD"), "GET") == 0)
	{
		strcpy(data,  (char*)getenv("QUERY_STRING"));
	}	
	else if(strcasecmp((char*) getenv("METHOD"), "POST") == 0)
	{
	    int size = atoi((char*)getenv("CONTENT_LENGTH"));
		int i = 0;
		for(; i < size; ++i)
		{
			read(0, data+i, 1);
		}
	}
   
 //   printf("data:%s\n", data);
    int i = 0;
	int j = 0;
    char s1[30] = {0};
	char s2[30] = {0};
	while(data[i] != '&')
	{
	    if( '0' <= data[i] && data[i] <= '9')
		{
			s1[j++] = data[i];
		}
		++i;
	}
	j = 0;
	while(data[i] != '\0')
	{
	    if( '0' <= data[i] && data[i] <= '9')
		{
			s2[j++] = data[i];
		}
		++i;
	}
//	printf("s1=%s,s2=%s\n",s1, s2);
	int left = atoi(s1);
	int right = atoi(s2);
	cal(left,right);
	return 0;
}
