# Lab 14

## Exercise 1

> Given the way the address was broken down, how big are the pages in this model?

5 bits offset, page size is 2^5 = 32 bytes.

> Explain the process by which we turn a virtual address into a physical address (emphasizing on the usage of TLB and Page table and the order of using them).

1. divide the virtual address into page number and offset.
2. check the TLB for the page number:
   - If it is a hit, we get the physical address
   - If it is a miss, check page table.
3. check the page table for the page number:
    - If the page is present, we get the physical address.
    - If the page is not present (page fault), allocate a new physical page for the virtual page, update the page table, TLB, and get the physical address.

> How many TLB Hits and Misses did we have for the randomly generated set of ten addresses? What about for Page Hits and Page Faults?

At most 9 TLB hits, at least 0

At most 9 page hits, at least 0

## Exercise 2

> Demonstrate that your ten memory accesses results in ten TLB Misses and ten Page Faults. Explain why such behavior occurs.

`00, 20, 40, 60, 80, a0, c0, e0, 00, 20`

The first 4 accesses are all in different pages, so they all result in TLB misses and page faults, these pages fill up the physical memory. The next 4 accesses are also in different pages and not the pages for the first 4 accesses, so they also result in TLB misses and page faults.

When the 9th access is made, the first 4 pages are already evicted from physical memory, so the 9th and 10th accesses also result in TLB misses and page faults.

## Exercise 3

> Explain the single parameter change that would result in ten TLB misses, but fewer than ten page faults.

Change memory size from 128 bytes (4 pages) to 256 bytes (8 pages). This way, all the virtual pages can fit in physical memory, and there are no evictions. The first 8 accesses will result in page faults but after that all pages are valid in page table.
