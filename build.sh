rm .log
cmake -DDEBUG_MODE=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DTESTS=OFF -B build -S .
cmake --build build -j $(nproc)