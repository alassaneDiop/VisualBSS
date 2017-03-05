#ifndef RENDER_H
#define RENDER_H

#include <QOpenGLFunctions>

namespace bss {
class Renderer;
}

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

class Renderer : public QOpenGLFunctions
{

protected:
    Renderer();

    virtual ~Renderer();

    virtual void draw() = 0;

public:

protected:
    void bindVAO();
    void releaseVAO();
    QOpenGLVertexArrayObject*   m_VAO;
    QOpenGLBuffer*              m_VBO;

    unsigned int                m_verticesCount;
};

#endif // RENDER_H
