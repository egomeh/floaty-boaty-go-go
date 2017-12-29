#include "definitions.hpp"

#include <string>
#include <sstream>
#include <iostream>

#include <thread>

#include "util.hpp"
#include "application.hpp"

#ifdef GUIONLY
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
#else
int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
#endif
{
    Application application(hInstance);
    application.Initialize();
    application.Run();

    return 0;
}
