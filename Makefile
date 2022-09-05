OBJS = main.o semaphores.o clientFunct.o helperFuncts.o

main.o: main.c semaphores.h clientFunct.h
	gcc -c main.c

semaphores.o: semaphores.c semaphores.h
	gcc -c semaphores.c

clientFunct.o: clientFunct.c clientFunct.h helperFuncts.h
	gcc -c clientFunct.c

helperFuncts.o: helperFuncts.c helperFuncts.h
	gcc -c helperFuncts.c

EXEC = main

$(EXEC): $(OBJS)
	gcc $(OBJS) -o $(EXEC)

ARGS = input.txt 8 6

run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	rm *.o