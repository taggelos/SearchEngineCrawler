CPP = g++ -Wall -Wextra -Wconversion -g -lm
OUT = mycrawler
OBJS = main.o fileUtil.o

$(OUT): $(OBJS)
	$(CPP) $^ -o $@

%.o: %.cpp %.h
	$(CPP) -c $<

%.o: %.cpp
	$(CPP) -c $<

run:
	./$(OUT) -h 127.0.0.1 -p 8080 -c 9090 -t 15 -d ../WebSiteCreator/empty/ ../

val: 
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUT) -d $(DATA) 

clean:
	rm -f $(OBJS) $(OUT)
