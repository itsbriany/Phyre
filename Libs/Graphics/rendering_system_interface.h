#pragma once

namespace Phyre {
namespace Graphics {

class RenderingSystemInterface {
public:
    virtual ~RenderingSystemInterface() { }
    
    // Throws a runtime exception if the rendering system failed to start correctly
    virtual void Start() = 0;
};

}
}
