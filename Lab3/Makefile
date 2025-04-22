
  
CC = g++ -std=c++11 -Wall -O2
TARGET = ./Legalizer
ALL_OBJECT_FILES = ./Legalizer.h ./Legalizer.cpp ./block.h

all: $(ALL_OBJECT_FILES)
	$(CC) ./main.cpp -o $(TARGET) $(ALL_OBJECT_FILES)

clean:
	rm $(TARGET) *_post.lg layout.txt