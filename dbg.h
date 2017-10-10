#ifndef dbg_h
#define dbg_h
#if DEBUG == 1
#define debug(message, ...) printf("DEBUG: IN FILE %s, LINE %d, " message "\n",\
						__FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug(message, ...)
#endif
#define check(A, R, M, ...) if (A) { fprintf(stderr, "CHECK FAILED. " M "\n", ##__VA_ARGS__);\
				return R; }
#endif
