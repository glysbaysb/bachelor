#!/bin/bash
build/tests/ecc/test_ecc || exit -1
build/tests/network/test_network || exit -1
build/tests/rpc/test_rpc || exit -1
build/tests/simulation/test_* || exit -1
build/tests/ficfg/test_ficfg || exit -1
build/tests/algo/test_algo || exit -1
