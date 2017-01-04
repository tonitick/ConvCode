# error control coding(2016 fall) project 3

* Channel.h and Channel.cpp was adapted from Simon's code.  
RandNum.h and RandNum.cpp was created by prof.Ma as annotated.

* Run the program(in linux environment) by:
  	* cd src
  	* make
  	* ./main.out

* The format of sample file
    * the first line is an integer I, denotes the length of input
    * the first line is an integer O, denotes the length of output
    * the third line is an integer S, denotes the number of stages
    * for the next I * S lines, each line denotes one edge in the trellis and
      contains 4 integer. They are the left vertex, input label, output label
      and right vertex, respectively. Note that when they are transformed to 
      binary format, the least significant binary digit is the leftmost digit.
