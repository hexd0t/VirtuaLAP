#include <iostream>
#include "../Global/Core.h"
#include "UI.h"

using std::placeholders::_1;
int main() {
    try {
        std::cout << "VirtuaLAP Desktop, starting..." << std::endl;

        UI ui;
        ui.Init();

        Core app(std::bind(&UI::CaptureImage, &ui, _1));

        app.StartPipeline();
        ui.Run(std::bind(&Core::Step, &app));

        ui.Deinit();
        return 0;
    }
    catch (std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown unhandled exception, aborting" << std::endl;
        return 2;
    }
}