CC=g++
CFLAGS=-std=c++11 -Iext -Isrc -Lext/message_v1.0.1.a
EDIR=ext
BDIR=bin
ODIR=obj
SDIR=src
TDIR=test

all: clean mount install $(BDIR)/main

$(BDIR)/main: $(ODIR)/main.o $(ODIR)/WeightedPriorityQueue.o $(ODIR)/LoadGenerator.o $(ODIR)/ThreadPool.o $(ODIR)/Server.o $(ODIR)/DistributionModel.o
	$(CC) $(CFLAGS) -o $(BDIR)/main $(ODIR)/main.o $(ODIR)/WeightedPriorityQueue.o $(ODIR)/LoadGenerator.o $(ODIR)/ThreadPool.o $(ODIR)/Server.o $(ODIR)/DistributionModel.o $(EDIR)/message.a

$(ODIR)/main.o: $(SDIR)/main.cpp
	$(CC) $(CFLAGS) -o $(ODIR)/main.o -c $(SDIR)/main.cpp

$(ODIR)/WeightedPriorityQueue.o: $(SDIR)/WeightedPriorityQueue.cpp $(SDIR)/WeightedPriorityQueue.h $(SDIR)/DistributionModel.h
	$(CC) $(CFLAGS) -o $(ODIR)/WeightedPriorityQueue.o -c $(SDIR)/WeightedPriorityQueue.cpp

$(ODIR)/LoadGenerator.o: $(SDIR)/LoadGenerator.cpp $(SDIR)/WeightedPriorityQueue.h $(SDIR)/LoadGenerator.h
	$(CC) $(CFLAGS) -o $(ODIR)/LoadGenerator.o -c $(SDIR)/LoadGenerator.cpp

$(ODIR)/ThreadPool.o: $(SDIR)/ThreadPool.cpp $(SDIR)/ThreadPool.h
	$(CC) $(CFLAGS) -o $(ODIR)/ThreadPool.o -c $(SDIR)/ThreadPool.cpp

$(ODIR)/Server.o: $(SDIR)/Server.cpp $(SDIR)/Server.h
	$(CC) $(CFLAGS) -o $(ODIR)/Server.o -c $(SDIR)/Server.cpp

$(ODIR)/DistributionModel.o: $(SDIR)/DistributionModel.cpp $(SDIR)/DistributionModel.h
	$(CC) $(CFLAGS) -o $(ODIR)/DistributionModel.o -c $(SDIR)/DistributionModel.cpp

install:
	wget -P $(EDIR) https://github.com/Zephyr-Queueing/Quartz-Model/releases/download/v1.0.4/message.a; \
	wget -P $(EDIR) https://github.com/Zephyr-Queueing/Quartz-Model/releases/download/v1.0.4/Message.h

mount:
	mkdir $(EDIR) $(BDIR) $(ODIR)

clean:
	rm -rf $(EDIR) $(BDIR) $(ODIR)
