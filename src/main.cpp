#include "ui/App.hpp"

#if defined(_WIN32)
#include <windows.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    AppUI app;
    return app.run();
}
#else
int main() {
    AppUI app;
    return app.run();
}
#endif
