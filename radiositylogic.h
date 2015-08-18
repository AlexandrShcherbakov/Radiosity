#ifndef RADIOSITYLOGIC_H_INCLUDED
#define RADIOSITYLOGIC_H_INCLUDED

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

///
///TYPES
///

///Type for representation of point in 3D space
    typedef struct point {
        double x, y, z;
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
    	double distance;
    	int num;
    } com_pare;


///*******************************************



///
///FUNCTIONS
///

///Main function of radiosity implementation
    int radiosityMain(HDC hdc);

///Function returns array of patches for current quadrangle
    patched_polygon getPatchesFromQuadrangle(polygon plg, int amountOfPatches);
    ///plg: current polygon
    ///amountOfPatches: amount of patches in one line

///Computes form-factor matrix for scene
	double ** computeFormFactorForScene(patched_polygon * plgs, int polygonCount);

///Computes form-factor for two polygons
    int computeFormFactorForPolygons(patched_polygon p1, patched_polygon p2, double **FF, int offset1, int offset2);

///Computes form-factor for two patches
	double computeFormFactorForPatches(patch p1, patch p2);

///Returns scene which writen in code
	polygon * hardcodedPolygons();

///Computes radiosity for scene
    int computeRadiosity(patch_radiosity *result, double ** ff,
					int patchesCount, int iterCount);

///Draws scene on screen
	int drawScene(polygon * pls, patched_polygon * plgs, int polygonCount,
				patch_radiosity *radio, int patchCount, HDC hdc);



///*******************************************
///
///OPERATORS FOR TYPES
///

///Substraction for two points
    point sub(point p1, point p2);

///Multiply point to number
    point mult(point p, double k);

///Sum for two points
    point sum(point p1, point p2);

///Square of polygon
    double square(polygon plg);

///Vector multiplication for two vectors
	point multV(point p1, point p2);

///Length of vector
	double length(point p);

///Normal vector for polygon
    point normal(polygon p);

///Center of weightes
	point center(polygon p);

///Random point in square
	point randomPointInSquare(polygon p);

///Reflect point at center of polygon
	point polarizePointInPolygon(polygon pl, point pnt);

///Random point in polygon
	point randomPoint(polygon p);

///Scalar multiplication for two vectors
	double multS(point p1, point p2);

///Cosinus for angle between two vectors
	double cosV(point p1, point p2);

///Check if point in polygon
	int inPolygon(polygon pl, point p);

///Comparator for polygons
    int compar (const void* p1, const void* p2);

///Distance between point and area
	double distance(polygon pl, point p);

///********************************************
///
///CONSTANTS
///
enum {
	POINTS_IN_QUADRANGLE=4,
    MONTE_KARLO_ITERATIONS_COUNT=1000,
    SCREEN_WIDTH=800,
    SCREEN_HEIGHT=600,
    SCALE_CONST=300
};

#ifdef __cplusplus
}
#endif

#endif // RADIOSITYLOGIC_H_INCLUDED
