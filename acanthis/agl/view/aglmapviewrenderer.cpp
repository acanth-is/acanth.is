// SPDX-FileCopyrightText: 2021 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "aglmapviewrenderer.h"
#include "agl/viewmodel/aglmapviewmodel.h"
#include "aglmapviewport.h"

#include <QQuickOpenGLUtils>

void AGLMapViewRenderer::synchronize(QQuickFramebufferObject *item) {
    AGLMapViewport *glView = static_cast<AGLMapViewport *>(item);
    m_eyePosX = glView->getEyePosX();
    m_eyePosY = glView->getEyePosY();
    m_zoomFactor = glView->getZoomFactor();
    m_mouseDragRect = glView->getMouseDragRect();
    m_foregroundColour = glView->getForegroundColour();
    m_backgroundColour = glView->getBackgroundColour();
    m_backgroundColourChanged = true;
    recalcView();
}

AGLMapViewRenderer::AGLMapViewRenderer(const QQuickFramebufferObject *item,
                                       const GraphViewModel *graphViewModel,
                                       const QColor &foregrounColour,
                                       const QColor &backgroundColour, int antialiasingSamples,
                                       bool highlightOnHover)
    : m_item(static_cast<const AGLMapViewport *>(item)), m_foregroundColour(foregrounColour),
      m_backgroundColour(backgroundColour), m_model(new AGLMapViewModel(graphViewModel)),
      m_highlightOnHover(highlightOnHover), m_antialiasingSamples(antialiasingSamples) {

    if (!m_model->hasGraphViewModel())
        return;

    m_core = QCoreApplication::arguments().contains(QStringLiteral("--coreprofile"));
    if (m_antialiasingSamples) {
        QSurfaceFormat format;
        format.setSamples(m_antialiasingSamples);
    }

    loadAxes();

    m_model->loadGLObjects();

    m_dragLine.setStrokeColour(m_foregroundColour);
    m_selectionRect.setStrokeColour(m_backgroundColour);

    QQuickOpenGLUtils::resetOpenGLState();

    m_backgroundColourChanged = true;

    m_selectionRect.initializeGL(m_core);
    m_dragLine.initializeGL(m_core);
    m_axes.initializeGL(m_core);

    m_model->initializeGL(m_core);

    m_model->loadGLObjectsRequiringGLContext();

    m_mModel.setToIdentity();

    m_mView.setToIdentity();
    m_mView.translate(0, 0, -1);
}

AGLMapViewRenderer::~AGLMapViewRenderer() {
    m_selectionRect.cleanup();
    m_dragLine.cleanup();
    m_axes.cleanup();
    m_model->cleanup();
}

void AGLMapViewRenderer::render() {
    if (!m_item->getGraphViewModel().hasMetaGraph())
        return;

    if (!m_model->hasGraphViewModel())
        return;

    if (m_backgroundColourChanged) {
        // TODO: This should be happening in the ctor, however
        // this particular qt opengl implementation does not
        // work in that way, so we have to do it here
        glClearColor(m_backgroundColour.redF(), m_backgroundColour.greenF(),
                     m_backgroundColour.blueF(), 1);
        m_backgroundColourChanged = false;
    }

    glEnable(GL_MULTISAMPLE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_model->updateGL(m_core);

    m_axes.paintGL(m_mProj, m_mView, m_mModel);

    m_model->paintGL(m_mProj, m_mView, m_mModel);

    float pos[] = {
        float(std::min(m_mouseDragRect.bottomRight().x(), m_mouseDragRect.topLeft().x())),
        float(std::min(m_mouseDragRect.bottomRight().y(), m_mouseDragRect.topLeft().y())),
        float(std::max(m_mouseDragRect.bottomRight().x(), m_mouseDragRect.topLeft().x())),
        float(std::max(m_mouseDragRect.bottomRight().y(), m_mouseDragRect.topLeft().y()))};
    m_selectionRect.setSelectionBounds(QMatrix2x2(pos));
    m_selectionRect.paintGL(m_mProj, m_mView, m_mModel);

    //    if ((m_mouseMode & MOUSE_MODE_SECOND_POINT) == MOUSE_MODE_SECOND_POINT) {
    //        float pos[] = {float(m_tempFirstPoint.x), float(m_tempFirstPoint.y),
    //        float(m_tempSecondPoint.x),
    //                       float(m_tempSecondPoint.y)};
    //        m_dragLine.paintGL(m_mProj, m_mView, m_mModel, QMatrix2x2(pos));
    //    }

    QQuickOpenGLUtils::resetOpenGLState();
}

void AGLMapViewRenderer::recalcView() {
    GLfloat screenRatio =
        GLfloat(m_viewportSize.width()) / static_cast<float>(m_viewportSize.height());
    m_mProj.setToIdentity();

    if (m_perspectiveView) {
        m_mProj.perspective(45.0f, screenRatio, 0.01f, 100.0f);
        m_mProj.scale(1.0f, 1.0f, m_zoomFactor);
    } else {
        m_mProj.ortho(-m_zoomFactor * 0.5f * screenRatio, m_zoomFactor * 0.5f * screenRatio,
                      -m_zoomFactor * 0.5f, m_zoomFactor * 0.5f, 0, 10);
    }
    m_mProj.translate(m_eyePosX, m_eyePosY, 0.0f);
}

void AGLMapViewRenderer::loadAxes() {
    std::vector<std::pair<SimpleLine, PafColor>> axesData;
    axesData.push_back(std::pair<SimpleLine, PafColor>(SimpleLine(0, 0, 1, 0), PafColor(1, 0, 0)));
    axesData.push_back(std::pair<SimpleLine, PafColor>(SimpleLine(0, 0, 0, 1), PafColor(0, 1, 0)));
    m_axes.loadLineData(axesData);
}
