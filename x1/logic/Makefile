DlR_ROOT = ..
DIR_INT =../intermediate
DIR_BIN = ../bin
DIR_PROJ= /logic

INCLUDE = -I../foundation -I../common
CC = g++
OPTIONS = -std=c++11 -g -O2 -Wall

Sources = LoGamePool.cpp
Objs = $(patsubst %.cpp,%.o, $(Sources))


logic:$(Objs)
	ar rcs $(DIR_BIN)/lib$@.a $(DIR_INT)$(DIR_PROJ)/$^

%.d : %.cpp
	$(CC) -MT"$(<:.cpp=.o) $@" -MM $(INCLUDE) $< > $@
%.o : %.cpp
	$(shell if [ ! -d "$(DIR_INT)$(DIR_PROJ)" ]; then mkdir $(DIR_INT)$(DIR_PROJ); fi)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) $(OPTIONS) $(INCLUDE) -c $^ -o $(DIR_INT)$(DIR_PROJ)/$@
