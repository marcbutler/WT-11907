CXX := clang++
CXXFLAGS += -std=c++11 -fsanitize=thread -g -O0

.PHONY: run clean

run: mixed only-atomic
	@echo "******************** ATOMIC ONLY ********************\n"
	TSAN_OPTIONS="verbosity=1" setarch $(uname -m) -R ./only-atomic
	@echo "\n\n\n******************** MIXED ********************\n"
	TSAN_OPTIONS="verbosity=1" setarch $(uname -m) -R ./mixed

only-atomic: tiny-datarace.cc
	$(CXX) $(CXXFLAGS) -DATOMIC_ONLY -o $@ tiny-datarace.cc

mixed: tiny-datarace.cc
	$(CXX) $(CXXFLAGS) -o $@ tiny-datarace.cc

clean:
	$(RM) -f mixed only-atomic
