#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include "app.h"

int main()
{
	App application;

	try
	{
		application.run();
	}
	catch (const std::runtime_error& e)
	{
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
