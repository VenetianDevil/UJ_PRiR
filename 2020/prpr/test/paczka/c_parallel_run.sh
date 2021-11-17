cp serial_260.txt serial.txt
mpirun -np 4 ./a.out 1024 64 1901 100.0 0.1 200
