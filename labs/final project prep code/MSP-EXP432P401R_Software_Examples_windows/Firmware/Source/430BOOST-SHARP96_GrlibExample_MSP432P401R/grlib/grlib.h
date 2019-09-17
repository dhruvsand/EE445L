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

#include <stdint.h>
#include <stdbool.h>

#ifndef __GRLIB_H__
#define __GRLIB_H__

#define NDEBUG
#include "assert.h"

//*****************************************************************************
//
//! \addtogroup primitives_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! This structure defines the characteristics of a Bitmap Image
//
//*****************************************************************************
typedef struct _Graphics_Image
{
    uint8_t bPP;	             //!< Bits per pixel and Compressed/Uncompressed
    uint16_t xSize;              //!< xSize
    uint16_t ySize;              //!< ySize
    uint16_t numColors;          //!< Number of Colors in Palette
    const uint32_t  * pPalette;  //!< Pointer to Palette
    const uint8_t * pPixel;      //!< Pointer to pixel data
} Graphics_Image;

//TODO
// Compatibility Definition


//*****************************************************************************
//
//! This structure defines the extents of a rectangle.  All points greater than
//! or equal to the minimum and less than or equal to the maximum are part of
//! the rectangle.
//
//*****************************************************************************
typedef struct _Graphics_Rectangle
{
	int16_t xMin;  			//!< The minimum X coordinate of the rectangle.
    int16_t yMin;			//!< The minimum Y coordinate of the rectangle.
    int16_t xMax;			//!< The maximum X coordinate of the rectangle.
    int16_t yMax;			//!< The maximum Y coordinate of the rectangle.
} Graphics_Rectangle;

//TODO
// Compatibility Definition

//*****************************************************************************
//
//! This structure defines the characteristics of a display driver.
//
//*****************************************************************************
typedef struct _Graphics_Display
{
    int32_t  size;				//!< The size of this structure.
    void *displayData;			//!< A pointer to display driver-specific data.
    uint16_t width;				//!< The width of this display.
    uint16_t heigth;			//!< The height of this display.
    void (*callPixelDraw)(void *displayData, int16_t x, int16_t y,
    		uint16_t value);	//!< A pointer to the function to draw a pixel on this display.
    void (*callPixelDrawMultiple)(void *displayData, int16_t x, int16_t y,
    		int16_t x0, int16_t count, int16_t bPP, const uint8_t *data,
    		const uint16_t *pucPalette);	//!< A pointer to the function to draw multiple pixels on this display.
    void (*callLineDrawH)(void *displayData, int16_t x1, int16_t x2, int16_t y,
    		uint16_t value);	//!< A pointer to the function to draw a horizontal line on this display.
    void (*callLineDrawV)(void *displayData, int16_t x, int16_t y1,
    		int16_t y2, uint16_t value); //!< A pointer to the function to draw a vertical line on this display.
    void (*callRectFill)(void *displayData, const Graphics_Rectangle *rect,
    		uint16_t value);	//!< A pointer to the function to draw a filled rectangle on this display.
    uint16_t (*callColorTranslate)(void *displayData, uint32_t  value);	//!< A pointer to the function to translate 24-bit RGB colors to display-specific colors.
    void (*callFlush)(void *displayData); //!< A pointer to the function to flush any cached drawing operations on this display.
    void (*callClearDisplay)(void *displayData, uint16_t value); //!<  A pointer to the function to clears Display. Contents of display buffer unmodified
} Graphics_Display;

//TODO
// Compatibility Definition

//*****************************************************************************
//
//! This structure describes a font used for drawing text onto the screen.
//
//*****************************************************************************
typedef struct _Graphics_Font
{
    uint8_t format;		//!< The format of the font.  Can be one of FONT_FMT_UNCOMPRESSED or FONT_FMT_PIXEL_RLE.
    uint8_t maxWidth;	//!< The maximum width of a character; this is the width of the widest character in the font, though any individual character may be narrower than this width.
    uint8_t height;		//!< The height of the character cell; this may be taller than the font data for the characters (to provide inter-line spacing).
    uint8_t baseline;	//!< The offset between the top of the character cell and the baseline of  the glyph.  The baseline is the bottom row of a capital letter, below which only the descenders of the lower case letters occur.
    uint16_t offset[96];//!< The offset within data to the data for each character in the font.
    const uint8_t *data;//!< A pointer to the data for the font.
} Graphics_Font;

//TODO
// Compatibility Definition

//*****************************************************************************
//
//! This is a newer version of the structure which describes a font used
//! for drawing text onto the screen.  This variant allows a font to contain an
//! arbitrary, contiguous block of codepoints from the 256 basic characters in
//! an ISO8859-n font and allows support for accented characters in Western
//! European languages and any left-to-right typeface supported by an ISO8859
//! variant. Fonts encoded in this format may be used interchangeably with the
//! original fonts merely by casting the structure pointer when calling any
//! function or macro which expects a font pointer as a parameter.
//
//*****************************************************************************
typedef struct _Graphics_FontEx
{
    uint8_t format;			//!< The format of the font.  Can be one of FONT_FMT_EX_UNCOMPRESSED or FONT_FMT_EX_PIXEL_RLE.
    uint8_t maxWidth;		//!< The maximum width of a character; this is the width of the widest character in the font, though any individual character may be narrower than this width.
    uint8_t height;			//!< The height of the character cell; this may be taller than the font data for the characters (to provide inter-line spacing).
    uint8_t baseline;		//!< The offset between the top of the character cell and the baseline of the glyph.  The baseline is the bottom row of a capital letter, below which only the descenders of the lower case letters occur.
    uint8_t first;		//!< The codepoint number representing the first character encoded in the font.
    uint8_t last;			//!< The codepoint number representing the last character encoded in the font.
    const uint16_t *offset;	//!< A pointer to a table containing the offset within data to the data for each character in the font.
    const uint8_t *data;	//!< A pointer to the data for the font.
} Graphics_FontEx;

//TODO
// Compatibility Definition

//*****************************************************************************
//
//! Indicates that the font data is stored in an uncompressed format.
//
//*****************************************************************************
#define GRAPHICS_FONT_FMT_UNCOMPRESSED   0x00

//*****************************************************************************
//
//! Indicates that the font data is stored using a pixel-based RLE format.
//
//*****************************************************************************
#define GRAPHICS_FONT_FMT_PIXEL_RLE      0x01

//*****************************************************************************
//
//! A marker used in the format field of a font to indicates that the font
//! data is stored using the new tFontEx structure.
//
//*****************************************************************************
#define GRAPHICS_FONT_EX_MARKER          0x80

//*****************************************************************************
//
//! Indicates that the font data is stored in an uncompressed format and uses
//! the tFontEx structure format.
//
//*****************************************************************************
#define GRAPHICS_FONT_FMT_EX_UNCOMPRESSED   (FONT_FMT_UNCOMPRESSED | FONT_EX_MARKER)

//*****************************************************************************
//
//! Indicates that the font data is stored using a pixel-based RLE format and
//! uses the tFontEx structure format.
//
//*****************************************************************************
#define GRAPHICS_FONT_FMT_EX_PIXEL_RLE      (FONT_FMT_PIXEL_RLE | FONT_EX_MARKER)

//*****************************************************************************
//
//! Value to automatically draw the entire length of the string
//! (subject to clipping)
//
//*****************************************************************************
#define GRAPHICS_AUTO_STRING_LENGTH     -1

//*****************************************************************************
//
//! Value to draw text opaque
//! The text foreground and background are drawn together
//
//*****************************************************************************
#define GRAPHICS_OPAQUE_TEXT     1

//*****************************************************************************
//
//! Value to draw text transparently
//! The text only (no background) is drawn
//
//*****************************************************************************
#define GRAPHICS_TRANSPARENT_TEXT     0

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with a single bit.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_1BPP_UNCOMP   0x01

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with two bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_2BPP_UNCOMP   0x02

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with four bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_4BPP_UNCOMP   0x04

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with eight bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_8BPP_UNCOMP   0x08

//*****************************************************************************
//
//! Indicates that the image data is compressed with 4 bit Run Length Encoding
//! and represents each pixel with a single bit.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_1BPP_COMP_RLE4     0x41

//*****************************************************************************
//
//! Indicates that the image data is compressed with 4 bit Run Length Encoding 
//! and represents each pixel with two bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_2BPP_COMP_RLE4     0x42

//*****************************************************************************
//
//! Indicates that the image data is compressed with 4 bit Run Length Encoding 
//! and represents each pixel with four bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_4BPP_COMP_RLE4     0x44

//*****************************************************************************
//
//! Indicates that the image data is compressed with 8 bit Run Length Encoding 
//! and represents each pixel with a single bit.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_1BPP_COMP_RLE8     0x81

//*****************************************************************************
//
//! Indicates that the image data is compressed with 8 bit Run Length Encoding
//! and represents each pixel with two bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_2BPP_COMP_RLE8     0x82

//*****************************************************************************
//
//! Indicates that the image data is compressed with 8 bit Run Length Encoding
//! and represents each pixel with four bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_4BPP_COMP_RLE8     0x84

//*****************************************************************************
//
//! Indicates that the image data is compressed with 8 bit Run Length Encoding
//! and represents each pixel with eight bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_8BPP_COMP_RLE8     0x88

//*****************************************************************************
//
//! Indicates that the image data is compressed and represents each pixel with
//! info bits.
//
//*****************************************************************************
#define GRAPHICS_IMAGE_FMT_8BPP_COMP_RLEBLEND     0x28


//*****************************************************************************
//
//! This structure defines a drawing context to be used to draw onto the
//! screen.  Multiple drawing contexts may exist at any time.
//
//*****************************************************************************
typedef struct _Graphics_Context
{
    int32_t  size;						//!< The size of this structure.
    const Graphics_Display *display;	//!< The screen onto which drawing operations are performed.
    Graphics_Rectangle clipRegion;		//!< The clipping region to be used when drawing onto the screen.
    uint32_t  foreground;				//!< The color used to draw primitives onto the screen.
    uint32_t  background;				//!< The background color used to draw primitives onto the screen.
    const Graphics_Font *font;			//!< The font used to render text onto the screen.
} Graphics_Context;

//*****************************************************************************
//
//! Sets the background color to be used.
//!
//! \param context is a pointer to the drawing context to modify.
//! \param value is the 24-bit RGB color to be used.
//!
//! This function sets the background color to be used for drawing operations
//! in the specified drawing context.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_setBackgroundColor(context, value)                   \
        do                                                               \
        {                                                                \
            Graphics_Context *pC = context;                                     \
            pC->background = Graphics_translateColorDisplay(pC->display, value); \
        }                                                                \
        while(0)

//*****************************************************************************
//
//! Sets the background color to be used.
//!
//! \param context is a pointer to the drawing context to modify.
//! \param value is the display driver-specific color to be used.
//!
//! This function sets the background color to be used for drawing operations
//! in the specified drawing context, using a color that has been previously
//! translated to a driver-specific color (for example, via
//! Graphics_translateColorDisplay()).
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_setBackgroundColorTranslated(context, value) \
        do                                                       \
        {                                                        \
            Graphics_Context *pC = context;                             \
            pC->background = value;                          \
        }                                                        \
        while(0)

//*****************************************************************************
//
//! Gets the width of the display being used by this drawing context.
//!
//! \param context is a pointer to the drawing context to query.
//!
//! This function returns the width of the display that is being used by this
//! drawing context.
//!
//! \return Returns the width of the display in pixels.
//
//*****************************************************************************
#define Graphics_getDisplayWidth(context)      \
        (Graphics_getWidthOfDisplay((context)->display))

//*****************************************************************************
//
//! Gets the height of the display being used by this drawing context.
//!
//! \param context is a pointer to the drawing context to query.
//!
//! This function returns the height of the display that is being used by this
//! drawing context.
//!
//! \return Returns the height of the display in pixels.
//
//*****************************************************************************
#define Graphics_getDisplayHeight(context)      \
        (Graphics_getHeightOfDisplay((context)->display))

//*****************************************************************************
//
//! Sets the font to be used.
//!
//! \param context is a pointer to the drawing context to modify.
//! \param pFnt is a pointer to the font to be used.
//!
//! This function sets the font to be used for string drawing operations in the
//! specified drawing context.  If a tFontEx type font is to be used, cast its
//! pointer to a font pointer before passing it as the pFnt parameter.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_setFont(context, pFnt) \
        do                               \
        {                                \
            Graphics_Context *pC = context;     \
            const Graphics_Font *pF = pFnt;      \
            pC->font = pF;              \
        }                                \
        while(0)

//*****************************************************************************
//
//! Sets the foreground color to be used.
//!
//! \param context is a pointer to the drawing context to modify.
//! \param value is the 24-bit RGB color to be used.
//!
//! This function sets the color to be used for drawing operations in the
//! specified drawing context.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_setForegroundColor(context, value)                   \
        do                                                               \
        {                                                                \
            Graphics_Context *pC = context;                                     \
            pC->foreground = Graphics_translateColorDisplay(pC->display, value); \
        }                                                                \
        while(0)

//*****************************************************************************
//
//! Sets the foreground color to be used.
//!
//! \param context is a pointer to the drawing context to modify.
//! \param value is the display driver-specific color to be used.
//!
//! This function sets the foreground color to be used for drawing operations
//! in the specified drawing context, using a color that has been previously
//! translated to a driver-specific color (for example, via
//! Graphics_translateColorDisplay()).
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_setForegroundColorTranslated(context, value) \
        do                                                       \
        {                                                        \
            Graphics_Context *pC = context;                             \
            pC->foreground = value;                          \
        }                                                        \
        while(0)

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param context is a pointer to the drawing context to use.
//!
//! This function flushes any cached drawing operations.  For display drivers
//! that draw into a local frame buffer before writing to the actual display,
//! calling this function will cause the display to be updated to match the
//! contents of the local frame buffer.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_flushBuffer(context)     \
        do                                 \
        {                                  \
            const Graphics_Context *pC = context; \
            Graphics_flushOnDisplay(pC->display);        \
        }                                  \
        while(0)


//*****************************************************************************
//
//! Forces a clear screen. Contents of Display buffer unmodified
//!
//! \param context is a pointer to the drawing context to use.
//!
//! This function forces a clear screen.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_clearDisplay(context)                             \
        do                                                          \
        {                                                           \
            const Graphics_Context *pC = context;                          \
            Graphics_clearDisplayOnDisplay(pC->display,pC->background );  \
        }                                                           \
        while(0)

//*****************************************************************************
//
//! Gets the baseline of a font.
//!
//! \param font is a pointer to the font to query.
//!
//! This function determines the baseline position of a font.  The baseline is
//! the offset between the top of the font and the bottom of the capital
//! letters.  The only font data that exists below the baseline are the
//! descenders on some lower-case letters (such as ``y'').
//!
//! \return Returns the baseline of the font, in pixels.
//
//*****************************************************************************
#define Graphics_getFontBaseline(font) \
        ((font)->baseline)

//*****************************************************************************
//
//! Gets the height of a font.
//!
//! \param font is a pointer to the font to query.
//!
//! This function determines the height of a font.  The height is the offset
//! between the top of the font and the bottom of the font, including any
//! ascenders and descenders.
//!
//! \return Returns the height of the font, in pixels.
//
//*****************************************************************************
#define Graphics_getFontHeight(font) \
        ((font)->height)

//*****************************************************************************
//
//! Gets the maximum width of a font.
//!
//! \param font is a pointer to the font to query.
//!
//! This function determines the maximum width of a font.  The maximum width is
//! the width of the widest individual character in the font.
//!
//! \return Returns the maximum width of the font, in pixels.
//
//*****************************************************************************
#define Graphics_getFontMaxWidth(font) \
        ((font)->maxWidth)

//*****************************************************************************
//
//! Gets the number of colors in an image.
//!
//! \param pImage is a tImage struct
//!
//! This function determines the number of colors in the palette of an image.
//! This is only valid for 4bpp and 8bpp images; 1bpp images do not contain a
//! palette.
//!
//! \return Returns the number of colors in the image.
//
//*****************************************************************************
#define Graphics_getImageColors(pImage)           \
        pImage->numColors
//*****************************************************************************
//
//! Gets the height of an image.
//!
//! \param pImage is a tImage struct
//!
//! This function determines the height of an image in pixels.
//!
//! \return Returns the height of the image in pixels.
//
//*****************************************************************************
#define Graphics_getImageHeight(pImage)          \
        pImage->ySize

//*****************************************************************************
//
//! Gets the width of an image.
//!
//! \param pImage is a tImage struct
//!
//! This function determines the width of an image in pixels.
//!
//! \return Returns the width of the image in pixels.
//
//*****************************************************************************
#define Graphics_getImageWidth(pImage)           \
        pImage->xSize

//*****************************************************************************
//
//! Determines the size of the buffer for a 1 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold a
//! 1 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define Graphics_getOffscreen1BppImageSize(lWidth, lHeight) \
        (5 + (((lWidth + 7) / 8) * lHeight))

//*****************************************************************************
//
//! Determines the size of the buffer for a 4 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold a
//! 4 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define Graphics_getOffscreen8BppImageSize(lWidth, lHeight)            \
        (6 + (16 * 3) + (((lWidth + 1) / 2) * lHeight))

//*****************************************************************************
//
//! Determines the size of the buffer for an 8 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold an
//! 8 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define GrOffScreen8BPPSize(lWidth, lHeight) \
        (6 + (256 * 3) + (lWidth * lHeight))

//*****************************************************************************
//
//! Draws a pixel.
//!
//! \param context is a pointer to the drawing context to use.
//! \param x is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//!
//! This function draws a pixel if it resides within the clipping region.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawPixel(context, x, y)                          \
        do                                                            \
        {                                                             \
            const Graphics_Context *pC = context;                            \
            if((x >= pC->clipRegion.xMin) &&                       \
               (x <= pC->clipRegion.xMax) &&                       \
               (y >= pC->clipRegion.yMin) &&                       \
               (y <= pC->clipRegion.yMax))                         \
            {                                                         \
                Graphics_drawPixelOnDisplay(pC->display, x, y,     \
                pC->foreground);                                     \
            }                                                         \
        }                                                             \
        while(0)

//*****************************************************************************
//
//! Gets the baseline of a string.
//!
//! \param context is a pointer to the drawing context to query.
//!
//! This function determines the baseline position of a string.  The baseline
//! is the offset between the top of the string and the bottom of the capital
//! letters.  The only string data that exists below the baseline are the
//! descenders on some lower-case letters (such as ``y'').
//!
//! \return Returns the baseline of the string, in pixels.
//
//*****************************************************************************
#define Graphics_getStringBaseline(context)  ((context)->font->baseline)

//*****************************************************************************
//
//! Draws a centered string.
//!
//! \param context is a pointer to the drawing context to use.
//! \param string is a pointer to the string to be drawn.
//! \param lLength is the number of characters from the string that should be
//! drawn on the screen.
//! \param x is the X coordinate of the center of the string position on the
//! screen.
//! \param y is the Y coordinate of the center of the string position on the
//! screen.
//! \param bOpaque is \b true if the background of each character should be
//! drawn and \b false if it should not (leaving the background as is).
//!
//! This function draws a string of test on the screen centered upon the
//! provided position.  The \e lLength parameter allows a portion of the
//! string to be examined without having to insert a NULL character at the
//! stopping point (which would not be possible if the string was located in
//! flash); specifying a length of -1 will cause the entire string to be
//! rendered (subject to clipping).
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawStringCentered(context, string, lLength, x, y,    \
        bOpaque)                                                            \
        do                                                                  \
        {                                                                   \
            const Graphics_Context *pC = context;                                  \
            const char *pcStr = string;                                   \
                                                                            \
            Graphics_drawString(pC, pcStr, lLength,                         \
                  (x) - (Graphics_getStringWidth(pC, pcStr, lLength) / 2), \
                  (y) - (pC->font->baseline / 2), bOpaque);             \
        }                                                                   \
        while(0)

//*****************************************************************************
//
//! Gets the height of a string.
//!
//! \param context is a pointer to the drawing context to query.
//!
//! This function determines the height of a string.  The height is the offset
//! between the top of the string and the bottom of the string, including any
//! ascenders and descenders.  Note that this will not account for the case
//! where the string in question does not have any characters that use
//! descenders but the font in the drawing context does contain characters with
//! descenders.
//!
//! \return Returns the height of the string, in pixels.
//
//*****************************************************************************
#define Graphics_getStringHeight(context)   \
        ((context)->font->height)

//*****************************************************************************
//
//! Gets the maximum width of a character in a string.
//!
//! \param context is a pointer to the drawing context to query.
//!
//! This function determines the maximum width of a character in a string.  The
//! maximum width is the width of the widest individual character in the font
//! used to render the string, which may be wider than the widest character
//! that is used to render a particular string.
//!
//! \return Returns the maximum width of a character in a string, in pixels.
//
//*****************************************************************************
#define Graphics_getStringMaxWidth(context)   \
        ((context)->font->maxWidth)

//*****************************************************************************
//
// A set of color definitions.  This set is the subset of the X11 colors (from
// rgb.txt) that are supported by typical web browsers.
//
//*****************************************************************************
#define ClrAliceBlue            0x00F0F8FF
#define ClrAntiqueWhite         0x00FAEBD7
#define ClrAqua                 0x0000FFFF
#define ClrAquamarine           0x007FFFD4
#define ClrAzure                0x00F0FFFF
#define ClrBeige                0x00F5F5DC
#define ClrBisque               0x00FFE4C4
#define ClrBlack                0x00000000
#define ClrBlanchedAlmond       0x00FFEBCD
#define ClrBlue                 0x000000FF
#define ClrBlueViolet           0x008A2BE2
#define ClrBrown                0x00A52A2A
#define ClrBurlyWood            0x00DEB887
#define ClrCadetBlue            0x005F9EA0
#define ClrChartreuse           0x007FFF00
#define ClrChocolate            0x00D2691E
#define ClrCoral                0x00FF7F50
#define ClrCornflowerBlue       0x006495ED
#define ClrCornsilk             0x00FFF8DC
#define ClrCrimson              0x00DC143C
#define ClrCyan                 0x0000FFFF
#define ClrDarkBlue             0x0000008B
#define ClrDarkCyan             0x00008B8B
#define ClrDarkGoldenrod        0x00B8860B
#define ClrDarkGray             0x00A9A9A9
#define ClrDarkGreen            0x00006400
#define ClrDarkKhaki            0x00BDB76B
#define ClrDarkMagenta          0x008B008B
#define ClrDarkOliveGreen       0x00556B2F
#define ClrDarkOrange           0x00FF8C00
#define ClrDarkOrchid           0x009932CC
#define ClrDarkRed              0x008B0000
#define ClrDarkSalmon           0x00E9967A
#define ClrDarkSeaGreen         0x008FBC8F
#define ClrDarkSlateBlue        0x00483D8B
#define ClrDarkSlateGray        0x002F4F4F
#define ClrDarkTurquoise        0x0000CED1
#define ClrDarkViolet           0x009400D3
#define ClrDeepPink             0x00FF1493
#define ClrDeepSkyBlue          0x0000BFFF
#define ClrDimGray              0x00696969
#define ClrDodgerBlue           0x001E90FF
#define ClrFireBrick            0x00B22222
#define ClrFloralWhite          0x00FFFAF0
#define ClrForestGreen          0x00228B22
#define ClrFuchsia              0x00FF00FF
#define ClrGainsboro            0x00DCDCDC
#define ClrGhostWhite           0x00F8F8FF
#define ClrGold                 0x00FFD700
#define ClrGoldenrod            0x00DAA520
#define ClrGray                 0x00808080
#define ClrGreen                0x00008000
#define ClrGreenYellow          0x00ADFF2F
#define ClrHoneydew             0x00F0FFF0
#define ClrHotPink              0x00FF69B4
#define ClrIndianRed            0x00CD5C5C
#define ClrIndigo               0x004B0082
#define ClrIvory                0x00FFFFF0
#define ClrKhaki                0x00F0E68C
#define ClrLavender             0x00E6E6FA
#define ClrLavenderBlush        0x00FFF0F5
#define ClrLawnGreen            0x007CFC00
#define ClrLemonChiffon         0x00FFFACD
#define ClrLightBlue            0x00ADD8E6
#define ClrLightCoral           0x00F08080
#define ClrLightCyan            0x00E0FFFF
#define ClrLightGoldenrodYellow 0x00FAFAD2
#define ClrLightGreen           0x0090EE90
#define ClrLightGrey            0x00D3D3D3
#define ClrLightPink            0x00FFB6C1
#define ClrLightSalmon          0x00FFA07A
#define ClrLightSeaGreen        0x0020B2AA
#define ClrLightSkyBlue         0x0087CEFA
#define ClrLightSlateGray       0x00778899
#define ClrLightSteelBlue       0x00B0C4DE
#define ClrLightYellow          0x00FFFFE0
#define ClrLime                 0x0000FF00
#define ClrLimeGreen            0x0032CD32
#define ClrLinen                0x00FAF0E6
#define ClrMagenta              0x00FF00FF
#define ClrMaroon               0x00800000
#define ClrMediumAquamarine     0x0066CDAA
#define ClrMediumBlue           0x000000CD
#define ClrMediumOrchid         0x00BA55D3
#define ClrMediumPurple         0x009370DB
#define ClrMediumSeaGreen       0x003CB371
#define ClrMediumSlateBlue      0x007B68EE
#define ClrMediumSpringGreen    0x0000FA9A
#define ClrMediumTurquoise      0x0048D1CC
#define ClrMediumVioletRed      0x00C71585
#define ClrMidnightBlue         0x00191970
#define ClrMintCream            0x00F5FFFA
#define ClrMistyRose            0x00FFE4E1
#define ClrMoccasin             0x00FFE4B5
#define ClrNavajoWhite          0x00FFDEAD
#define ClrNavy                 0x00000080
#define ClrOldLace              0x00FDF5E6
#define ClrOlive                0x00808000
#define ClrOliveDrab            0x006B8E23
#define ClrOrange               0x00FFA500
#define ClrOrangeRed            0x00FF4500
#define ClrOrchid               0x00DA70D6
#define ClrPaleGoldenrod        0x00EEE8AA
#define ClrPaleGreen            0x0098FB98
#define ClrPaleTurquoise        0x00AFEEEE
#define ClrPaleVioletRed        0x00DB7093
#define ClrPapayaWhip           0x00FFEFD5
#define ClrPeachPuff            0x00FFDAB9
#define ClrPeru                 0x00CD853F
#define ClrPink                 0x00FFC0CB
#define ClrPlum                 0x00DDA0DD
#define ClrPowderBlue           0x00B0E0E6
#define ClrPurple               0x00800080
#define ClrRed                  0x00FF0000
#define ClrRosyBrown            0x00BC8F8F
#define ClrRoyalBlue            0x004169E1
#define ClrSaddleBrown          0x008B4513
#define ClrSalmon               0x00FA8072
#define ClrSandyBrown           0x00F4A460
#define ClrSeaGreen             0x002E8B57
#define ClrSeashell             0x00FFF5EE
#define ClrSienna               0x00A0522D
#define ClrSilver               0x00C0C0C0
#define ClrSkyBlue              0x0087CEEB
#define ClrSlateBlue            0x006A5ACD
#define ClrSlateGray            0x00708090
#define ClrSnow                 0x00FFFAFA
#define ClrSpringGreen          0x0000FF7F
#define ClrSteelBlue            0x004682B4
#define ClrTan                  0x00D2B48C
#define ClrTeal                 0x00008080
#define ClrThistle              0x00D8BFD8
#define ClrTomato               0x00FF6347
#define ClrTurquoise            0x0040E0D0
#define ClrViolet               0x00EE82EE
#define ClrWheat                0x00F5DEB3
#define ClrWhite                0x00FFFFFF
#define ClrWhiteSmoke           0x00F5F5F5
#define ClrYellow               0x00FFFF00
#define ClrYellowGreen          0x009ACD32
#define ClrBlack		        0x00000000

//*****************************************************************************
//
// Masks and shifts to aid in color format translation by drivers.
//
//*****************************************************************************
#define ClrRedMask              0x00FF0000
#define ClrRedShift             16
#define ClrGreenMask            0x0000FF00
#define ClrGreenShift           8
#define ClrBlueMask             0x000000FF
#define ClrBlueShift            0

//*****************************************************************************
//
// Prototypes for the predefined fonts in the graphics library.  ..Cm.. is the
// computer modern font, which is a serif font.  ..Cmsc.. is the computer
// modern small-caps font, which is also a serif font.  ..Cmss.. is the
// computer modern sans-serif font.
//
//*****************************************************************************
extern const Graphics_Font g_sFontCm12;
extern const Graphics_Font g_sFontCm12b;
extern const Graphics_Font g_sFontCm12i;
extern const Graphics_Font g_sFontCm14;
extern const Graphics_Font g_sFontCm14b;
extern const Graphics_Font g_sFontCm14i;
extern const Graphics_Font g_sFontCm16;
extern const Graphics_Font g_sFontCm16b;
extern const Graphics_Font g_sFontCm16i;
extern const Graphics_Font g_sFontCm18;
extern const Graphics_Font g_sFontCm18b;
extern const Graphics_Font g_sFontCm18i;
extern const Graphics_Font g_sFontCm20;
extern const Graphics_Font g_sFontCm20b;
extern const Graphics_Font g_sFontCm20i;
extern const Graphics_Font g_sFontCm22;
extern const Graphics_Font g_sFontCm22b;
extern const Graphics_Font g_sFontCm22i;
extern const Graphics_Font g_sFontCm24;
extern const Graphics_Font g_sFontCm24b;
extern const Graphics_Font g_sFontCm24i;
extern const Graphics_Font g_sFontCm26;
extern const Graphics_Font g_sFontCm26b;
extern const Graphics_Font g_sFontCm26i;
extern const Graphics_Font g_sFontCm28;
extern const Graphics_Font g_sFontCm28b;
extern const Graphics_Font g_sFontCm28i;
extern const Graphics_Font g_sFontCm30;
extern const Graphics_Font g_sFontCm30b;
extern const Graphics_Font g_sFontCm30i;
extern const Graphics_Font g_sFontCm32;
extern const Graphics_Font g_sFontCm32b;
extern const Graphics_Font g_sFontCm32i;
extern const Graphics_Font g_sFontCm34;
extern const Graphics_Font g_sFontCm34b;
extern const Graphics_Font g_sFontCm34i;
extern const Graphics_Font g_sFontCm36;
extern const Graphics_Font g_sFontCm36b;
extern const Graphics_Font g_sFontCm36i;
extern const Graphics_Font g_sFontCm38;
extern const Graphics_Font g_sFontCm38b;
extern const Graphics_Font g_sFontCm38i;
extern const Graphics_Font g_sFontCm40;
extern const Graphics_Font g_sFontCm40b;
extern const Graphics_Font g_sFontCm40i;
extern const Graphics_Font g_sFontCm42;
extern const Graphics_Font g_sFontCm42b;
extern const Graphics_Font g_sFontCm42i;
extern const Graphics_Font g_sFontCm44;
extern const Graphics_Font g_sFontCm44b;
extern const Graphics_Font g_sFontCm44i;
extern const Graphics_Font g_sFontCm46;
extern const Graphics_Font g_sFontCm46b;
extern const Graphics_Font g_sFontCm46i;
extern const Graphics_Font g_sFontCm48;
extern const Graphics_Font g_sFontCm48b;
extern const Graphics_Font g_sFontCm48i;
extern const Graphics_Font g_sFontCmsc12;
extern const Graphics_Font g_sFontCmsc14;
extern const Graphics_Font g_sFontCmsc16;
extern const Graphics_Font g_sFontCmsc18;
extern const Graphics_Font g_sFontCmsc20;
extern const Graphics_Font g_sFontCmsc22;
extern const Graphics_Font g_sFontCmsc24;
extern const Graphics_Font g_sFontCmsc26;
extern const Graphics_Font g_sFontCmsc28;
extern const Graphics_Font g_sFontCmsc30;
extern const Graphics_Font g_sFontCmsc32;
extern const Graphics_Font g_sFontCmsc34;
extern const Graphics_Font g_sFontCmsc36;
extern const Graphics_Font g_sFontCmsc38;
extern const Graphics_Font g_sFontCmsc40;
extern const Graphics_Font g_sFontCmsc42;
extern const Graphics_Font g_sFontCmsc44;
extern const Graphics_Font g_sFontCmsc46;
extern const Graphics_Font g_sFontCmsc48;
extern const Graphics_Font g_sFontCmss12;
extern const Graphics_Font g_sFontCmss12b;
extern const Graphics_Font g_sFontCmss12i;
extern const Graphics_Font g_sFontCmss14;
extern const Graphics_Font g_sFontCmss14b;
extern const Graphics_Font g_sFontCmss14i;
extern const Graphics_Font g_sFontCmss16;
extern const Graphics_Font g_sFontCmss16b;
extern const Graphics_Font g_sFontCmss16i;
extern const Graphics_Font g_sFontCmss18;
extern const Graphics_Font g_sFontCmss18b;
extern const Graphics_Font g_sFontCmss18i;
extern const Graphics_Font g_sFontCmss20;
extern const Graphics_Font g_sFontCmss20b;
extern const Graphics_Font g_sFontCmss20i;
extern const Graphics_Font g_sFontCmss22;
extern const Graphics_Font g_sFontCmss22b;
extern const Graphics_Font g_sFontCmss22i;
extern const Graphics_Font g_sFontCmss24;
extern const Graphics_Font g_sFontCmss24b;
extern const Graphics_Font g_sFontCmss24i;
extern const Graphics_Font g_sFontCmss26;
extern const Graphics_Font g_sFontCmss26b;
extern const Graphics_Font g_sFontCmss26i;
extern const Graphics_Font g_sFontCmss28;
extern const Graphics_Font g_sFontCmss28b;
extern const Graphics_Font g_sFontCmss28i;
extern const Graphics_Font g_sFontCmss30;
extern const Graphics_Font g_sFontCmss30b;
extern const Graphics_Font g_sFontCmss30i;
extern const Graphics_Font g_sFontCmss32;
extern const Graphics_Font g_sFontCmss32b;
extern const Graphics_Font g_sFontCmss32i;
extern const Graphics_Font g_sFontCmss34;
extern const Graphics_Font g_sFontCmss34b;
extern const Graphics_Font g_sFontCmss34i;
extern const Graphics_Font g_sFontCmss36;
extern const Graphics_Font g_sFontCmss36b;
extern const Graphics_Font g_sFontCmss36i;
extern const Graphics_Font g_sFontCmss38;
extern const Graphics_Font g_sFontCmss38b;
extern const Graphics_Font g_sFontCmss38i;
extern const Graphics_Font g_sFontCmss40;
extern const Graphics_Font g_sFontCmss40b;
extern const Graphics_Font g_sFontCmss40i;
extern const Graphics_Font g_sFontCmss42;
extern const Graphics_Font g_sFontCmss42b;
extern const Graphics_Font g_sFontCmss42i;
extern const Graphics_Font g_sFontCmss44;
extern const Graphics_Font g_sFontCmss44b;
extern const Graphics_Font g_sFontCmss44i;
extern const Graphics_Font g_sFontCmss46;
extern const Graphics_Font g_sFontCmss46b;
extern const Graphics_Font g_sFontCmss46i;
extern const Graphics_Font g_sFontCmss48;
extern const Graphics_Font g_sFontCmss48b;
extern const Graphics_Font g_sFontCmss48i;
extern const Graphics_Font g_sFontCmtt12;
extern const Graphics_Font g_sFontCmtt14;
extern const Graphics_Font g_sFontCmtt16;
extern const Graphics_Font g_sFontCmtt18;
extern const Graphics_Font g_sFontCmtt20;
extern const Graphics_Font g_sFontCmtt22;
extern const Graphics_Font g_sFontCmtt24;
extern const Graphics_Font g_sFontCmtt26;
extern const Graphics_Font g_sFontCmtt28;
extern const Graphics_Font g_sFontCmtt30;
extern const Graphics_Font g_sFontCmtt32;
extern const Graphics_Font g_sFontCmtt34;
extern const Graphics_Font g_sFontCmtt36;
extern const Graphics_Font g_sFontCmtt38;
extern const Graphics_Font g_sFontCmtt40;
extern const Graphics_Font g_sFontCmtt42;
extern const Graphics_Font g_sFontCmtt44;
extern const Graphics_Font g_sFontCmtt46;
extern const Graphics_Font g_sFontCmtt48;
extern const Graphics_Font g_sFontFixed6x8;

//*****************************************************************************
//
// Language identifiers supported by the string table processing functions.
//
//*****************************************************************************
#define GrLangZhPRC             0x0804      // Chinese (PRC)
#define GrLangZhTW              0x0404      // Chinese (Taiwan)
#define GrLangEnUS              0x0409      // English (United States)
#define GrLangEnUK              0x0809      // English (United Kingdom)
#define GrLangEnAUS             0x0C09      // English (Australia)
#define GrLangEnCA              0x1009      // English (Canada)
#define GrLangEnNZ              0x1409      // English (New Zealand)
#define GrLangFr                0x040C      // French (Standard)
#define GrLangDe                0x0407      // German (Standard)
#define GrLangHi                0x0439      // Hindi
#define GrLangIt                0x0410      // Italian (Standard)
#define GrLangJp                0x0411      // Japanese
#define GrLangKo                0x0412      // Korean
#define GrLangEsMX              0x080A      // Spanish (Mexico)
#define GrLangEsSP              0x0C0A      // Spanish (Spain)
#define GrLangSwKE              0x0441      // Swahili (Kenya)
#define GrLangUrIN              0x0820      // Urdu (India)
#define GrLangUrPK              0x0420      // Urdu (Pakistan)

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param value is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
#define Graphics_translateColorDisplay(display, value)                   \
        ((display)->callColorTranslate((display)->displayData, value))

//*****************************************************************************
//
//! Flushes cached drawing operations.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//!
//! This function flushes any cached drawing operations on a display.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_flushOnDisplay(display)    \
        do                                   \
        {                                    \
            const Graphics_Display *pD = display;   \
            pD->callFlush(pD->displayData); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Forces a clear Display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param value is the current background color to fill the screen with.
//!
//! This function clears the Display
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_clearDisplayOnDisplay(display, value)          \
        do                                                         \
        {                                                          \
            const Graphics_Display *pD = display;                         \
            pD->callClearDisplay(pD->displayData, value);       \
        }                                                          \
        while(0)

//*****************************************************************************
//
//! Gets the height of the display.
//!
//! \param display is a pointer to the display driver structure for the
//! display to query.
//!
//! This function determines the height of the display.
//!
//! \return Returns the height of the display in pixels.
//
//*****************************************************************************
#define Graphics_getHeightOfDisplay(display) ((display)->heigth)

//*****************************************************************************
//
//! Draws a horizontal line on a display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param x1 is the starting X coordinate of the line.
//! \param x2 is the ending X coordinate of the line.
//! \param y is the Y coordinate of the line.
//! \param value is the color to draw the line.
//!
//! This function draws a horizontal line on a display.  This assumes that
//! clipping has already been performed, and that both end points of the line
//! are within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawHorizontalLineOnDisplay(display, x1, x2, y, value) \
        do                                                                    \
        {                                                                     \
            const Graphics_Display *pD = display;                                    \
            pD->callLineDrawH(pD->displayData, x1, x2, y, value);       \
        }                                                                     \
        while(0)

//*****************************************************************************
//
//! Draws a vertical line on a display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param x is the X coordinate of the line.
//! \param y1 is the starting Y coordinate of the line.
//! \param y2 is the ending Y coordinate of the line.
//! \param value is the color to draw the line.
//!
//! This function draws a vertical line on a display.  This assumes that
//! clipping has already been performed, and that both end points of the line
//! are within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawVerticalLineOnDisplay(display, x, y1, y2, value)  \
        do                                                                   \
        {                                                                    \
            const Graphics_Display *pD = display;                                   \
            pD->callLineDrawV(pD->displayData, x, y1, y2, value);      \
        }                                                                    \
        while(0)

//*****************************************************************************
//
//! Draws a pixel on a display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param x is the X coordinate of the pixel.
//! \param y is the Y coordinate of the pixel.
//! \param value is the color to draw the pixel.
//!
//! This function draws a pixel on a display.  This assumes that clipping has
//! already been performed.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawPixelOnDisplay(display, x, y, value)    \
        do                                                        \
        {                                                         \
            const Graphics_Display *pD = display;                        \
            pD->callPixelDraw(pD->displayData, x, y, value); \
        }                                                         \
        while(0)

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on a display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param x is the X coordinate of the first pixel.
//! \param y is the Y coordinate of the first pixel.
//! \param x0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param count is the number of pixels to draw.
//! \param bPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param data is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on a display, using the
//! supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_drawPixelOnDisplayMultiple(display, x, y, x0, count,   \
		bPP, data, pucPalette)                                           \
        do                                                                   \
        {                                                                    \
            const Graphics_Display *pD = display;                                   \
            pD->callPixelDrawMultiple(pD->displayData, x, y, x0, count, \
                                     bPP, data, pucPalette);             \
        }                                                                    \
        while(0)

//*****************************************************************************
//
//! Fills a rectangle on a display.
//!
//! \param display is the pointer to the display driver structure for the
//! display to operate upon.
//! \param rect is a pointer to the structure describing the rectangle to
//! fill.
//! \param value is the color to fill the rectangle.
//!
//! This function fills a rectangle on the display.  This assumes that clipping
//! has already been performed, and that all sides of the rectangle are within
//! the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define Graphics_fillRectangleOnDisplay(display, rect, value) \
        do                                                        \
        {                                                         \
            const Graphics_Display *pD = display;                \
            pD->callRectFill(pD->displayData, rect, value);   \
        }                                                         \
        while(0)

//*****************************************************************************
//
//! Gets the width of the display.
//!
//! \param display is a pointer to the display driver structure for the
//! display to query.
//!
//! This function determines the width of the display.
//!
//! \return Returns the width of the display in pixels.
//
//*****************************************************************************
#define Graphics_getWidthOfDisplay(display) \
        ((display)->width)

//*****************************************************************************
//
//! Determines if a point lies within a given rectangle.
//!
//! \param rect is a pointer to the rectangle which the point is to be checked
//! against.
//! \param x is the X coordinate of the point to be checked.
//! \param y is the Y coordinate of the point to be checked.
//!
//! This function determines whether point (x, y) lies within the rectangle
//! described by \e rect.
//!
//! \return Returns 1 if the point is within the rectangle or 0 otherwise.
//
//*****************************************************************************
#define Graphics_isPointWithinRectangle(rect, x, y)                                  \
            ((((x) >= (rect)->xMin) && ((x) <= (rect)->xMax) &&       \
              ((y) >= (rect)->yMin) && ((y) <= (rect)->yMax)) ? 1 : 0)

//*****************************************************************************
//
// Prototypes for the graphics library functions.
//
//*****************************************************************************
extern void Graphics_drawCircle(const Graphics_Context *context, int32_t  x,
		int32_t  y, int32_t  lRadius);
extern void Graphics_fillCircle(const Graphics_Context *context, int32_t  x,
		int32_t  y, int32_t  lRadius);
extern void Graphics_setClipRegion(Graphics_Context *context,
		Graphics_Rectangle *rect);
extern void Graphics_initContext(Graphics_Context *context,
		const Graphics_Display *display);
extern void Graphics_drawImage(const Graphics_Context *context,
                        const Graphics_Image *pBitmap, int16_t x, int16_t y);
extern void Graphics_drawLine(const Graphics_Context *context, int32_t  x1,
		int32_t  y1, int32_t  x2, int32_t  y2);
extern void Graphics_drawLineH(const Graphics_Context *context, int32_t  x1,
		int32_t  x2, int32_t  y);
extern void Graphics_drawLineV(const Graphics_Context *context, int32_t  x,
		int32_t  y1, int32_t  y2);
extern void Graphics_drawRectangle(const Graphics_Context *context,
		const Graphics_Rectangle *rect);
extern void Graphics_fillRectangle(const Graphics_Context *context,
		const Graphics_Rectangle *rect);
extern void Graphics_drawString(const Graphics_Context *context, const char *string,
                         int32_t  lLength, int32_t  x, int32_t  y,
                         bool  opaque);
extern int32_t  Graphics_getStringWidth(const Graphics_Context *context,
		const char *string, int32_t  lLength);
extern int32_t  Graphics_isOverlappingRectangle(Graphics_Rectangle *psRect1,
		Graphics_Rectangle *psRect2);
extern int32_t  Graphics_getRectangleIntersection(Graphics_Rectangle *psRect1,
		Graphics_Rectangle *psRect2, Graphics_Rectangle *psIntersect);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __GRLIB_H__
