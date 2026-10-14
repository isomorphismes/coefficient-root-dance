# ICK compiles the maintained polynomial source and existing regression directly.
ICK ?= ick
BUILD ?= build/icky-host
CMAKE ?= cmake
NINJA ?= ninja
ANDROID_NDK ?=
RENDERER_BUILD ?= build/renderer-state
.PHONY: test renderer-state-test renderer-state-profile
test: $(BUILD)/polynomial_test
	$(BUILD)/polynomial_test

$(BUILD)/polynomial_test: tests/polynomial_test.c app/src/main/cpp/polynomial.c app/src/main/cpp/polynomial.h
	mkdir -p "$(BUILD)"
	$(ICK) -fno-link-libatomic -std=c11 -Wall -Wextra -Werror tests/polynomial_test.c app/src/main/cpp/polynomial.c -lm -o "$@"

# Both profiles execute the actual renderer through an assertion-enabled
# static Bionic test. Its GL capture observes the values sent to the shader.
renderer-state-test:
	$(MAKE) -f icky/Makefile renderer-state-profile PROFILE=Debug
	$(MAKE) -f icky/Makefile renderer-state-profile PROFILE=Release

renderer-state-profile:
	test -n "$(ANDROID_NDK)"
	$(CMAKE) -S app/src/main/cpp -B "$(RENDERER_BUILD)/$(PROFILE)" -G Ninja -DCMAKE_MAKE_PROGRAM="$(NINJA)" -DCMAKE_TOOLCHAIN_FILE="$(ANDROID_NDK)/build/cmake/android.toolchain.cmake" -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-26 -DCMAKE_BUILD_TYPE="$(PROFILE)" -DCOEFFICIENT_RENDERER_STATE_TEST=ON
	$(CMAKE) --build "$(RENDERER_BUILD)/$(PROFILE)" --target renderer_state_test
	"$(RENDERER_BUILD)/$(PROFILE)/renderer_state_test"
