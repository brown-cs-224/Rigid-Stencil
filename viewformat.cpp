#include "viewformat.h"

ViewFormat::ViewFormat()
{
    setVersion(4, 3);
    setProfile(QGLFormat::CompatibilityProfile);
    setStencil(true);
    setStencilBufferSize(8);
    setSampleBuffers(true);
}
