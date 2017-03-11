#include "selectorrenderer.h"


#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>

SelectorRenderer::SelectorRenderer()
{

}

SelectorRenderer::~SelectorRenderer()
{

}

void SelectorRenderer::draw()
{
    this->bindVAO();

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);

    this->releaseVAO();
}

void SelectorRenderer::updateData(const QVector<float> &data)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
}

void SelectorRenderer::prepareData(unsigned int verticesCount, unsigned int dataCount)
{
    initializeOpenGLFunctions();
    m_verticesCount = verticesCount;

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, dataCount * sizeof(float), NULL, GL_STATIC_DRAW);
}
