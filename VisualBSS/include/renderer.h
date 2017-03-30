#ifndef RENDER_H
#define RENDER_H

#include <QOpenGLFunctions>


namespace bss {
class Renderer;
}

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;


/**
 * @brief The Renderer class Abstract class to draw stuffs with OpenGL.
 */
class Renderer : public QOpenGLFunctions
{
public:
    /**
     * @brief sendData
     * @param data
     * @param verticesCount
     */
    void sendData(const QVector<float>& data, unsigned int verticesCount);

    /**
     * @brief createVAO If VAO is not created, this function initialize it.
     */
    void createVAO();

    /**
     * @brief initGLFunc Initialize OpenGL fuctions.
     */
    void initGLFunc();

protected:
    Renderer();
    virtual ~Renderer();

    /**
     * @brief draw Abstract function, has no implementation and dedicated to
     * draw with OpenGL functions.
     */
    virtual void draw() = 0;

    /**
     * @brief bindVAO Bind VAO.
     */
    void bindVAO();

    /**
     * @brief releaseVAO Release VAO.
     */
    void releaseVAO();

protected:
    /**
     * @brief m_VAO VertexArrayObject.
     */
    QOpenGLVertexArrayObject* m_VAO;

    /**
     * @brief m_VBO VertexBufferObject.
     */
    QOpenGLBuffer* m_VBO;

    /**
     * @brief m_verticesCount Number of vertices to be drawn.
     */
    unsigned int m_verticesCount;
};

#endif // RENDER_H
