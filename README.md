# Advanced-Data-Structure
Codes
The goals of this assignment are two-fold:

    To introduce you to random-access file I/O in UNIX using C.
    To investigate time efficiency issues associated with in-memory versus disk-based searching strategies. 

This assignment uses two "lists" of integer values: a key list and a seek list. The key list is a collection of integers K = (k0, ..., kn-1) representing n keys for a hypothetical database. The seek list is a separate collection of integers S = (s0, ..., sm-1) representing a sequence of m requests for keys to be retrieved from the database.

You will implement two different search strategies to try to locate each si from the seek list:

    Linear search. A sequential search of K for a key that matches the current seek value si.
    Binary search. A binary search through a sorted list of keys K for a key that matches the current seek value si. The fact that the keys are sorted allows approximately half the remaining keys to be ignored from consideration during each step of the search. 

Each of the two searches (linear and binary) will be performed in two different environments. In the first, the key list K will be held completely in memory. In the second, individual elements ki ∈ K will read from disk as they are needed.
