# Tests

Two suites, each its own binary. Nothing is wired into the root `CMakeLists.txt`,
so the game build is untouched.

## Part B — rules testable without the seam

Compiles against tag `lab4-base` as well as this branch.

```
g++ -std=c++17 -Iinclude -Itests src/util.cpp src/platform.cpp src/score.cpp tests/test_baseline_rules.cpp -o partb_tests
./partb_tests
```

## Part D — the eating rule, through the seam

Needs the seam, so it only builds on `lab4`.

```
g++ -std=c++17 -Iinclude -Itests src/tick.cpp src/util.cpp src/platform.cpp tests/test_tick_rules.cpp -o seam_tests
./seam_tests
```

## Coverage (Part C)

Run at `lab4-base` over every source file except `main.cpp`:

```
g++ --coverage -O0 -g -std=c++17 -Iinclude -Itests src/game.cpp src/platform.cpp src/render.cpp src/score.cpp src/ui.cpp src/util.cpp tests/test_baseline_rules.cpp -o partb_tests
./partb_tests
gcov -b partb_tests-util.gcda
```

On MinGW add `-static -static-libgcc -static-libstdc++` if the binary cannot find
its runtime DLLs.
