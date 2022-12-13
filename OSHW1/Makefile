#Makefile

hw1 : fat.o Disk.o testcase.o validate.o
	gcc -o hw1 fat.o Disk.o testcase.o validate.o
   
testcase.o : testcase.c
	gcc -c -o testcase.o testcase.c

fat.o : fat.c
	gcc -c -o fat.o fat.c
      
Disk.o : Disk.c
	gcc -c -o Disk.o Disk.c
   
clean :
	rm Disk.o fat.o testcase.o hw1
