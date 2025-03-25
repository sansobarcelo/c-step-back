# Rasterizer algorithms
https://gist.github.com/bert/1085538

# Debug build gen
cmake -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Release Build gen
cmake -B build -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build ./build
