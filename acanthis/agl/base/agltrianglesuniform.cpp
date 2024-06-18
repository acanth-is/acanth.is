// SPDX-FileCopyrightText: 2017 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "agltrianglesuniform.h"

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
 * @brief GLTrianglesUniform::GLTrianglesUniform
 * This class is an OpenGL representation of a set of triangles of uniform colour
 */

AGLTrianglesUniform::AGLTrianglesUniform() : m_count(0), m_program(0) {}

void AGLTrianglesUniform::loadTriangleData(const std::vector<Point2f> &points,
                                           const QRgb &polyColour) {
    m_built = false;

    m_count = 0;
    m_data.resize(static_cast<qsizetype>(points.size() * static_cast<size_t>(DATA_DIMENSIONS)));

    for (auto &point : points) {
        add(QVector3D(static_cast<float>(point.x), static_cast<float>(point.y), 0.0f));
    }
    m_colour.setX(static_cast<float>(qRed(polyColour)) / 255.0f);
    m_colour.setY(static_cast<float>(qGreen(polyColour)) / 255.0f);
    m_colour.setZ(static_cast<float>(qBlue(polyColour)) / 255.0f);
}

void AGLTrianglesUniform::setupVertexAttribs() {
    m_vbo.bind();
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                             DATA_DIMENSIONS * static_cast<GLsizei>(sizeof(GLfloat)), 0);
    m_vbo.release();
}

void AGLTrianglesUniform::initializeGL(bool core) {
    if (m_data.size() == 0)
        return;
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       core ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                       core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->link();

    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_colourVectorLoc = m_program->uniformLocation("colourVector");

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(constData(), m_count * static_cast<GLsizei>(sizeof(GLfloat)));

    setupVertexAttribs();
    m_program->setUniformValue(m_colourVectorLoc, m_colour);
    m_program->release();
    m_built = true;
}

void AGLTrianglesUniform::updateGL(bool m_core) {
    if (m_program == 0) {
        // has not been initialised yet, do that instead
        initializeGL(m_core);
    } else {
        m_vbo.bind();
        m_vbo.allocate(constData(), m_count * static_cast<GLsizei>(sizeof(GLfloat)));
        m_vbo.release();
        m_built = true;
    }
}

void AGLTrianglesUniform::updateColour(const QRgb &polyColour) {
    m_colour.setX(static_cast<float>(qRed(polyColour)) / 255.0f);
    m_colour.setY(static_cast<float>(qGreen(polyColour)) / 255.0f);
    m_colour.setZ(static_cast<float>(qBlue(polyColour)) / 255.0f);
    m_program->bind();
    m_program->setUniformValue(m_colourVectorLoc, m_colour);
    m_program->release();
}

void AGLTrianglesUniform::cleanup() {
    if (!m_built)
        return;
    m_vbo.destroy();
    delete m_program;
    m_program = 0;
}

void AGLTrianglesUniform::paintGL(const QMatrix4x4 &mProj, const QMatrix4x4 &mView,
                                  const QMatrix4x4 &mModel) {
    if (!m_built)
        return;
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    m_program->setUniformValue(m_projMatrixLoc, mProj);
    m_program->setUniformValue(m_mvMatrixLoc, mView * mModel);

    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    glFuncs->glDrawArrays(GL_TRIANGLES, 0, vertexCount());

    m_program->release();
}

void AGLTrianglesUniform::add(const QVector3D &v) {
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    m_count += DATA_DIMENSIONS;
}
