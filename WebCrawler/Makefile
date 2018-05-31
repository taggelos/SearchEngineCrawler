CPP = g++ -Wall -Wextra -Wconversion -g -lm -pthread
OUT = mycrawler
OBJS = main.o fileUtil.o threadUtil.o siteQueue.o

$(OUT): $(OBJS)
	$(CPP) $^ -o $@

%.o: %.cpp %.h
	$(CPP) -c $<

%.o: %.cpp
	$(CPP) -c $<

run:
	./$(OUT) -h 127.0.0.1 -p 8080 -c 9091 -t 15 -d /sites /site0/page0_9117.html

val: 
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUT) -d $(DATA) 

clean:
	rm -rf $(OBJS) $(OUT) sites
