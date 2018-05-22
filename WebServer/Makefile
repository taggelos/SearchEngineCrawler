CPP = g++ -Wall -Wextra -Wconversion -g -pthread
OUT = myhttpd
OBJS = main.o fileUtil.o threadUtil.o jobQueue.o wordList.o

$(OUT): $(OBJS)
	$(CPP) $^ -o $@

%.o: %.cpp %.h
	$(CPP) -c $<

%.o: %.cpp
	$(CPP) -c $<

run:
	./$(OUT) -p 8080 -c 9090 -t 15 -d ../WebSiteCreator/empty/

val: 
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUT) -p 8080 -c 9090 -t 15 -d ../WebSiteCreator/empty/

clean:
	rm -f $(OBJS) $(OUT)
