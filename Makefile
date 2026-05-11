all:
	g++ -fPIC -shared src/*.cpp -o bin/libbib.so
	g++ src/main.cpp -Lbin -lbib -Wl,-rpath=./bin -o bin/programa

tests:
	g++ test/*.cpp -Lbin -lbib -Wl,-rpath=./bin -o bin/testeRegressivo

clean:
	rm -f bin/*