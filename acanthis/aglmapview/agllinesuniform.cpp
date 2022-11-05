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

#include "agllinesuniform.h"

#include <math.h>

static const char *vertexShaderSourceCore = // auto-format hack
    "#version 150\n"
    "in vec4 vertex;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSourceCore = // auto-format hack
    "#version 150\n"
    "uniform vec4 colourVector;\n"
    "out highp vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = colourVector;\n"
    "}\n";

static const char *vertexShaderSource = // auto-format hack
    "attribute vec4 vertex;\n"
    "uniform mat4 projMatrix;\n"
    "uniform mat4 mvMatrix;\n"
    "void main() {\n"
    "   gl_Position = projMatrix * mvMatrix * vertex;\n"
    "}\n";

static const char *fragmentShaderSource = // auto-format hack
    "uniform highp vec4 colourVector;\n"
    "void main() {\n"
    "   gl_FragColor = colourVector;\n"
    "}\n";

/**
 * @brief GLLinesUniform::GLLinesUniform
 * This class is an OpenGL representation of  multiple lines of uniform colour
 */

AGLLinesUniform::AGLLinesUniform() : m_count(0), m_program(0) {}

void AGLLinesUniform::loadLineData(const std::vector<SimpleLine> &lines, const QColor &lineColour) {
    m_built = false;

    m_count = 0;
    m_data.resize(lines.size() * 2 * DATA_DIMENSIONS);

    for (auto &line : lines) {
        add(QVector3D(line.start().x, line.start().y, 0.0f));
        add(QVector3D(line.end().x, line.end().y, 0.0f));
    }
    m_colour.setX(lineColour.redF());
    m_colour.setY(lineColour.greenF());
    m_colour.setZ(lineColour.blueF());
    m_colour.setW(lineColour.alphaF());
}

void AGLLinesUniform::setupVertexAttribs() {
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, DATA_DIMENSIONS * sizeof(GLfloat), 0);
    m_vbo.release();
}

void AGLLinesUniform::initializeGL(bool coreProfile) {
    if (m_data.size() == 0)
        return;
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       coreProfile ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(
        QOpenGLShader::Fragment, coreProfile ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_colourVectorLoc = m_program->uniformLocation("colourVector");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(constData(), m_count * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    setupVertexAttribs();
    m_program->setUniformValue(m_colourVectorLoc, m_colour);
    m_program->release();
    m_built = true;
}

void AGLLinesUniform::updateGL(bool coreProfile) {
    if (m_program == 0) {
        // has not been initialised yet, do that instead
        initializeGL(coreProfile);
    } else {
        m_vbo.bind();
        m_vbo.allocate(constData(), m_count * sizeof(GLfloat));
        m_vbo.release();
        m_built = true;
    }
}

void AGLLinesUniform::updateColour(const QColor &lineColour) {
    m_colour.setX(lineColour.redF());
    m_colour.setY(lineColour.greenF());
    m_colour.setZ(lineColour.blueF());
    m_program->bind();
    m_program->setUniformValue(m_colourVectorLoc, m_colour);
    m_program->release();
}

void AGLLinesUniform::cleanup() {
    if (!m_built)
        return;
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
}

void AGLLinesUniform::paintGL(const QMatrix4x4 &m_mProj, const QMatrix4x4 &m_mView,
                              const QMatrix4x4 &m_mModel) {
    if (!m_built)
        return;
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, m_mProj);
    m_program->setUniformValue(m_mvMatrixLoc, m_mView * m_mModel);

    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    glFuncs->glDrawArrays(GL_LINES, 0, vertexCount());

    m_program->release();
}

void AGLLinesUniform::add(const QVector3D &v) {
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    m_count += DATA_DIMENSIONS;
}
