# ICK compiles the maintained polynomial source and existing regression directly.
ICK ?= ick
BUILD ?= build/icky-host
.PHONY: test
test: $(BUILD)/polynomial_test
	$(BUILD)/polynomial_test

$(BUILD)/polynomial_test: tests/polynomial_test.c app/src/main/cpp/polynomial.c app/src/main/cpp/polynomial.h
	mkdir -p "$(BUILD)"
	$(ICK) -std=c11 -Wall -Wextra -Werror tests/polynomial_test.c app/src/main/cpp/polynomial.c -lm -o "$@"
