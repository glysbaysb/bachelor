#include <iostream>
#include <fstream>
#include "json.hpp"

// for convenience
using json = nlohmann::json;
#include "ficfg.h"

struct FiCfg ficfg_parse(const char* file) {
	std::ifstream i(file);
	json j;
	i >> j;

	FiCfg a;
	a.breakEngineA = *j.find("breakEngineA");
	a.stuckAtEngineA = *j.find("stuckAtEngineA");
	a.breakEngineB = *j.find("breakEngineB");
	a.stuckAtEngineB = *j.find("stuckAtEngineB");

	a.dropWorldStatus = *j.find("dropWorldStatus");
	a.fakeWorldStatus = *j.find("fakeWorldStatus");
	a.dupWorldStatus = *j.find("dupWorldStatus");

	return a;
}
