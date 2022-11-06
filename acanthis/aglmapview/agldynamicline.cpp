// Copyright (C) 2017, Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "agldynamicline.h"
/**
 * @brief GLDynamicLine::GLDynamicLine
 * This class is an OpenGL representation of a dynamic line. See GLDynamicRect
 * to understand the details of the implementation. This class only uses the
 * diagonal points of the GLDynamicRect to make a line
 */

AGLDynamicLine::AGLDynamicLine() {
    m_count = 0;
    m_program = 0;
    m_data.resize(2);

    add(0);
    add(3);
}

void AGLDynamicLine::paintGL(const QMatrix4x4 &mProj, const QMatrix4x4 &mView,
                             const QMatrix4x4 &mModel) {
    if (!m_built)
        return;
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, mProj);
    m_program->setUniformValue(m_mvMatrixLoc, mView * mModel);
    m_program->setUniformValue(m_diagVertices2DLoc, m_selectionBounds);

    m_program->setUniformValue(m_colourVectorLoc, m_colour_stroke);
    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    glFuncs->glDrawArrays(GL_LINE_LOOP, 0, vertexCount());

    m_program->release();
}
