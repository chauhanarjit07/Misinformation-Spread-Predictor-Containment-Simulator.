#!/bin/bash
# Build (rebuild if build/ missing)
mkdir -p build
cmake -B build -S . || cmake ..
cmake --build build/ --config Debug
# Run simulation
./build/Debug/rumorguard.exe --nodes 500 --m 3 --trials 20 --outdir results/
echo "Done. Check results/."
echo "Next: open ui/index.html in browser."
