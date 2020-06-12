# Advanced-Data-Structure

Assignments from the graduate course CSC-541: Advance Data Structures.

# Assignment 1: Performance Analysis of In-memory based search and Disk-based search methods.
```
assn_1 search-mode keyfile-name seekfile-name
```
Ex:
```
assn_1 --disk-lin key.db seek.db
```
search-mode: --mem-lin, --mem-bin, --disk-lin, --disk-bin


# Assignment 2: In-memory indexing with Availability List
```
assn_2 avail-list-order studentfile-name
```
Ex:
```
assn_2 --best-fit student.db
```
avail-list-order: --first-fit, --best-fit, --worst-fit

# Assignment 3: Disk-based Mergesort (Basic, Multistep Mergesort and Replacement Policy mergesort)
```
assn_3 mergesort-method index-file sorted-index-file
```
Ex:
```
assn_3 --multistep input.bin sort.bin
```
mergesort-method: --basic, --multistep, --replacement

