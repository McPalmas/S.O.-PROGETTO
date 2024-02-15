all: compile link clean	
	@chmod +x versione_processi
	@echo "Done"

compile:
	@echo "Compiling..."
	@gcc -c main.c -o main.o
	@gcc -c graphic.c -o graphic.o
	@gcc -c menu.c -o menu.o 
	@gcc -c logic.c -o logic.o 
	@gcc -c time.c -o time.o
	@gcc -c crocodile.c -o crocodile.o
	@gcc -c plant.c -o plant.o
	@gcc -c frog.c -o frog.o
	
link:
	@echo "Linking..."
	@gcc *.o -lncurses -lpthread -o versione_processi

clean:
	@echo "Cleaning..."
	@rm *.o


	
