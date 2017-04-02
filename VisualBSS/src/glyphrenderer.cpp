/* Copyright 2017
 * All rights reserved to "Université de Bordeaux"
 *
 * This file is part of VisualBSS.
 * VisualBSS is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VisualBSS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with VisualBSS.  If not, see <http://www.gnu.org/licenses/>.
 * */

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

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, m_verticesCount);

    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->releaseVAO();
}
