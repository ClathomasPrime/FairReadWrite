# FairReadWrite
Read-write locks for Xinu that grant access in precisely the order it is requested

## Problem

When I learned about read-write locks in OS class,
we learned or implemented several variants.
They (essentially) had the different trade-offs listed
[here](https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock#Priority_policies).
I was somewhat shocked that none of the implementations had the
property that access to the resource was granted in exactly the order it was requested
(with multiple readers allowed simultaneous access).
In particular, 
* the "reader-biased" locks we studied
would block the writers until no readers wanted the resource
* The "writer-biased" locks we studied
would indeed block new readers once a writer made a request.
However, when that writer was finished, all of the waiting
readers would be granted access (even if another writer
had requested access before some of the readers).

## Proposal

In my opinion, the "obvious" policy has "global queue"
in which readers and writers are granted access in exactly
the order they request it.
For example, if readers **ri** and writers **wi** request
the resource in the order **r1, r2, w1, r3, r4, r5, w2, w3, r6, r7, w4**
then access will be granted in the following batches:
**{r1, r2},  {w1},  {r3, r4, r5},  {w2},  {w3},  {r6, r7},  {w4}**.
Not only is this policy "most obvious" but it seems highly applicable
in very time-sensitive programs, for example, those capturing or
manipulating real-time data.

I tried for a while to implement this lock policy,
but I could not manage it with "normal" mutexes
and synchronization primitives.
Even when I tried to manually maintain a queue
of PIDs in order to help emulate a "mixed type"
queue of reader and writer processes,
I could not finish the implementation.
This lead me to implement the locks at the kernel level,
where the implementation was straightforward.

I would love to know if this policy is actually implementable
using ordinary synchronization primitives.
Furthermore, it would be very interesting to develop enough 
formalism to tell precicely which lock policies are
implementable using which sets of synchronization primitives.
One day I hope to return to this subject;
for now, I'm leaving this repository as a reminder.

## Repo Organization

To test out the Xinu stuff again, download the Xinu source,
stick the files readlock.c and writelock.c in the /system/ folder, and
add some prototypes to /include/prototypes.h.
Maybe do some other stuff too...
