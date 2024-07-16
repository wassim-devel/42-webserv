#include "../includes/WebServ.hpp"

int main(int argc, char **argv)
{
	std::ifstream config;

	if (argc != 2)
		return (1);
	config.open(argv[1]);
	try
	{
		WebServ ws(argv[1]);
		ws.start();
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}
}
