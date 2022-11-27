CC=g++
CFLAGS=-std=c++11 -Iext -Isrc -Lext/message_v1.0.1.a
EDIR=ext
BDIR=bin
ODIR=obj
SDIR=src
TDIR=test

all: clean mount install $(BDIR)/main

$(BDIR)/main: $(ODIR)/main.o $(ODIR)/WeightedPriorityQueue.o $(ODIR)/LoadGenerator.o
	$(CC) $(CFLAGS) -o $(BDIR)/main $(ODIR)/main.o $(ODIR)/WeightedPriorityQueue.o $(ODIR)/LoadGenerator.o $(EDIR)/message.a

$(ODIR)/main.o: $(SDIR)/main.cpp
	$(CC) $(CFLAGS) -o $(ODIR)/main.o -c $(SDIR)/main.cpp

$(ODIR)/WeightedPriorityQueue.o: $(SDIR)/WeightedPriorityQueue.cpp $(SDIR)/WeightedPriorityQueue.h $(SDIR)/DistributionModel.h
	$(CC) $(CFLAGS) -o $(ODIR)/WeightedPriorityQueue.o -c $(SDIR)/WeightedPriorityQueue.cpp

$(ODIR)/LoadGenerator.o: $(SDIR)/LoadGenerator.cpp $(SDIR)/WeightedPriorityQueue.h $(SDIR)/LoadGenerator.h
	$(CC) $(CFLAGS) -o $(ODIR)/LoadGenerator.o -c $(SDIR)/LoadGenerator.cpp

install:
	wget -P $(EDIR) https://github.com/Zephyr-Queueing/Quartz-Model/releases/download/v1.0.4/message.a; \
	wget -P $(EDIR) https://github.com/Zephyr-Queueing/Quartz-Model/releases/download/v1.0.4/Message.h

mount:
	mkdir $(EDIR) $(BDIR) $(ODIR)

clean:
	rm -rf $(EDIR) $(BDIR) $(ODIR)
