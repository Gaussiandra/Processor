disDir = disassembler

disassembler.out:
	g++ $(disDir)/disassembler.cpp globalUtils.cpp -o $(disDir)/disassembler.out

dis: disassembler.out
	./$(disDir)/disassembler.out $(args)