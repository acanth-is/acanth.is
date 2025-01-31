# SPDX-FileCopyrightText: 2021 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

target_sources(acanthis
    PUBLIC
        base/aglobject.h
        base/agldynamicline.h
        base/agldynamicrect.h
        base/agllines.h
        base/agllinesuniform.h
        base/aglrastertexture.h
        base/agltriangles.h
        base/agltrianglesuniform.h
        func/aglutriangulator.h
        derived/aglobjects.h
        derived/aglpolygons.h
        derived/aglregularpolygons.h
        composite/aglmap.h
        composite/aglshapemap.h
        composite/aglshapegraph.h
        composite/aglpixelmap.h
        composite/aglgraph.h
        viewmodel/aglviewmodel.h
        viewmodel/aglmapviewmodel.h
        view/aglmapviewrenderer.h
        view/aglmapviewport.h
    PRIVATE
        base/agldynamicline.cpp
        base/agldynamicrect.cpp
        base/agllines.cpp
        base/agllinesuniform.cpp
        base/aglrastertexture.cpp
        base/agltriangles.cpp
        base/agltrianglesuniform.cpp
        func/aglutriangulator.cpp
        derived/aglpolygons.cpp
        derived/aglregularpolygons.cpp
        composite/aglshapemap.cpp
        composite/aglshapegraph.cpp
        composite/aglpixelmap.cpp
        composite/aglgraph.cpp
        viewmodel/aglmapviewmodel.cpp
        view/aglmapviewrenderer.cpp
        view/aglmapviewport.cpp
)



