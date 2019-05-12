#include "Simulation.h"

#include <glad/glad.h> /* ToDo(AMü): Remove this?
 That would mean we need to wrap all Graphics-API calls in Platform-specific code
 Alternatively, use Preprocessor flags to include the correct headers [uglier, but probably easier] */

Simulation::Simulation() {

}

Simulation::~Simulation() {

}

void Simulation::Step(CameraImageData *camImage, ImageAnalysisResult *imgAnalysis, TrackGeometry *track) {
    glClearColor(0.8f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
