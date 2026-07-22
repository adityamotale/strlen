# `strlen`

A hand-rolled implementation of `strlen` in x86-64 assembly using AVX-512 SIMD ISA.

## Benchmark

The benchmark measures throughput using a dynamically allocated 256 MiB string,

* Payload: 256 MiB
* Throughput: ~12.5 GiB/s
* Limiting Factor: Memory-bound (Single-Core LFB Limit)

> [!TIP]
> Despite AVX-512 theoretically being capable of processing 100+ GB/s per core, the throughput hits a hard wall at
> ~12.5 GiB/s. This is an architectural limitation, not a compute one. The bottleneck is strictly caused by the
> single core's Line Fill Buffers (LFBs) choking against pure RAM latency. Once a core's LFBs are fully saturated
> waiting for memory fetches, it cannot pull data any faster, regardless of system RAM capacity or channel width.

## Local Usage

NOTE: Requires a CPU with AVX-512 support (specifically `AVX512F`, `AVX512BW`, and `AVX512VL`)

```bash
nasm -f elf64 strlen_avx512.asm -o strlen_avx512.o
gcc -O3 -mavx512f -mavx512bw -mavx512vl bench.c strlen_avx512.o -o bench
taskset -c 1 ./bench
```
