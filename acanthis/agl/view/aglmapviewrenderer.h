﻿// SPDX-FileCopyrightText: 2021 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "../base/agldynamicline.h"
#include "../base/agldynamicrect.h"
#include "../base/agllines.h"
#include "../viewmodel/aglviewmodel.h"

#include "graphviewmodel.h"

#include <QMatrix4x4>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLShaderProgram>
#include <QSettings>
#include <QtQuick/QQuickFramebufferObject>
#include <QtQuick/QQuickWindow>

class AGLMapViewport;
class AGLMapViewRenderer : public QQuickFramebufferObject::Renderer {

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        m_viewportSize = size;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(m_antialiasingSamples);
        return new QOpenGLFramebufferObject(size, format);
    }

    void synchronize(QQuickFramebufferObject *item) override;

  public:
    AGLMapViewRenderer(const QQuickFramebufferObject *item, const GraphViewModel *graphDocViewModel,
                       const QColor &foregrounColour, const QColor &backgroundColour,
                       int antialiasingSamples, bool highlightOnHover);
    ~AGLMapViewRenderer();

    void render() override;
    void update();

  private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program = nullptr;
    const AGLMapViewport *m_item;

    void recalcView();

    static QColor colorMerge(QColor color, QColor mergecolor) {
        return QColor::fromRgb((color.rgba() & 0x006f6f6f) | (mergecolor.rgba() & 0x00a0a0a0));
    }

    void notifyDatasetChanged() {
        m_datasetChanged = true;
        //        update();
    }

    QString m_currentFile;

    bool m_core;
    bool m_perspectiveView = false;
    float m_eyePosX;
    float m_eyePosY;
    float m_zoomFactor = 20;
    QMatrix4x4 m_mProj;
    QMatrix4x4 m_mView;
    QMatrix4x4 m_mModel;

    QColor m_foregroundColour;
    QColor m_backgroundColour;
    bool m_backgroundColourChanged = false;

    AGLDynamicRect m_selectionRect;
    AGLDynamicLine m_dragLine;
    AGLLines m_axes;

    std::unique_ptr<AGLViewModel> m_model;

    bool m_highlightOnHover = true;

    int m_antialiasingSamples = 0; // set this to 0 if rendering is too slow

    bool m_datasetChanged = false;

    void loadAxes();

    QRectF m_mouseDragRect = QRectF(0, 0, 0, 0);

    Point2f m_tempFirstPoint;
    Point2f m_tempSecondPoint;

    int m_currentlyEditingShapeRef = -1;

    Point2f m_polyStart;
    int m_polyPoints = 0;

    inline int PixelDist(QPoint a, QPoint b) {
        return (int)sqrt(
            double((b.x() - a.x()) * (b.x() - a.x()) + (b.y() - a.y()) * (b.y() - a.y())));
    }
};
