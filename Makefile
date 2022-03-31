runMe: mapBuilder.o minHeap.o main.o heatMap.o entity.o pokemon.o fileReader.o pokemon_species.o experience.o type_names.o pokemon_moves.o moves.o
	g++ mapBuilder.o minHeap.o main.o heatMap.o entity.o pokemon.o fileReader.o pokemon_species.o experience.o type_names.o pokemon_moves.o moves.o -o runMe -lcurses

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

pokemon_species.o: pokemon_species.cpp
	g++ pokemon_species.cpp -c -ggdb -Wall

experience.o: experience.cpp
	g++ experience.cpp -c -ggdb -Wall

type_names.o: type_names.cpp
	g++ type_names.cpp -c -ggdb -Wall

pokemon_moves.o: pokemon_moves.cpp
	g++ pokemon_moves.cpp -c -ggdb -Wall

moves.o: moves.cpp
	g++ moves.cpp -c -ggdb -Wall

clean:
	rm -f runMe *.o core *~
