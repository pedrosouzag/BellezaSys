all: lib tests

lib:
	g++ -Wall -Wextra -std=c++17 -fPIC -shared src/core/*.cpp -Iinclude -o bin/libbellezasys.so

tests:
	g++ -Wall -Wextra -std=c++17 test/funcional/*.cpp -Iinclude -Lbin -lbellezasys -o bin/funcional_tests
	g++ -Wall -Wextra -std=c++17 test/unit/*.cpp -Iinclude -Itest/unit -Lbin -lbellezasys -o bin/unit_tests

runFunctional:
	LD_LIBRARY_PATH=./bin ./bin/funcional_tests

runUnit:
	LD_LIBRARY_PATH=./bin ./bin/unit_tests

run: runFunctional runUnit

clean:
	rm -rf bin/
