# gucdb
Key-value store

What's used:
- B-Trie for index
- 2^N sized slabs for values

No concurrency ATM, but I was planning to write a log-structured access layer to it.
