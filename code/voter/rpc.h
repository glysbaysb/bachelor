#ifndef RPC_H
#define RPC_H

enum Procedure {
	NONE = 0,
	MOVE_ROBOT = 0x10000000,
	CREATE_ROBOT,
};

void* createRPCContext(void);

int handleRPCReply(void* rpc, char* buf, size_t len);
#endif
