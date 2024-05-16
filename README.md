# WT-11907

[MongoDB Jira Ticket](https://jira.mongodb.org/browse/WT-11907)

Experiment to see if TSAN can be used without C11 atomics, without
requiring blanket suppression: by instead using compiler builtin
atomic functions.

This is intended to be used *only* as an interim solution. The
standard atomic API is preferrable for any number of reasons.
