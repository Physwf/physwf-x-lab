#include <thread>
#include <chrono>

extern void socket_client();
extern void socket_server();

int main(int argc, char* argv[])
{
	std::thread server_thread(socket_server);
	server_thread.detach();
	std::this_thread::sleep_for(std::chrono::seconds(10));
	socket_client();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return 0;
}