
.PHONY:all 
all:httpd wwwroot
httpd:httpd.c pthread_pool.c
	gcc -o $@ $^ -lpthread
	cd ./cgi; make clean; make;cd -

wwwroot:httpd
	mkdir wwwroot
	cp -a ./lib wwwroot/
	cp -a ./image wwwroot/
	cp -a ./cgi wwwroot/
	cp  *.html wwwroot/

.PHONY:clean
clean:	
	rm -rf httpd wwwroot
	cd ./cgi; make clean; cd -
