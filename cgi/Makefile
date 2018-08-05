.PHONY:all
all:cal_cgi Insert_cgi Select_cgi
cal_cgi:cal_cgi.c
	gcc -o $@ $^
Insert_cgi:Insert_cgi.c
	gcc -o $@ $^ -I./lib/include -L./lib/lib -lmysqlclient
Select_cgi:Select_cgi.c
	gcc -o $@ $^ -I./lib/include -L./lib/lib -lmysqlclient
.PHONY:clean
clean:
	rm -f cal_cgi Insert_cgi Select_cgi
