#include <iostream>
#include "../Global/Core.h"
#include "UI.h"

using namespace std::placeholders;
int main() {
    //try {
        std::cout << "VirtuaLAP Desktop, starting..." << std::endl;

        UI ui;

        Core app(std::bind(&UI::CaptureImage, &ui, _1));

        ui.Init(app.GetCallbacks());
        app.Init();

        app.StartPipeline();
        ui.Run(std::bind(&Core::Step, &app, _1));

        ui.Deinit();
        return 0;
    /*}
    catch (std::exception& ex) {
        std::cerr << "Unhandled exception: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown unhandled exception, aborting" << std::endl;
        return 2;
    }/**/
}