Design and software simulation of a data/instruction cache

A logic project focused on writing a script for an instruction/data cache algorithm. The goal was to create a shareable cache among 4 processors. The instruction cache was to be 8-way associative and the data cache - 4-way. The algorithm uses the LRU and MESI protocols to keep the data up-to-date among the processors. As the cache is filled in, the LRU policy determines which line of cache should be erased to accommodate a new one. As the processors access the cache, the lines are assigned a status according to MESI to make sure processors do not use obsolete data.
The data for testing is provided as an external txt file with a list of commands to execute. The code outputs the current state of the cache at the end to check. All tests were passed and the requirements were met.

MCU: n/a
Script: Python
Components: n/a
Timeline: 1 month
Team role: code