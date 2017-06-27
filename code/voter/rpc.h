#ifndef RPC_H
#define RPC_H

enum Operation {
	REQUEST = 0,
	RESPONSE = 1,
};

enum Procedure {
	NONE = 0,
	MOVE_ROBOT = 0x10000000,
	CREATE_ROBOT,
};

struct RPCRequest {
	enum Operation op;
	int id;
	enum Procedure procedure;
	void* params;
};

struct RPCReply {
	enum Operation op;
	int id;
	int error;
	void* params;
};
#endif
