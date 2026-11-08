cmake -DDEBUG_MODE=OFF -DTESTS=ON -B build -S .
cmake --build build -j $(nproc) --clean-first
./build/interpreter
