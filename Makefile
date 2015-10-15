CPPFLAGS += -I"$(PWD)/include"
CXXFLAGS += -g -O0 --std=c++14 -Wall -Wextra -Werror

test_objs = template_switch_test.o
test_exe = template_switch_test

.PHONY: run_test clean

run_test: $(test_exe)
	"./$(test_exe)"

$(test_exe): $(test_objs)
	$(CXX) $(LDFLAGS) $(test_objs) -o $@

clean:
	rm -rv $(test_objs) $(test_exe)
