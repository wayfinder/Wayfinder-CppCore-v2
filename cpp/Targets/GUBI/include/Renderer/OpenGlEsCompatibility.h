#ifndef OPENGLESCOMPATIBILITY_H
#define OPENGLESCOMPATIBILITY_H

#ifdef __SYMBIAN__
#include <gles/gl.h>
#include <gles/egl.h>
#elif defined IPHONE
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#else
#include <GLES/gl.h>
#include <GLES/egl.h>
#endif

#endif /* OPENGLESCOMPATIBILITY_H */
