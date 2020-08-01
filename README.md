Problem 1:
	Divide the array equally to each process and perform quicksort on each subarray concurrently.
	And then merge the k (number of processes) sorted arrays using a min heap to get the final sorted array.


Problem 2:
	Parallel Bellman Ford Algorithm.
	Divide the edges equally among the processes and perform bellman ford updates concurrently.
	After each iteration of edge update, send the updated distances to the root process. Receive minimum distances
	from the root and then continue to the next edge update iteration.