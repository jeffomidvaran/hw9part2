/* C interface for program benchmark timer management. */
/* Note you must split this file into Timer.h and Timer.c */
/* Your program includes Timer.h and must link-in Timer.o */

/* extern int gettimeofday(struct timeval *tp, void *tzp); */
/* extern int getrusage(int who, struct rusage *rusag); */

/* Timer.h */
	int Timer_start();
	int Timer_elapsedWallclockTime(double *wc);
	int Timer_elapsedUserTime(double *ut);
	int Timer_elapsedSystemTime(double *sys);
	int Timer_elapsedTime(double *wc, double *us, double *sys);
