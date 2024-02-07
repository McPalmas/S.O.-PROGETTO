all: compile link clean	
	@chmod +x VERSIONE-THREAD
	@echo "Done"

compile:
	@echo "Compiling..."
	@gcc -c main.c -o main.o
	@gcc -c graphic.c -o graphic.o
	@gcc -c menu.c -o menu.o
	@gcc -c extern.c -o extern.o 
	@gcc -c logic.c -o logic.o 
	@gcc -c time.c -o time.o
	@gcc -c crocodile.c -o crocodile.o
	@gcc -c plant.c -o plant.o
	@gcc -c frog.c -o frog.o

link:
	@echo "Linking..."
	@gcc *.o -lncurses -lpthread -o VERSIONE-THREAD

clean:
	@echo "Cleaning..."
	@rm *.o


	
