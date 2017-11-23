#ifndef INCLUDE_FICFG
#define INCLUDE_FICFG

struct FiCfg {
	double breakEngineA,
		   stuckAtEngineA,
		   breakEngineB,
		   stuckAtEngineB;
	double dropWorldStatus,
		   fakeWorldStatus,
		   dupWorldStatus;
};
struct FiCfg ficfg_parse(const char* file);
#endif
