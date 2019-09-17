/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include "grlib.h"

//*****************************************************************************
//
//! \addtogroup context_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Initializes a drawing context.
//!
//! \param context is a pointer to the drawing context to initialize.
//! \param display is a pointer to the Graphics_Display Info structure that
//! describes the display driver to use.
//!
//! This function initializes a drawing context, preparing it for use.  The
//! provided display driver will be used for all subsequent graphics
//! operations, and the default clipping region will be set to the extent of
//! the screen.
//!
//! \return None.
//
//*****************************************************************************
void Graphics_initContext(Graphics_Context *context, const Graphics_Display *display)
{
    //
    // Check the arguments.
    //
    assert(context);
    assert(display);

    //
    // Set the size of the context.
    //
    context->size = sizeof(Graphics_Context);

    //
    // Save the pointer to the display.
    //
    context->display = display;

    //
    // Initialize the extent of the clipping region to the extents of the
    // screen.
    //
    context->clipRegion.xMin = 0;
    context->clipRegion.yMin = 0;
    context->clipRegion.xMax = display->width - 1;
    context->clipRegion.yMax = display->heigth - 1;

    //
    // Provide a default color and font.
    //
    context->foreground = 0;
    context->background = 0;
    context->font = 0;
}

//*****************************************************************************
//
//! Sets the extents of the clipping region.
//!
//! \param context is a pointer to the drawing context to use.
//! \param rect is a pointer to the structure containing the extents of the
//! clipping region.
//!
//! This function sets the extents of the clipping region.  The clipping region
//! is not allowed to exceed the extents of the screen, but may be a portion of
//! the screen.
//!
//! The supplied coordinate are inclusive; \e xMin of 1 and \e xMax of 1 will
//! define a clipping region that will display only the pixels in the X = 1
//! column.  A consequence of this is that the clipping region must contain
//! at least one row and one column.
//!
//! \return None.
//
//*****************************************************************************
void Graphics_setClipRegion(Graphics_Context *context, Graphics_Rectangle *rect)
{
    uint32_t  w, h;

    //
    // Check the arguments.
    //
    assert(context);
    assert(rect);

    //
    // Get the width and height of the display.
    //
    w = Graphics_getWidthOfDisplay(context->display);
    h = Graphics_getHeightOfDisplay(context->display);

    //
    // Set the extents of the clipping region, forcing them to reside within
    // the extents of the screen.
    //
    context->clipRegion.xMin = ((rect->xMin < 0) ? 0 :
                                   ((rect->xMin >= w) ? (w - 1) :
                                    rect->xMin));
    context->clipRegion.yMin = ((rect->yMin < 0) ? 0 :
                                   ((rect->yMin >= h) ? (h - 1) :
                                    rect->yMin));
    context->clipRegion.xMax = ((rect->xMax < 0) ? 0 :
                                   ((rect->xMax >= w) ? (w - 1) :
                                    rect->xMax));
    context->clipRegion.yMax = ((rect->yMax < 0) ? 0 :
                                   ((rect->yMax >= h) ? (h - 1) :
                                    rect->yMax));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
