# coefficient-root-dance

A native Android touch prototype for moving between polynomial coefficients and roots.

The interaction now uses a runtime-degree **monic polynomial**

`zⁿ + cₙ₋₁zⁿ⁻¹ + ... + c₁z + c₀`

with the leading coefficient fixed at `1`. The current mobile slice supports degrees 1 through 12; that bound is a renderer/interaction limit, not part of the polynomial model.

## Interaction

- Left half: numbered coefficient handles `0` through `n - 1`.
- Right half: `n` unnumbered root dots.
- Drag any coefficient handle and all roots update immediately.
- Drag any root dot and all coefficients update immediately.
- `−` and `+` at the top of the coefficient side change the degree; the number between them is the current degree.
- Increasing the degree multiplies the current polynomial by `z`, adding a root at the origin without disturbing the existing roots.
- Decreasing the degree removes the root nearest the origin, a geometric rule that does not impose visible root numbering.
- Repeated roots are shown as concentric circles, one circle per multiplicity level beyond the first dot.

Coefficient-to-root updates use a generic simultaneous root solve and then a minimum-motion assignment against the previous frame so the unnumbered root dots avoid gratuitous swaps while dragging. Root-to-coefficient updates multiply the linear factors directly.

The Android shell uses the same basic `NativeActivity` + `AInputEvent` + EGL/GLES touch/render loop already proven in Wegert. The polynomial conversion remains isolated in `polynomial.c`.

## Build

Requires Android SDK 36, NDK `29.0.14206865`, CMake 3.22.1, and Gradle 9.5+.

```sh
gradle :app:assembleDebug
```

The debug APK is written to `app/build/outputs/apk/debug/app-debug.apk`.

## Math check

```sh
make -f icky/Makefile test ICK=/path/to/qualified/ick
```

The maintained C arithmetic uses `÷` and is compiled directly by pinned ICK.
The Android and unsigned-release workflows qualify the compiler for each
maintained ABI, restore the stages under `build/ick/<abi>`, and check out the
pinned shared source producer under `.ai-ci-ick`. Local Gradle builds require
these same stages and checkout; no source rewrite or stock-C fallback is used.
NDK r29 still assembles ICK output, compiles unchanged NativeActivity glue, and
links the application. Existing Fortify2, stack protection and API26 are retained.
Native-library proofs do not replace the existing APK, emulator or device checks.

The selected compiler now includes ICK's complex-member extraction repair at
`fbe86e23d55cfec2000c08e61deea2a407fd7175`, via shared action
`66023d128a316cf9ab2c5146df60bb9fed82bb17`. The preceding compiler could expose
its internal polar components to `crealf`/`cimagf` instead of Cartesian values.
That defect moved the initial handles and prevented the existing emulator drag
checks from reaching them; the expected touch coordinates remain unchanged.

`make -f icky/Makefile renderer-state-test ANDROID_NDK=/path/to/ndk` builds and
executes the actual renderer's reset, GL scalar uploads, screen coordinates and
hit tests with the restored x86-64 stage in both Debug and Release. It includes
`dance.c` itself and replaces only the GL transport with a capture, retaining
assertions in both profiles. The preceding compiler fails this test and the
repaired native frontend passes through actual r29/API26 static Bionic. The
Android workflow runs the same regression before the unchanged emulator gate;
new cross-compiler and emulator acceptance remain exact-head CI obligations.
