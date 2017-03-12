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
public:
    void sendData(const QVector<float>& data, unsigned int verticesCount);
    void createVAO();
    void initGLFunc();

protected:
    Renderer();
    virtual ~Renderer();
    virtual void draw() = 0;

    void bindVAO();
    void releaseVAO();

protected:
    QOpenGLVertexArrayObject*   m_VAO;
    QOpenGLBuffer*              m_VBO;

    unsigned int                m_verticesCount;
};

#endif // RENDER_H
