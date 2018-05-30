all:
	+$(MAKE) -C src
check_s:
	bash ./check_scripts/check_serv.sh
check_c:
	bash ./check_scripts/check_cli.sh
clean:
	rm .DS_STORE && cd ./src && make clean