#ifndef INCLUDE_FICFG
#define INCLUDE_FICFG
#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif
