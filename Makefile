CC = gcc
CFLAGS = -Wall -Wextra -g -O0

main.exe: main.c BSCMap.c BSCMap.h BSCStr.c BSCStr.h
	$(CC) $(CFLAGS) main.c BSCMap.c BSCStr.c -o main.exe

run: main.exe
	.\main.exe

clean:
	del /Q main.exe
