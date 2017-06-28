// SERVER
	rpc_server* s = new_rpc_server();

	//typedef (rpcfunc)(int* arr, int arrSize);
	s->register(MOVE, &move);//void ::register(int id, typedef);

	s->listen(sock1);
	s->listen(sock2);

	s->run();



// CLIENT
{
	rpc_client* rpc = new_rpc_client();
	?? ret = rpc->call(CREATE);
	rpc->call(MOVE, ID, 1, 1);

	--> Fazit: in python waere schoener
