rm .log
cmake -DDEBUG_MODE=ON -B build -S .
cmake --build build -j $(nproc)