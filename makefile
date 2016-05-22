####################################################################
# PROGRAMMER:         Yang Wang
# COURSE:             CSC460: Operating System
# LAST MODIFIED DATE: XXX 
# DESCRIPTION:        A makefile for the producer-Consumer problem
# FILES:              makefile
# NOTE:               N/A
# INSTRUCTION FOR EXECUTION:
#     type:  make
####################################################################

pc:  ProducerConsumer.cpp  pc.h  showBuf.o
	c++11  -pthread ProducerConsumer.cpp  showBuf.o  -o pc
showBuf.o:  showBuf.cpp  pc.h
	c++11  -c showBuf.cpp  -o showBuf.o

