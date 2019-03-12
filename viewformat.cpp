#include "viewformat.h"

ViewFormat::ViewFormat()
{
    setVersion(4, 3);
    setProfile(QGLFormat::CoreProfile);
    setStencil(true);
    setStencilBufferSize(8);
    setSampleBuffers(true);
}
