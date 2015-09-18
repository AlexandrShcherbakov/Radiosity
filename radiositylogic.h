#ifndef RADIOSITYLOGIC_H_INCLUDED
#define RADIOSITYLOGIC_H_INCLUDED

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "GL/wglext.h"
#include "GL/glext.h"

#ifdef __cplusplus
extern "C" {
#endif


// расширения OpenGL
// VAO
PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
// VBO
PFNGLGENBUFFERSPROC    glGenBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBINDBUFFERPROC    glBindBuffer;
PFNGLBUFFERDATAPROC    glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
// Shaders
PFNGLCREATEPROGRAMPROC     glCreateProgram;
PFNGLDELETEPROGRAMPROC     glDeleteProgram;
PFNGLLINKPROGRAMPROC       glLinkProgram;
PFNGLVALIDATEPROGRAMPROC   glValidateProgram;
PFNGLUSEPROGRAMPROC        glUseProgram;
PFNGLGETPROGRAMIVPROC      glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLCREATESHADERPROC      glCreateShader;
PFNGLDELETESHADERPROC      glDeleteShader;
PFNGLSHADERSOURCEPROC      glShaderSource;
PFNGLCOMPILESHADERPROC     glCompileShader;
PFNGLATTACHSHADERPROC      glAttachShader;
PFNGLDETACHSHADERPROC      glDetachShader;
PFNGLGETSHADERIVPROC       glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC  glGetShaderInfoLog;
// Shaders attributes
PFNGLGETATTRIBLOCATIONPROC        glGetAttribLocation;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
// Shaders uniforms
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;




///
///TYPES
///

///Type for representation of point in 3D space
    typedef struct point {
        float x, y, z;
    } point;

///Type for representation of polygon in 3D space
    typedef struct polygon {
        point * vertex;
        point normal;
        int length;
    } polygon;

///Type for representation of patch in polygon
    typedef polygon patch;

///Patched polygon
    typedef struct patched_polygon {
        patch ** patches;
        int axis1, axis2;
    } patched_polygon;

///Struct which includes data for radiosity
	typedef struct patch_radiosity {
        point emmision;
        point reflectance;
        point incident;
        point excident;
        point deposit;
	} patch_radiosity;

///Struct for sort polygons
    typedef struct com_pare {
    	float distance;
    	int num;
    } com_pare;


///*******************************************



///
///FUNCTIONS
///

///Main function of radiosity implementation
    int radiosityMain();


///Function returns array of patches for current quadrangle
    int createPatchesFromQuadrangle(int polygonIndex, int ptCount);
    ///plg: current polygon
    ///amountOfPatches: amount of patches in one line

///Computes form-factor matrix for scene
	int computeFormFactorForScene();

///Computes form-factor for two polygons
    int computeFormFactorForPolygons(int p1, int p2);

///Computes form-factor for two patches
	float computeFormFactorForPatches(patch p1, patch p2, int pl1, int pl2);

///Returns scene which writen in code
	polygon * hardcodedPolygons();

///Computes radiosity for scene
    int computeRadiosity(int iterCount);

///Draws scene on screen
	int drawScene(HDC hdc);

///Initialize shaders
	int initShaders();

///Load shader
	int loadShader(char * shaderName, char **textOut, int *textLen);

///Set triangle
	int setTriangle();

///Set shader`s attributes
	int setAttrib();

///Set shader`s uniforms
	int setUniforms();

///Set centers of patches to shader
	int setCenters();

///Initialize and use shaders
	int useShaders(HDC hdc);

///*******************************************
///
///OPERATORS FOR TYPES
///

///Substraction for two points
    point sub(point p1, point p2);

///Multiply point to number
    point mult(point p, float k);

///Sum for two points
    point sum(point p1, point p2);

///Square of polygon
    float square(polygon plg);

///Vector multiplication for two vectors
	point multV(point p1, point p2);

///Length of vector
	float length(point p);

///Normal vector for polygon
    point normal(polygon p);

///Center of weightes
	point center(patch p);

///Random point in square
	point randomPointInSquare(polygon p);

///Reflect point at center of polygon
	point polarizePointInPolygon(polygon pl, point pnt);

///Random point in polygon
	point randomPoint(patch p);

///Scalar multiplication for two vectors
	float multS(point p1, point p2);

///Cosinus for angle between two vectors
	float cosV(point p1, point p2);

///Check if point in polygon
	int inPolygon(polygon pl, point p);

///Comparator for polygons
    int compar (const void* p1, const void* p2);

///Distance between point and area
	float distance(polygon pl, point p);

///Check if ray has mutual point with polygon
    int checkIntersection(polygon pl, point p1, point p2);

///********************************************
///
///CONSTANTS
///

enum {
	POINTS_IN_QUADRANGLE=4,
    MONTE_KARLO_ITERATIONS_COUNT=14,
    SCREEN_WIDTH=600,
    SCREEN_HEIGHT=600,
    SCALE_CONST=300,
    COORDS_COUNT=3,
    COLOR_COUNT=3,
    VECTORS_IN_RADIOSITY=5
};


///********************************************
///
///GLOBAL VARIABLES
///

///Form factor for scene
static float **ff;

///Polygons
static polygon *poly;

///Patched polygons
static patched_polygon *pt_poly;

///Radiosity
static patch_radiosity *radio;

///Number of polygons
static int polygonCount;

///Number of patches
static int patchCount;

///Array of offsets for index of patch
static int *ptindoffsets;

#ifdef __cplusplus
}
#endif

#define OPENGL_GET_PROC(p,n) \
		n = (p)wglGetProcAddress(#n); \
        if (NULL == n) \
        { \
                printf("Loading extension '%s' fail (%d)\n", #n, GetLastError()); \
                return 0; \
        }

GLenum g_OpenGLError;

#define OPENGL_CHECK_FOR_ERRORS() \
        if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
                printf("OpenGL error 0x%X\n", (unsigned)g_OpenGLError);

#endif // RADIOSITYLOGIC_H_INCLUDED


static void ThrowExceptionOnGLError(int line, const char *file)
{
  static char errMsg[512];

  GLenum gl_error = glGetError();

  if(gl_error == GL_NO_ERROR)
    return;

  switch(gl_error)
  {
  case GL_INVALID_ENUM:
    sprintf(errMsg, "GL_INVALID_ENUM file %s line %d\n", file, line);
    break;

  case GL_INVALID_VALUE:
    sprintf(errMsg, "GL_INVALID_VALUE file %s line %d\n",  file, line);
    break;

  case GL_INVALID_OPERATION:
    sprintf(errMsg, "GL_INVALID_OPERATION file %s line %d\n",  file, line);
    break;

  case GL_STACK_OVERFLOW:
    sprintf(errMsg, "GL_STACK_OVERFLOW file %s line %d\n",  file, line);
    break;

  case GL_STACK_UNDERFLOW:
    sprintf(errMsg, "GL_STACK_UNDERFLOW file %s line %d\n",  file, line);
    break;

  case GL_OUT_OF_MEMORY:
    sprintf(errMsg, "GL_OUT_OF_MEMORY file %s line %d\n",  file, line);
    break;

  case GL_TABLE_TOO_LARGE:
    sprintf(errMsg, "GL_TABLE_TOO_LARGE file %s line %d\n",  file, line);
    break;

  case GL_NO_ERROR:
    break;

  default:
    sprintf(errMsg, "Unknown error @ file %s line %d\n",  file, line);
    break;
  }

  if(gl_error != GL_NO_ERROR)
    printf("!!!ERROR BUGURT\n%s", errMsg);
}


#define CHECK_GL_ERRORS ThrowExceptionOnGLError(__LINE__,__FILE__);
