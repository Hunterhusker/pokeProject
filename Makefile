runMe: mapBuilder.o minHeap.o main.o heatMap.o player.o
	gcc mapBuilder.o minHeap.o main.o heatMap.o player.o -o runMe

mapBuilder.o: mapBuilder.c
	gcc mapBuilder.c -c -ggdb -Wall

minHeap.o: minHeap.c
	gcc minHeap.c -c -ggdb -Wall
    
main.o: main.c
	gcc main.c -c -ggdb -Wall

heatMap.o: heatMap.c
	gcc heatMap.c -c -ggdb -Wall
	
player.o: player.c
	gcc player.c -c -ggdb -Wall

clean:
	rm -f runMe *.o core *~
