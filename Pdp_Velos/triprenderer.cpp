#include "triprenderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>

TripRenderer::TripRenderer()
{

}

TripRenderer::~TripRenderer()
{

}

void TripRenderer::draw()
{
    this->bindVAO();

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, m_verticesCount);

    this->releaseVAO();
}
