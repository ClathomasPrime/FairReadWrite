// writelock.c

#include <xinu.h>

// Invariant: 
// 	* at any point (outside these syscalls) if the queue is nonempty
// 	  then either a writer is living or a writer is on top of the queue

/*------------------------------------------------------------------------
 * writelock - lock yourself with write privileges
 *------------------------------------------------------------------------
 */
syscall	writelock
	( rwid32 rw 
) {
	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process' table entry	*/
	struct	rwlockentry *rwlockptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	if (isbadrwlock(rw)) {
		restore(mask);
		return SYSERR;
	}
	rwlockptr = &rwlocktab[rw];
	if (rwlockptr->state == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	rwlockptr->wait = TRUE;

	// wait if queue is already populated or readers a living
	bool8 wait = 
		nonempty(rwlockptr->queue) || rwlockptr->nReaders > 0;

	enqueue(currpid, rwlockptr->queue);
	queuetab[currpid].qkey = WRITER;

	if(wait){
		prptr = &proctab[currpid];
		prptr->prrwlock = rw;
		prptr->prstate = PR_WAIT;
		resched();
	}

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
syscall	writeunlock(rwid32 rw) {
	intmask mask;			/* Saved interrupt mask		*/
	//struct	procent *prptr;		/* Ptr to process' table entry	*/
	struct	rwlockentry *rwlockptr;		/* Ptr to sempahore table entry	*/

	mask = disable();
	if (isbadrwlock(rw)) {
		restore(mask);
		return SYSERR;
	}
	rwlockptr = &rwlocktab[rw];
	if (rwlockptr->state == S_FREE) {
		restore(mask);
		return SYSERR;
	}

	// spoop yourself out of the queue
	pid32 qwriter = dequeue(rwlockptr->queue);
	if(qwriter != currpid){
		kprintf("ERROR living writer is not at front of queue\n");
	}

	qid16 nextPid = firstid(rwlockptr->queue);
	if(queuetab[nextPid].qkey == WRITER){
		// unwait a writer, but don't remove it from the queue
		ready((pid32) nextPid);
	}else{
		while(queuetab[nextPid].qkey == READER){
			ready((pid32) nextPid);
			dequeue(rwlockptr->queue);
			nextPid = firstid(rwlockptr->queue);
		}
	}

	restore(mask);
	return OK;
}
