runMe: mapBuilder.o minHeap.o main.o heatMap.o entity.o pokemon.o fileReader.o
	g++ mapBuilder.o minHeap.o main.o heatMap.o entity.o pokemon.o fileReader.o -o runMe -lcurses

mapBuilder.o: mapBuilder.cpp
	g++ mapBuilder.cpp -c -ggdb -Wall

minHeap.o: minHeap.cpp
	g++ minHeap.cpp -c -ggdb -Wall
    
main.o: main.cpp
	g++ main.cpp -c -ggdb -Wall

heatMap.o: heatMap.cpp
	g++ heatMap.cpp -c -ggdb -Wall
	
entity.o: entity.cpp
	g++ entity.cpp -c -ggdb -Wall

fileReader.o: fileReader.cpp
	g++ fileReader.cpp -c -ggdb -Wall

pokemon.o: pokemon.cpp
	g++ pokemon.cpp -c -ggdb -Wall

clean:
	rm -f runMe *.o core *~
