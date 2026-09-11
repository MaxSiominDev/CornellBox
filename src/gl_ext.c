#include "gl_ext.h"

#include <string.h>

static const char *required[] = {
    "GL_ARB_multitexture",
    "GL_ARB_depth_texture",
    "GL_ARB_shadow",
    "GL_EXT_framebuffer_object"
};

static int has_extension(const char *name)
{
    const char *all = (const char *)glGetString(GL_EXTENSIONS);
    const size_t length = strlen(name);
    const char *hit = all;

    while (hit != NULL && (hit = strstr(hit, name)) != NULL) {
        const char after = hit[length];

        if ((hit == all || hit[-1] == ' ') && (after == ' ' || after == '\0')) {
            return 1;
        }
        hit += length;
    }

    return 0;
}

#ifdef __APPLE__

static int resolve_entry_points(const char **missing)
{
    (void)missing;
    return 0;
}

#else

PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;

typedef void (*GenericProc)(void);

// wglGetProcAddress can return 0..3 or -1 on failure, not just NULL
static GenericProc resolve(const char *name)
{
    PROC address = wglGetProcAddress(name);

    if (address == (PROC)0 || address == (PROC)1 || address == (PROC)2 ||
        address == (PROC)3 || address == (PROC)-1) {
        return NULL;
    }

    return (GenericProc)address;
}

#define RESOLVE(variable, type, name)               \
    do {                                            \
        variable = (type)resolve(name);             \
        if (variable == NULL) {                     \
            *missing = name;                        \
            return -1;                              \
        }                                           \
    } while (0)

static int resolve_entry_points(const char **missing)
{
    RESOLVE(glActiveTexture, PFNGLACTIVETEXTUREPROC, "glActiveTexture");
    RESOLVE(glGenFramebuffersEXT, PFNGLGENFRAMEBUFFERSEXTPROC, "glGenFramebuffersEXT");
    RESOLVE(glBindFramebufferEXT, PFNGLBINDFRAMEBUFFEREXTPROC, "glBindFramebufferEXT");
    RESOLVE(glFramebufferTexture2DEXT, PFNGLFRAMEBUFFERTEXTURE2DEXTPROC, "glFramebufferTexture2DEXT");
    RESOLVE(glCheckFramebufferStatusEXT, PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, "glCheckFramebufferStatusEXT");

    return 0;
}

#endif

int gl_ext_load(const char **missing)
{
    size_t i;

    for (i = 0; i < sizeof required / sizeof required[0]; i++) {
        if (!has_extension(required[i])) {
            *missing = required[i];
            return -1;
        }
    }

    return resolve_entry_points(missing);
}
