#include <msgpack.h>

int main(int argc, char** argv) {
	FILE* fbuf;
	msgpack_packer pk;
	msgpack_sbuffer sbuf;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, &msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 4);
	msgpack_pack_int32(&pk, 0); // operation
	msgpack_pack_int32(&pk, 0x1234ABCD); // id
	msgpack_pack_int32(&pk, 42); // error
	msgpack_pack_array(&pk, 1);

	msgpack_pack_str(&pk, 6);
	msgpack_pack_str_body(&pk, "Hello", 6);

	fbuf = fopen("rpcreply.bin", "w");
	fwrite(sbuf.data, 1, sbuf.size, fbuf);
	fclose(fbuf);
}
