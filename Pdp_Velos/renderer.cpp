#include "renderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>


Renderer::Renderer()
{
    m_VAO = Q_NULLPTR;
    m_VBO = Q_NULLPTR;
}

Renderer::~Renderer()
{
    if (m_VBO && m_VAO->isCreated())
        m_VAO->destroy();

    if (m_VAO && m_VBO->isCreated())
        m_VBO->destroy();

    if (m_VAO)
        delete m_VAO;

    if (m_VBO)
        delete m_VBO;
}

void Renderer::initGLFunc()
{
    initializeOpenGLFunctions();
}

void Renderer::sendData(const QVector<float> &data, unsigned int verticesCount)
{
    initializeOpenGLFunctions();
    m_verticesCount = verticesCount;

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::createVAO()
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

void Renderer::bindVAO()
{
    if (m_VAO->isCreated())
        m_VAO->bind();
}

void Renderer::releaseVAO()
{
    if (m_VAO->isCreated())
        m_VAO->release();
}
