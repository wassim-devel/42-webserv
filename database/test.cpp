#include <iostream>

class Goat {
public:
	std::string getGoat() const
	{
		return _hello;
	}
private:
	std::string _hello;
};

int main()
{
	Goat* yes = NULL;

	std::cout << yes->getGoat() << std::endl;
}

-----------------------------26876495153802303899661974926--
