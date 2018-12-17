P3.out : Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o
	g++ -g -o P3.out Project3.o SetLimits.o LexicalAnalyzer.o SyntacticalAnalyzer.o CodeGenerator.o

Project3.o : Project3.cpp SetLimits.h SyntacticalAnalyzer.h
	g++ -g -c Project3.cpp

SetLimits.o : SetLimits.cpp SetLimits.h
	g++ -g -c SetLimits.cpp

LexicalAnalyzer.o : LexicalAnalyzer.cpp LexicalAnalyzer.h
	g++ -g -c LexicalAnalyzer.cpp

SyntacticalAnalyzer.o : SyntacticalAnalyzer.cpp SyntacticalAnalyzer.h LexicalAnalyzer.h
	g++ -g -c SyntacticalAnalyzer.cpp

CodeGenerator.o : CodeGenerator.cpp CodeGenerator.h
	g++ -g -c CodeGenerator.cpp

clean : 
	rm -f *.o P3.out *.gch *.lst *.dbg *.p* *~

submit : Project3.cpp LexicalAnalyzer.h LexicalAnalyzer.cpp SyntacticalAnalyzer.h SyntacticalAnalyzer.cpp CodeGenerator.h CodeGenerator.cpp makefile README.txt
	rm -rf Team1P3
	mkdir Team1P3
	cp Project3.cpp Team1P3
	cp LexicalAnalyzer.h Team1P3
	cp LexicalAnalyzer.cpp Team1P3
	cp SyntacticalAnalyzer.h Team1P3
	cp SyntacticalAnalyzer.cpp Team1P3
	cp CodeGenerator.h Team1P3
	cp CodeGenerator.cpp Team1P3
	cp makefile Team1P3
	cp README.txt Team1P3
	tar cfvz Team1P3.tgz Team1P3
	cp Team1P3.tgz ~tiawatts/cs460drop
