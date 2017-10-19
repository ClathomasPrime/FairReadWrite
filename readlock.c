// readlock.c

#include <xinu.h>

/*------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
syscall	readlock(rwid32 rw) {
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

	rwlockptr->nReaders++;

	if(nonempty(rwlockptr->queue)){ //queue yourself
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;
		prptr->prrwlock = rw;		// Record rwlock ID
		enqueue(currpid,rwlockptr->queue);
		queuetab[currpid].qkey = READER;
		resched();
		// return here eventually
	}

	restore(mask);
	return OK;
}


/*------------------------------------------------------------------------
 *------------------------------------------------------------------------
 */
syscall	readunlock(rwid32 rw) {
	intmask mask;			/* Saved interrupt mask		*/
	// struct	procent *prptr;		/* Ptr to process' table entry	*/
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

	rwlockptr->nReaders--;

	if(rwlockptr->nReaders == 0 && nonempty(rwlockptr->queue)){
		// unwait a writer (first guy MUST be a writer
		// because as this is called, no writer is living)
		qid16 next = firstid(rwlockptr->queue); 
		ready((pid32)next);
		// ready the writer but do not remove it from the queue yet
	}

	restore(mask);
	return OK;
}
