CPPFLAGS += -I"$(PWD)/include" -I"$(PWD)/vendor/googletest/include/"
CXXFLAGS += -g -O0 --std=c++14 -Wall -Wextra -Werror -pthread
LDFLAGS += -pthread

JOBS := 2

test_libs = vendor/googletest/make/gtest_main.a
test_objs = template_switch_test.o
test_exe = template_switch_test

.PHONY: run_test clean

run_test: $(test_exe)
	"./$(test_exe)"

$(test_exe): googletest $(test_libs) $(test_objs)
	$(CXX) $(LDFLAGS) $(test_objs) $(test_libs) -o $@

clean:
	rm -rvf $(test_objs) $(test_exe)

clean-all: clean googletest-clean

## googletest ##

.PHONY: googletest googletest-clean

vendor/googletest/make/gtest_main.a:
	cd vendor/googletest/make; make -j$(JOBS)

googletest-clean:
	cd vendor/googletest/; git clean -fd
