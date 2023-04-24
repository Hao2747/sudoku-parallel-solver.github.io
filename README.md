# Parallel Sudoku Solver
> Team Members: 
> Kevin Zhu (kzhu2)
> Hao Zhang (haozhan2)

Project Proposal [here](./Project%20Proposal.pdf)

Project Milestone [here](./Project%20Milestone.pdf)

# Summary

We plan on parallelizing a sudoku solver using multiple algorithms and then comparing overall runtime performance for each approach. Approaches include breadth-first search, distributed work queue, work-stealing distributed queue (hopefully), and a constraints-solver approach (hopefully), and if time permits, a DFS approach. We plan to be coding in OpenMP and if time permits, we also will try an approach in MPI for large sudoku boards.

## Resources Used 
We plan on using both the GHC and PSC machines, and may get benchmarks from either machine. The PSC machine will be important for the MPI approach(if we have the time to do it) as well as testing the scaling of our algorithm. As of now, we plan to code the entire thing ourselves starting from scratch. 


We plan on using existing sudoku datasets as well as our own testcases. Sources below 

+  https://github.com/t-dillon/tdoku/blob/master/benchmarks/README.md
+ https://www.sudokuwiki.org/sudoku.htm

