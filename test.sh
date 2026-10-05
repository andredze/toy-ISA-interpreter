cmake -DDEBUG_MODE=OFF -DTESTS=ON -B build -S .
cmake --build build -j $(nproc) --clean-first
./build/anki_interpreter