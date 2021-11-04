disDir = disassembler
asmDir = assembler
procDir = processor
stackDir = SaveStack

disassembler.out:
	g++ $(disDir)/disassembler.cpp globalUtils.cpp -o $(disDir)/disassembler.out

dis: disassembler.out
	./$(disDir)/disassembler.out $(args)

assembler.out:
	g++ $(asmDir)/main.cpp $(asmDir)/assembler.cpp globalUtils.cpp -o $(asmDir)/assembler.out

asm: assembler.out
	./$(asmDir)/assembler.out $(args)

processor.out:
	g++ $(procDir)/processor.cpp globalUtils.cpp $(stackDir)/stack.cpp $(stackDir)/utils.cpp -o $(procDir)/processor.out

proc: processor.out
	./$(procDir)/processor.out $(args)