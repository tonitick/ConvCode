main:RandNum.o Information.o Noise.o Channel.o CConvCodec.o Runner.o main.cpp
	g++ main.cpp *.o -o main.out
Runner.o:Runner.cpp Runner.h RandNum.o Information.o Noise.o Channel.o
	g++ -c Runner.cpp
CConvCodec.o:CConvCodec.cpp CConvCodec.h
	g++ -c CConvCodec.cpp
Channel.o:Channel.cpp Channel.h Noise.o
	g++ -c Channel.cpp
Noise.o:Noise.cpp Noise.h RandNum.o
	g++ -c Noise.cpp
Information.o:Information.h Information.cpp RandNum.o
	g++ -c Information.cpp
RandNum.o:RandNum.h RandNum.cpp
	g++ -c RandNum.cpp
clean:
	rm *.o *.out
