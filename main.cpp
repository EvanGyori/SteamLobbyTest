#include "Manager.h"

int main()
{
    Manager manager;
    if (!manager.init()) {
	return 1;
    }
    manager.run();

    return 0;
}
