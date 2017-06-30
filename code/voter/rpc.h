#ifndef RPC_H
#define RPC_H

enum Procedure {
	NONE = 0,
	MOVE_ROBOT = 0x10000000,
	CREATE_ROBOT,
};

/**
 * @brief creates an empty rpc context
 *
 * @return NULL on error
 */
void* createRPCContext(void);

/**
 * @brief parses an rpc message and calls the appropiate function
 *
 * @param rpc: the rpc context
 * @param buf: the message
 * @param len: the length of the message
 *
 * @return < 0 on error.
 * 	-1 == procedure not found
 */
int handleRPC(void* rpc, char* buf, size_t len);

typedef void (* TypeRPCProcedure)(int* params);
/**
 * @brief adds a procedure to the rpc context, so it can be called remotely.
 *
 * @param rpc: the rpc context
 * @param num: the id identifiying the procedure
 * @param proc: the procedure itself
 *
 * @return <0 on error
 */
int addProcedure(void* rpc, enum Procedure num, TypeRPCProcedure proc);

/**
 * @brief write a RPC message to outBuffer
 *
 * @param num: the procedure to be called
 * @param params: the arguments for that call
 * @param paramsLen: how many arguments there are
 * @param outBuffer: where the message shall be written. Don't forget to free() it!
 * @param outBufferLen: length of the message.
 *
 * @return <0 on error
 */
int createRPCRequest(enum Procedure num, int* params, size_t paramsLen, void* outBuffer, size_t* outBufferLen);
#endif
