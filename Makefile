CPP = g++ -Wall -Wextra -Wconversion -g -lm
OUT = minisearch
DATA = test.txt
OBJS = main.o fileUtil.o pipeUtil.o trieUtil.o wordList.o postingList.o heap.o trie.o jobExecutorUtil.o workerUtil.o

$(OUT): $(OBJS)
	$(CPP) $^ -o $@

%.o: %.cpp %.h
	$(CPP) -c $<

%.o: %.cpp
	$(CPP) -c $<

run:
	./$(OUT) -d $(DATA)

val: 
	valgrind -v --leak-check=full --show-leak-kinds=all ./$(OUT) -d $(DATA)

clean:
	rm -f $(OBJS) $(OUT) .j2w* .w2j* log/Worker_*

cclean:
	rm -f $(OBJS) $(OUT) .j2w* .w2j*
	rm -rf log