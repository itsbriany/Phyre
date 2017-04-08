#pragma once
#include <string>

namespace Phyre {
namespace Graphics {
        
/**
 * \brief The base class from which all Vulkan applications should inherit from
 */
class Application {
public:
    //---------------------- Construction/Destruction ----------------
    Application();
    virtual ~Application();

    //---------------------- Window Handles --------------------------

    /**
     * \brief Called when the window's framebuffer gets resized
     */
    virtual void OnFramebufferResize(int width, int height) = 0;

private:
    //---------------------- Logging Helper --------------------------
    static const std::string kWho;
};

}
}
