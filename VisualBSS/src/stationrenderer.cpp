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

#include "stationrenderer.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>
#include <QtMath>


StationRenderer::StationRenderer()
{

}

StationRenderer::~StationRenderer()
{

}

void StationRenderer::draw()
{
    this->bindVAO();

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->bufferId());

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glPointSize(10.f);
    glDrawArrays(GL_POINTS, 0, m_verticesCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    this->releaseVAO();
}

void StationRenderer::drawFilledCircle(unsigned int triangleAmount, unsigned int radius)
{
    QVector<float> vertices;

    for (int i = 0; i < (int)triangleAmount; ++i)
    {
        vertices.append(radius * qCos(2.f * M_PI / triangleAmount));
        vertices.append(radius * qSin(2.f * M_PI / triangleAmount));
    }

    m_VBO->bind();
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    m_VBO->release();

    this->bindVAO();

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO->IndexBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    glDrawArrays(GL_POINTS, 0, triangleAmount);

    this->releaseVAO();
}

