#ifdef __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <time.h>

// Maximum length of last message
#define MINUNIT_MESSAGE_LEN 1024
// Do not change
#define MINUNIT_NSECS 1000000000
#define EPSILON 1E-12

// Misc. counters
static int minunit_run = 0;
static int minunit_assert = 0;
static int minunit_fail = 0;

// Timers
typedef struct timespec timespec_t ;
static timespec_t minunit_real_timer = { 0, 0 };
static timespec_t minunit_proc_timer = { 0, 0 };

// Last message
static char minunit_last_message[MINUNIT_MESSAGE_LEN];

// Test setup and teardown function pointers
static void (*minunit_setup)(void) = NULL;
static void (*minunit_teardown)(void) = NULL;

// Definitions
#define MU_TEST(method_name) static int method_name()
#define MU_TEST_SUITE(suite_name) static void suite_name()

// Configure setup and teardown functions
#define MU_SUITE_CONFIGURE(setup_fun, teardown_fun) \
	minunit_setup = setup_fun; \
	minunit_teardown = teardown_fun;

// Test runner
#define MU_RUN_TEST(test) do {\
		if (minunit_real_timer.tv_sec==0 && minunit_real_timer.tv_nsec==0) {\
			clock_gettime(CLOCK_MONOTONIC, &minunit_real_timer);\
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &minunit_proc_timer);\
		}\
		if (minunit_setup) (*minunit_setup)();\
		int fail = test();\
		minunit_run++;\
		if (fail) {\
			minunit_fail++;\
			printf("F");\
			printf("\n%s\n", minunit_last_message);\
		} else {\
			printf(".");\
		}\
		fflush(stdout);\
		if (minunit_teardown) (*minunit_teardown)();\
	} while (0)

// Report
#define MU_REPORT() do {\
	printf("\n\n%d tests, %d assertions, %d failures\n", minunit_run, minunit_assert, minunit_fail);\
	timespec_t end_real_timer, end_proc_timer;\
	clock_gettime(CLOCK_MONOTONIC, &end_real_timer);\
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_proc_timer);\
	printf("\nFinished in %.8f seconds (real) %.8f seconds (proc)\n\n",\
		mu_timer_diff(&minunit_real_timer, &end_real_timer),\
		mu_timer_diff(&minunit_proc_timer, &end_proc_timer));\
	} while (0)

// Assertions
#define MU_ASSERT(test, message) do {\
		minunit_assert++;\
		if (!(test)) {\
			snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, message);\
			return 1;\
		}\
	} while (0)

#define MU_ASSERT_EQ(expected, result) do {\
		minunit_assert++;\
		int e = (expected);\
		int r = (result);\
		if (e != r) {\
			snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %d expected but was %d", __func__, __FILE__, __LINE__, e, r);\
			return 1;\
		}\
	} while (0)

#define MU_ASSERT_DOUBLE_EQ(expected, result) do {\
		minunit_assert++;\
		double e = (expected);\
		double r = (result);\
		if (fabs(e-r) > EPSILON) {\
			snprintf(minunit_last_message, MINUNIT_MESSAGE_LEN, "%s failed:\n\t%s:%d: %g expected but was %g", __func__, __FILE__, __LINE__, e, r);\
			return 1;\
		}\
	} while (0)

// Misc. utilities
static double mu_timer_diff(timespec_t *start, timespec_t *end)
{
	timespec_t diff;
	if ((end->tv_nsec - start->tv_nsec) < 0) {
		diff.tv_sec = end->tv_sec - start->tv_sec - 1;
		diff.tv_nsec = MINUNIT_NSECS + end->tv_nsec - start->tv_nsec;
	} else {
		diff.tv_sec = end->tv_sec - start->tv_sec;
		diff.tv_nsec = end->tv_nsec - start->tv_nsec;
	}
	return (double)diff.tv_sec + (double)diff.tv_nsec/MINUNIT_NSECS;
}

#ifdef __cplusplus
}
#endif

