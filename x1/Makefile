
all:common logic

logic:common force
	make -C ./logic

common:force
	make -C ./common

force:;

.PHONY : clean  
clean:   
	-find ./intermediate -name "*.o" | xargs rm -f
	-find ./bin -name "*.a" | xargs rm -f
	-find ./bin -name "*.so" | xargs rm -f
