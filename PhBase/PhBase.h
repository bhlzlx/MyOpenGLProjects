#pragma once
#include <stdio.h>
#include "3RD/zlib/zlib.h"
#include "3RD/libpng/png.h"
#include "3RD/libpng/pngconf.h"
#include "3RD/libpng/pngstruct.h"
#include "3RD/libpng/pnginfo.h"

#include "Archive.h"

#include "TypeDef.h"

#define __gl_check_error__  {\
    GLenum error = glGetError();\
    if( error != GL_NO_ERROR )\
        ErrorHandler( error, __FILE__, __LINE__ );\
}

inline void ErrorHandler(PhU32 _eror, const char * _file, PhU32 _line)
{
	printf("error!");
}