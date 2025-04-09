Cache memory pipeline simulator

A logic project focused on writing a script for an instruction/data cache memory pipeline algorithm. The pipeline consists of 5 stages: fetch, decode, execute, memory access, write back. Data was fed in as a list of hex-encoded commands to execute. Each command contained the registers to perform operations on and the code for the operation itself. 12 different operations (arithmetic, logical, memory access and conditional) could be encountered. As registers were operated on, the cache was to determine any dependencies and hold execution of the dependent operations until the registers were up-to-date. The code allows running the operations in a forwarding and non-forwarding way.

MCU: n/a
Script: Python
Components: n/a
Timeline: 1 month
Team role: code