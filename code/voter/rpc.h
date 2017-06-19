#ifndef RPC_H
#define RPC_H

enum Operation {
	REQUEST = 0,
	RESPONSE = 1,
};

struct RPCRequest {
	enum Operation op;
	int id;
	int procedure;
	void* params;
};

struct RPCReply {
	enum Operation op;
	int id;
	int error;
	void* params;
};
#endif
