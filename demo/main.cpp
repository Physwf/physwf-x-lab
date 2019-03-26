
extern void socket_client();
extern void socket_server();

int main(int argc, char* argv[])
{
	socket_server();
	socket_client();
	return 0;
}