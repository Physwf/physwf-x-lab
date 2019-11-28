#include <thread>
#include <chrono>

extern void socket_client();
extern void socket_server();

int __main(int argc, char* argv[])
{
	std::thread server_thread(socket_server);
	server_thread.detach();
	std::this_thread::sleep_for(std::chrono::seconds(5));
	socket_client();
	return 0;
}
