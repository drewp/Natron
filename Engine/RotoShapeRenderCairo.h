/* ***** BEGIN LICENSE BLOCK *****
 * This file is part of Natron <http://www.natron.fr/>,
 * Copyright (C) 2016 INRIA and Alexandre Gauthier-Foichat
 *
 * Natron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Natron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Natron.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
 * ***** END LICENSE BLOCK ***** */

#ifndef ROTOSHAPERENDERCAIRO_H
#define ROTOSHAPERENDERCAIRO_H

// ***** BEGIN PYTHON BLOCK *****
// from <https://docs.python.org/3/c-api/intro.html#include-files>:
// "Since Python may define some pre-processor definitions which affect the standard headers on some systems, you must include Python.h before any standard headers are included."
#include <Python.h>
// ***** END PYTHON BLOCK *****

#include <list>

#ifdef ROTO_SHAPE_RENDER_ENABLE_CAIRO

#include "Global/GlobalDefines.h"
#include "Engine/EngineFwd.h"

#include "Engine/RotoBezierTriangulation.h"

NATRON_NAMESPACE_ENTER;

class RotoShapeRenderCairo
{
public:
    
    RotoShapeRenderCairo()
    {

    }

    static QString getCairoVersion();

    /**
     * @brief Small RAII class that properly destroys the cairo image upon destruction
     **/
    class CairoImageWrapper
    {
    public:

        CairoImageWrapper()
        : cairoImg(0)
        , ctx(0)
        {
        }

        CairoImageWrapper(cairo_surface_t* img,
                          cairo_t* context)
        : cairoImg(img)
        , ctx(context)
        {
        }

        ~CairoImageWrapper();
        
        cairo_surface_t* cairoImg;
        cairo_t* ctx;
    };

    /**
     * @brief Low level: render a dot with feather given parameters onto the cairo image
     **/
    static void renderDot_cairo(cairo_t* cr,
                          std::vector<cairo_pattern_t*>* dotPatterns,
                          const Point &center,
                          double internalDotRadius,
                          double externalDotRadius,
                          double pressure,
                          bool doBuildUp,
                          const std::vector<std::pair<double, double> >& opacityStops,
                          double opacity);

    /**
     * @brief High level: Allocates a cairo image and calls renderDot_cairo
     **/
    static bool allocateAndRenderSingleDotStroke_cairo(int brushSizePixel, double brushHardness, double alpha, CairoImageWrapper& wrapper);

    /**
     * @brief Low-level: used to apply a pattern and destroy it
     **/
    static void applyAndDestroyMask(cairo_t* cr, cairo_pattern_t* mesh);

    /**
     * @brief High level: split a closed bezier represented by a list of control points into coons patchs
     **/
    static void bezulate(double time, const BezierCPs& cps, std::list<BezierCPs>* patches);

    /**
     * @brief Low level: renders the given stroke item into the cairo image
     **/
    static void renderStroke_cairo(cairo_t* cr,
                                   std::vector<cairo_pattern_t*>& dotPatterns,
                                   const std::list<std::list<std::pair<Point, double> > >& strokes,
                                   const double distToNextIn,
                                   const Point& lastCenterPointIn,
                                   const RotoDrawableItem* stroke,
                                   bool doBuildup,
                                   double opacity,
                                   double time,
                                   unsigned int mipmapLevel,
                                   double* distToNextOut,
                                   Point* lastCenterPoint);

    /**
     * @brief Low level: renders the given bezier with motion blur onto the given cairo image
     **/
    static void renderBezier_cairo(cairo_t* cr, const Bezier* bezier, double opacity, double time, double startTime, double endTime, double mbFrameStep, unsigned int mipmapLevel);

    /**
     * @brief Low level: renders the given bezier feather onto the given mesh pattern. This uses the old algorithm which does not use triangulation.
     **/
    static void renderFeather_old_cairo(const Bezier * bezier, double time, unsigned int mipmapLevel, double shapeColor[3], double opacity, double featherDist, double fallOff, cairo_pattern_t * mesh);

    /**
    * @brief Low level: renders the given internal bezier shape onto the given mesh pattern. This uses the old algorithm which does not use triangulation.
    **/
    static void renderInternalShape_old_cairo(double time, unsigned int mipmapLevel, double shapeColor[3], double opacity, const Transform::Matrix3x3 & transform, cairo_t * cr, cairo_pattern_t * mesh, const BezierCPs &cps);

    /**
     * @brief Low level: renders the given bezier feather onto the given mesh pattern. This uses the new algorithm which does use triangulation.
     **/
    static void renderFeather_cairo(const RotoBezierTriangulation::PolygonData& inArgs, double shapeColor[3],  double fallOff, cairo_pattern_t * mesh);

    /**
     * @brief Low level: renders the given internal bezier shape onto the given mesh pattern. This uses the new algorithm which does use triangulation.
     **/
    static void renderInternalShape_cairo(const RotoBezierTriangulation::PolygonData& inArgs,
                                          double shapeColor[3],  cairo_pattern_t * mesh);


    /**
     * @brief High level: renders the given roto item into the supplied image.
     **/
    static void renderMaskInternal_cairo(const RotoDrawableItemPtr& rotoItem,
                                         const RectI & roi,
                                         const ImageComponents& components,
                                         const double startTime,
                                         const double endTime,
                                         const double timeStep,
                                         const double time,
                                         const ImageBitDepthEnum depth,
                                         const unsigned int mipmapLevel,
                                         const bool isDuringPainting,
                                         const double distToNextIn,
                                         const Point& lastCenterPointIn,
                                         const std::list<std::list<std::pair<Point, double> > >& strokes,
                                         const ImagePtr &dstImage,
                                         double* distToNextOut,
                                         Point* lastCenterPointOut);


    static void renderSmear_cairo(double time,
                                  unsigned int mipMapLevel,
                                  const RotoStrokeItem* rotoItem,
                                  const RectI& roi,
                                  const ImagePtr& dstImage,
                                  const double distToNextIn,
                                  const Point& lastCenterPointIn,
                                  const std::list<std::list<std::pair<Point, double> > >& strokes,
                                  double* distToNextOut,
                                  Point* lastCenterPointOut);

    static void purgeCaches_cairo_internal(std::vector<cairo_pattern_t*>& cache);

    static void purgeCaches_cairo(const RotoDrawableItemPtr& rotoItem);
    
};


NATRON_NAMESPACE_EXIT;

#endif //ROTO_SHAPE_RENDER_ENABLE_CAIRO

#endif // ROTOSHAPERENDERCAIRO_H
