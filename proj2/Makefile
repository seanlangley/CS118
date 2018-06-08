all:
	+$(MAKE) -C src
check_s:
	bash ./check_scripts/check_serv.sh
check_c:
	bash ./check_scripts/check_cli.sh

dist:
	tar -czvf proj2.tar.gz *

clean:
	rm output.txt received_file.jpg & cd ./src && make clean