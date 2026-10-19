CC = /opt/homebrew/opt/llvm/bin/clang
MPICC = OMPI_CC=$(CC) /opt/homebrew/bin/mpicc

CFLAGS = -std=c11 -O2 -Wall -Wextra -Iinclude
OMP_FLAGS = -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -Wl,-rpath,/opt/homebrew/opt/libomp/lib

COMMON_SRCS = src/input.c src/scoring.c src/events.c src/metrics.c

TARGETS = sequential openmp mpi hybrid

.PHONY: all clean test

all: $(TARGETS)

sequential: $(COMMON_SRCS) src/backend_seq.c src/main_seq.c
	$(CC) $(CFLAGS) $(COMMON_SRCS) src/backend_seq.c src/main_seq.c -o sequential

openmp: $(COMMON_SRCS) src/backend_omp.c src/main_omp.c
	$(CC) $(CFLAGS) $(OMP_FLAGS) $(COMMON_SRCS) src/backend_omp.c src/main_omp.c -o openmp

mpi: $(COMMON_SRCS) src/backend_mpi.c src/main_mpi.c
	$(MPICC) $(CFLAGS) $(COMMON_SRCS) src/backend_mpi.c src/main_mpi.c -o mpi

hybrid: $(COMMON_SRCS) src/backend_hybrid.c src/main_hybrid.c
	$(MPICC) $(CFLAGS) $(OMP_FLAGS) $(COMMON_SRCS) src/backend_hybrid.c src/main_hybrid.c -o hybrid

test: test_correctness
	./test_correctness

test_correctness: tests/test_correctness.c $(COMMON_SRCS) src/backend_seq.c src/backend_omp.c
	$(CC) $(CFLAGS) $(OMP_FLAGS) tests/test_correctness.c $(COMMON_SRCS) src/backend_seq.c src/backend_omp.c -o test_correctness

clean:
	rm -rf $(TARGETS) test_correctness *.dSYM/ generate_data
