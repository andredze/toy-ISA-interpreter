| Command | Mean, ms | Relative time | Speedup |
|---|---:|---:|---:|
| `taskset -c 0 ./build/release-interpreter/interpreter "examples/crc32.elf"` | 242.924 | 1.000 | 1.000 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 0` | 42.709 | 0.176 | 5.688 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 1` | 42.930 | 0.177 | 5.659 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 5` | 40.929 | 0.168 | 5.935 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 10` | 41.915 | 0.173 | 5.796 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 50` | 40.971 | 0.169 | 5.929 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 100` | 42.252 | 0.174 | 5.749 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 500` | 41.268 | 0.170 | 5.887 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 1000` | 43.573 | 0.179 | 5.575 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 5000` | 41.870 | 0.172 | 5.802 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 10000` | 40.878 | 0.168 | 5.943 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 50000` | 43.801 | 0.180 | 5.546 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 100000` | 52.700 | 0.217 | 4.610 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 500000` | 108.145 | 0.445 | 2.246 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 1000000` | 160.057 | 0.659 | 1.518 |
| `taskset -c 0 ./build/release-jit/interpreter "examples/crc32.elf" 5000000` | 242.324 | 0.998 | 1.002 |
