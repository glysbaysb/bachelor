/**
 * @file The connection between simulation and voter is not affected by the
 * network fault injector. However the fault model specifies that there are
 * some possible failures, namely a world status gets dropped, duplicated or
 * modifed.
 *
 * The probabilities for those should be up to the user. To hide this from the
 * students (and only have to parse the fault injector config in one place) the
 * actual values are retrived from the simulation.
 */
#ifndef INCLUDE_FICFG
#define INCLUDE_FICFG
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This structure holds the probabilities for each possible failure between
 * voter and simulation.
 *
 * Due to limitations in the RPC library all values are ints. So to get the
 * actual probability you have to divide 1 by the respective value.
 * However the values can be used directly as: (rand() % X) == 0
 */
struct FiCfg {
	int dropWorldStatus, //! probability is 1 in dropWorldStatus
		   fakeWorldStatus, //! probability is 1 in fakeWorldStatus
		   dupWorldStatus; //! probability is 1 in dupWorldStatus
};

#ifdef __cplusplus
}
#endif
#endif
