#include "selectorrenderer.h"


#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>
#include <QRectF>

#include <QDebug>

SelectorRenderer::SelectorRenderer()
{
    m_verticesCount = 12;
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

void SelectorRenderer::updateData(const QRectF& rect)
{
    QVector<float> data;
    data += QVector<float>({ (float)rect.topLeft().x(), (float)-rect.topLeft().y(), (float)rect.bottomRight().x() });
    data += QVector<float>({ (float)-rect.topLeft().y(), (float)rect.topLeft().x(), (float)-rect.bottomRight().y() });
    data += QVector<float>({ (float)rect.topLeft().x(), (float)-rect.bottomRight().y(), (float)rect.bottomRight().x() });
    data += QVector<float>({ (float)-rect.bottomRight().y(), (float)rect.bottomRight().x(), (float)-rect.topLeft().y() });

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
