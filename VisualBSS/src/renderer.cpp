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

#include "renderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>


Renderer::Renderer()
{
    m_verticesCount = 0;
    m_VAO = Q_NULLPTR;
    m_VBO = Q_NULLPTR;
}

Renderer::~Renderer()
{
    if (m_VAO && m_VAO->isCreated())
        m_VAO->destroy();

    if (m_VBO && m_VBO->isCreated())
        m_VBO->destroy();

    delete m_VAO;
    delete m_VBO;
}

void Renderer::initGLFunc()
{
    initializeOpenGLFunctions();
}

void Renderer::sendData(const QVector<float> &data, unsigned int verticesCount)
{
    m_verticesCount = verticesCount;

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());
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
