#include "renderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>


Renderer::Renderer()
{
    m_VBO = new QOpenGLBuffer;
    m_VBO->create();
    m_VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_VBO->bind();

    m_VAO = new QOpenGLVertexArrayObject;
    m_VBO->release();
}

Renderer::~Renderer()
{
    if (m_VBO->isCreated())
        m_VBO->destroy();
    if (m_VAO->isCreated())
        m_VAO->destroy();

    if (m_VAO)
        delete m_VAO;
    if (m_VBO)
        delete m_VBO;
}

void Renderer::sendData(const QVector<float> &data, unsigned int verticesCount)
{
    initializeOpenGLFunctions();
    m_verticesCount = verticesCount;

//    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

void Renderer::bindVAO()
{
    if (!m_VAO->isCreated())
    {
        m_VAO->create();
        m_VAO->bind();
        m_VBO->bind();
        m_VBO->release();
        m_VAO->release();
    }
    m_VAO->bind();
}

void Renderer::releaseVAO()
{
    if (m_VAO->isCreated())
        m_VAO->release();
}
