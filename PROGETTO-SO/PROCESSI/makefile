all: compile link clean	
	@chmod +x VERSIONE-PROCESSI
	@echo "Done"

compile:
	@echo "Compiling..."
	@gcc -c graphic.c -o graphic.o
	@gcc -c menu.c -o menu.o 
	@gcc -c main.c -o main.o

link:
	@echo "Linking..."
	@gcc *.o -lncurses -lpthread -o VERSIONE-PROCESSI

clean:
	@echo "Cleaning..."
	@rm *.o


	
