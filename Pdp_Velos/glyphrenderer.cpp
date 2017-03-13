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

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glDrawArrays(GL_LINES, 0, m_verticesCount);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->releaseVAO();
}
