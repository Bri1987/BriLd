build:
	rm -rf cmake-build-debug
	mkdir cmake-build-debug
	cd cmake-build-debug && cmake ..
	make -C cmake-build-debug

test:
	chmod 777 ./test/hello.sh
	@printf '\e[32mBegin Test\e[0m\n'
	./test/hello.sh
	@printf '\e[32mPassed all tests\e[0m\n'

clean:
	rm -rf out/
	rm -rf ld

.PHONY: build clean test