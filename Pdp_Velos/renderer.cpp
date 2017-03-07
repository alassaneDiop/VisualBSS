#include "renderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

Renderer::Renderer()
{
    m_VBO = new QOpenGLBuffer;
    m_VBO->create();
    m_VBO->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_VBO->bind();

    m_VAO = new QOpenGLVertexArrayObject;
    m_VAO->create();
    m_VAO->bind();

    m_VAO->release();
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

void Renderer::bindVAO()
{
    m_VAO->bind();
}

void Renderer::releaseVAO()
{
    m_VAO->release();
}
