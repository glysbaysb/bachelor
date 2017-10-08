#ifndef RPC_H
#define RPC_H

#include <msgpack.h>

#ifdef __cplusplus
extern "C" {
#endif

enum Procedure {
	NONE = 0,
	MOVE_ROBOT = 0x10000000,
	CREATE_ROBOT,
	VOTE_MOVE_ROBOT,
	ECHO = 0x20000000,
};

/**
 * @brief creates an empty rpc context
 *
 * @return NULL on error
 */
void* createRPCContext(void);

/**
 * @brief frees that context.
 *
 * DO NOT UNDER ANY CIRCUMSTANCE free an RPC context while it is still used.
 * Or, to put it another way: this library is not threadsafe
 */
void destroyRPCContext(void*);

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
int handleRPC(void* rpc, const unsigned char* buf, const size_t len);

typedef void (* TypeRPCProcedure)(void* optional, msgpack_object_array* params);
/**
 * @brief adds a procedure to the rpc context, so it can be called remotely.
 *
 * @param rpc: the rpc context
 * @param num: the id identifiying the procedure
 * @param proc: the procedure itself
 * @param optional: will be passed to the procedure.
 *
 * @return <0 on error
 */
int addProcedure(void* rpc, const enum Procedure num, const TypeRPCProcedure proc, void* optional);

/**
 * @brief write a RPC message to outBuffer
 *
 * @param rpc: the rpc context
 * @param num: the procedure to be called
 * @param params: the arguments for that call. Maybe be NULL if no params shall be passed.
 *                The library expects this to be a valid msgpack array
 * @param paramsLen: how long the arguments are
 * @param outBuffer: where the message shall be written. Don't forget to free() it!
 * @param outBufferLen: length of the message.
 *
 * @return <0 on error
 */
int createRPCRequest(void* rpc, const enum Procedure num, const void* paramsBuffer, size_t paramsLen, void** outBuffer, size_t* outBufferLen);

typedef struct RPCProcedure {
	enum Procedure num;
	TypeRPCProcedure proc;
	void* optional;
} RPCProcedure;
/**
 * @brief gets a list of the registered procedures
 *
 * @param rpc_: the RPC context
 * @param arr: where the list shall be written
 * @param sizeOfArr: how many elements may be written
 *
 * @return: -1 if the output param is too small, 0 on success
 */
int getRegisteredProcedures(const void* rpc_, RPCProcedure* arr, const size_t sizeOfArr);

typedef struct RPCInFlight {
	int id;
	RPCProcedure proc;
} RPCInFlight;
/**
 * @brief gets a list of the requests still in flight
 * (i.e. handleRPC() has not been called for the reply)
 *
 * @param rpc_: the RPC context
 * @param arr: where the list shall be written
 * @param sizeOfArr: how many elements may be written
 *
 * @return: -1 if the output param is too small, 0 on success
 */
int getRPCsInFlight(const void* rpc_, RPCInFlight* arr, const size_t sizeOfArr);

#ifdef __cplusplus
}
#endif

#endif
