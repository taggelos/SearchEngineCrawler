CPP = g++ -Wall -Wextra -Wconversion -g -lm -pthread -IJobExecutorModified
OUT = mycrawler
OBJS = main.o threadUtil.o commandHandler.o siteQueue.o JobExecutorModified/fileUtil.o JobExecutorModified/pipeUtil.o JobExecutorModified/trieUtil.o JobExecutorModified/wordList.o JobExecutorModified/postingList.o JobExecutorModified/pathList.o JobExecutorModified/heap.o JobExecutorModified/trie.o JobExecutorModified/jobExecutorUtil.o JobExecutorModified/workerUtil.o

$(OUT): $(OBJS)
	$(CPP) main.o commandHandler.o threadUtil.o siteQueue.o fileUtil.o pipeUtil.o trieUtil.o wordList.o postingList.o pathList.o heap.o trie.o jobExecutorUtil.o workerUtil.o -o $@

%.o: %.cpp %.h
	$(CPP) -c $<

%.o: %.cpp
	$(CPP) -c $<

run:
	./$(OUT) -h 127.0.0.1 -p 8080 -c 9091 -t 15 -d /sites /site0/page0_9117.html

val: 
	valgrind -v --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(OUT) -d $(DATA) 

clean:
	rm -rf *.o sites
	rm -f $(OBJS) $(OUT) .j2w* .w2j*
	rm -rf log