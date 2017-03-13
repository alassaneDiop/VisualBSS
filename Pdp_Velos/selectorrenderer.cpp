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

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);

    glDisableVertexAttribArray(0);
    this->releaseVAO();
}

void SelectorRenderer::updateData(const QVector<float> &data)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(GLfloat), data.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SelectorRenderer::prepareData(unsigned int verticesCount, unsigned int dataCount)
{
    initializeOpenGLFunctions();
    m_verticesCount = verticesCount;

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
    glBufferData(GL_ARRAY_BUFFER, dataCount * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
