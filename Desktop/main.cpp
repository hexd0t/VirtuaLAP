#include <iostream>
#include "../Global/Core.h"
#include "UI.h"

using namespace std::placeholders;
int main() {
    try {
        std::cout << "VirtuaLAP Desktop, starting..." << std::endl;

        UI ui;

        Core app(std::bind(&UI::CaptureImage, &ui, _1));
        auto resizecallback = std::bind(&Core::FramebufferSizeChanged, &app, _1, _2);
        auto drawuiwindowcallback = std::bind(&Core::DrawUIWindow, &app, _1, _2, _3, _4, _5);
        ui.Init(resizecallback, drawuiwindowcallback);
        app.Init();

        app.StartPipeline();
        ui.Run(std::bind(&Core::Step, &app, _1));

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