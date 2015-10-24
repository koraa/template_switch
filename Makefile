CPPFLAGS += -I"$(PWD)/include" -I"$(PWD)/vendor/googletest/include/"
CXXFLAGS += -g -O0 --std=c++14 -Wall -Wextra -Werror -pthread
LDFLAGS += -pthread

JOBS := 2

test_libs = vendor/googletest/make/gtest_main.a
test_objs = template_switch_test.o
test_headers = include/metafrog/template_switch.hpp
test_exe = template_switch_test

.PHONY: run_test clean gen_assemblys

run_test: $(test_exe)
	"./$(test_exe)"

$(test_exe): googletest $(test_libs) $(test_headers) $(test_objs)
	$(CXX) $(LDFLAGS) $(test_objs) $(test_libs) -o $@

gen_assembly_flags = --std=c++14 -Wall -Wextra -Werror -pthread -march=native -mtune=native -I"$(PWD)/include"
gen_assemblies:
	# TODO: This is utterly despicable code: Make each invocation a parallelized target!
	gcc   -S -O0    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_O0.s
	gcc   -S -O1    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_O1.s
	gcc   -S -O2    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_O2.s
	gcc   -S -O3    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_O3.s
	gcc   -S -Os    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_Os.s
	gcc   -S -Ofast -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_gcc_Ofast.s
	gcc   -S -O0    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_O0.s
	gcc   -S -O1    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_O1.s
	gcc   -S -O2    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_O2.s
	gcc   -S -O3    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_O3.s
	gcc   -S -Os    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_Os.s
	gcc   -S -Ofast -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_gcc_Ofast.s
	gcc   -S -O0    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_O0.s
	gcc   -S -O1    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_O1.s
	gcc   -S -O2    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_O2.s
	gcc   -S -O3    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_O3.s
	gcc   -S -Os    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_Os.s
	gcc   -S -Ofast -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_gcc_Ofast.s
	clang -S -O0    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_O0.s
	clang -S -O1    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_O1.s
	clang -S -O2    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_O2.s
	clang -S -O3    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_O3.s
	clang -S -Os    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_Os.s
	clang -S -Oz    -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_Oz.s
	clang -S -Ofast -DCONSTANT_TEMPLATE_CASE=0x10   $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x10_clang_Ofast.s
	clang -S -O0    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_O0.s
	clang -S -O1    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_O1.s
	clang -S -O2    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_O2.s
	clang -S -O3    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_O3.s
	clang -S -Os    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_Os.s
	clang -S -Oz    -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_Oz.s
	clang -S -Ofast -DCONSTANT_TEMPLATE_CASE=0x100  $(gen_assembly_flags) assembly_example.cc -o assembly_example_0x100_clang_Ofast.s
	clang -S -O0    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_O0.s
	clang -S -O1    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_O1.s
	clang -S -O2    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_O2.s
	clang -S -O3    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_O3.s
	clang -S -Os    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_Os.s
	clang -S -Oz    -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_Oz.s
	clang -S -Ofast -DCONSTANT_TEMPLATE_CASE=argc   $(gen_assembly_flags) assembly_example.cc -o assembly_example_argc_clang_Ofast.s

clean:
	rm -rvf $(test_objs) $(test_exe) assembly_example_*.s

clean-all: clean googletest-clean

## googletest ##

.PHONY: googletest googletest-clean

vendor/googletest/make/gtest_main.a:
	cd vendor/googletest/make; make -j$(JOBS)

googletest-clean:
	cd vendor/googletest/; git clean -fd
