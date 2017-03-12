#include "glyphrenderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

GlyphRenderer::GlyphRenderer()
{

}

GlyphRenderer::~GlyphRenderer()
{

}

void GlyphRenderer::draw()
{
    this->bindVAO();

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, m_verticesCount);

    this->releaseVAO();
}
