#include"radiositylogic.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <float.h>
#include <x86intrin.h>
#include <omp.h>

#include <time.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "GL/wglext.h"
#include "GL/glext.h"

#include "../clew/clew.h"

#include <CL/cl_gl.h>
#include <string.h>

int radiosityMain() {
	//Initializetion of polygons
	polygonCount = 18;
    poly = hardcodedPolygons();
    pt_poly = calloc(polygonCount, sizeof(*pt_poly));
    ptindoffsets = calloc(polygonCount + 1, sizeof(*pt_poly));
    ptindoffsets[0] = 0;
    int k = 16;

	//Creation of patches
	for (int i = 0; i < 6; ++i) {
        createPatchesFromQuadrangle(i, k);
        if (i > 0) {
            ptindoffsets[i] = ptindoffsets[i - 1] + pt_poly[i - 1].axis1 *
													pt_poly[i - 1].axis2;
        }
	}
	for (int i = 6; i < polygonCount; ++i) {
        createPatchesFromQuadrangle(i, k / 2);
        if (i > 0) {
            ptindoffsets[i] = ptindoffsets[i - 1] + pt_poly[i - 1].axis1 *
													pt_poly[i - 1].axis2;
        }
	}
    patchCount = ptindoffsets[polygonCount] = ptindoffsets[polygonCount - 1] +
			pt_poly[polygonCount - 1].axis1 * pt_poly[polygonCount - 1].axis2;

	//Initialization of form-factors and radiosity
    radio = calloc(patchCount, sizeof(*radio));

    emission = calloc(patchCount, sizeof(*emission));
    excident = calloc(patchCount, sizeof(*excident));
    incident = calloc(patchCount, sizeof(*incident));
    reflectance = calloc(patchCount, sizeof(*reflectance));
    deposit = calloc(patchCount, sizeof(*deposit));

    ff = calloc(patchCount, sizeof(*ff));
    for (int i = 0; i < patchCount; ++i) {
		ff[i] = calloc(patchCount, sizeof(**ff));
    }

	//Read or compute form-factor
	char ff_file[50];
    sprintf(ff_file, "ff\\%d", patchCount);
    FILE *formfactfile;
    if ((formfactfile = fopen(ff_file, "rb")) == NULL) {
		computeFormFactorForScene();
        formfactfile = fopen(ff_file, "wb");
        for (int i = 0; i < patchCount; ++i) {
            for (int j = 0; j < patchCount; ++j) {
				fwrite(&ff[i][j], sizeof(ff[i][j]), 1, formfactfile);
                //fprintf(formfactfile, "%lf ", ff[i][j]);
            }
            //fprintf(formfactfile, "\n");
        }
    } else {
    	printf("Form-factor already computed\n");
		for (int i = 0; i < patchCount; ++i) {
            for (int j = 0; j < patchCount; ++j) {
				fread(&ff[i][j], sizeof(ff[i][j]), 1, formfactfile);
                //fscanf(formfactfile, "%lf ", &ff[i][j]);
            }
        }
    }

	//Initialization of colors
    for (int i = 0; i < patchCount; ++i) {
        radio[i].reflectance.x = 1;
        radio[i].reflectance.y = 1;
        radio[i].reflectance.z = 1;
    }
    for (int i = k * k; i < 2 * k * k; ++i) {
        radio[i].reflectance.x = 1;
        radio[i].reflectance.y = 0;
        radio[i].reflectance.z = 0;
    }
    for (int i = 2 * k * k; i < 3 * k * k; ++i) {
        radio[i].reflectance.x = 0;
        radio[i].reflectance.y = 1;
        radio[i].reflectance.z = 0;
    }
    for (int i = 6 * k * k; i < 6 * k * k + 6 * k * k / 4; ++i) {
        radio[i].reflectance.x = 0.75;
        radio[i].reflectance.y = 0.75;
        radio[i].reflectance.z = 0;
    }
    for (int i = 6 * k * k + 6 * k * k / 4; i < patchCount; ++i) {
		radio[i].reflectance.x = 0;
        radio[i].reflectance.y = 250.0 / 255;
        radio[i].reflectance.z = 154.0 / 255;
    }
    clewInit(L"OpenCL.dll");
    return 1;
}


polygon * hardcodedPolygons() {
	polygon * plgs = calloc(18, sizeof(*plgs));

	//Back wall
    plgs[0].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[0].normal.x = 1;
    plgs[0].normal.y = 0;
    plgs[0].normal.z = 0;
    plgs[0].length = POINTS_IN_QUADRANGLE;
	plgs[0].vertex[0].x = -1;
    plgs[0].vertex[0].y = -1;
    plgs[0].vertex[0].z = -1;
    plgs[0].vertex[1].x = -1;
    plgs[0].vertex[1].y = -1;
    plgs[0].vertex[1].z = 1;
    plgs[0].vertex[2].x = -1;
    plgs[0].vertex[2].y = 1;
    plgs[0].vertex[2].z = 1;
    plgs[0].vertex[3].x = -1;
    plgs[0].vertex[3].y = 1;
    plgs[0].vertex[3].z = -1;

    //Left wall
	plgs[1].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[1].normal.x = 0;
    plgs[1].normal.y = 1;
    plgs[1].normal.z = 0;
    plgs[1].length = POINTS_IN_QUADRANGLE;
	plgs[1].vertex[0].x = 1;
    plgs[1].vertex[0].y = -1;
    plgs[1].vertex[0].z = -1;
    plgs[1].vertex[1].x = 1;
    plgs[1].vertex[1].y = -1;
    plgs[1].vertex[1].z = 1;
    plgs[1].vertex[2].x = -1;
    plgs[1].vertex[2].y = -1;
    plgs[1].vertex[2].z = 1;
    plgs[1].vertex[3].x = -1;
    plgs[1].vertex[3].y = -1;
    plgs[1].vertex[3].z = -1;

    //Right wall
	plgs[2].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[2].normal.x = 0;
    plgs[2].normal.y = -1;
    plgs[2].normal.z = 0;
    plgs[2].length = POINTS_IN_QUADRANGLE;
	plgs[2].vertex[0].x = 1;
    plgs[2].vertex[0].y = 1;
    plgs[2].vertex[0].z = -1;
    plgs[2].vertex[1].x = -1;
    plgs[2].vertex[1].y = 1;
    plgs[2].vertex[1].z = -1;
    plgs[2].vertex[2].x = -1;
    plgs[2].vertex[2].y = 1;
    plgs[2].vertex[2].z = 1;
    plgs[2].vertex[3].x = 1;
    plgs[2].vertex[3].y = 1;
    plgs[2].vertex[3].z = 1;

    //Bottom wall
	plgs[3].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[3].normal.x = 0;
    plgs[3].normal.y = 0;
    plgs[3].normal.z = 1;
    plgs[3].length = POINTS_IN_QUADRANGLE;
	plgs[3].vertex[0].x = 1;
    plgs[3].vertex[0].y = -1;
    plgs[3].vertex[0].z = -1;
    plgs[3].vertex[1].x = -1;
    plgs[3].vertex[1].y = -1;
    plgs[3].vertex[1].z = -1;
    plgs[3].vertex[2].x = -1;
    plgs[3].vertex[2].y = 1;
    plgs[3].vertex[2].z = -1;
    plgs[3].vertex[3].x = 1;
    plgs[3].vertex[3].y = 1;
    plgs[3].vertex[3].z = -1;

	//Top wall
	plgs[4].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[4].normal.x = 0;
    plgs[4].normal.y = 0;
    plgs[4].normal.z = -1;
    plgs[4].length = POINTS_IN_QUADRANGLE;
	plgs[4].vertex[0].x = 1;
    plgs[4].vertex[0].y = -1;
    plgs[4].vertex[0].z = 1;
    plgs[4].vertex[1].x = 1;
    plgs[4].vertex[1].y = 1;
    plgs[4].vertex[1].z = 1;
    plgs[4].vertex[2].x = -1;
    plgs[4].vertex[2].y = 1;
    plgs[4].vertex[2].z = 1;
    plgs[4].vertex[3].x = -1;
    plgs[4].vertex[3].y = -1;
    plgs[4].vertex[3].z = 1;

    //Front wall
	plgs[5].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[5].normal.x = -1;
    plgs[5].normal.y = 0;
    plgs[5].normal.z = 0;
    plgs[5].length = POINTS_IN_QUADRANGLE;
	plgs[5].vertex[0].x = 1;
    plgs[5].vertex[0].y = -1;
    plgs[5].vertex[0].z = -1;
    plgs[5].vertex[1].x = 1;
    plgs[5].vertex[1].y = -1;
    plgs[5].vertex[1].z = 1;
    plgs[5].vertex[2].x = 1;
    plgs[5].vertex[2].y = 1;
    plgs[5].vertex[2].z = 1;
    plgs[5].vertex[3].x = 1;
    plgs[5].vertex[3].y = 1;
    plgs[5].vertex[3].z = -1;


	//Back wall
    plgs[6].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[6].normal.x = -1;
    plgs[6].normal.y = 0;
    plgs[6].normal.z = 0;
    plgs[6].length = POINTS_IN_QUADRANGLE;
	plgs[6].vertex[0].x = -0.25;
    plgs[6].vertex[0].y = -0.25;
    plgs[6].vertex[0].z = -0.75;
    plgs[6].vertex[1].x = -0.25;
    plgs[6].vertex[1].y = 0.25;
    plgs[6].vertex[1].z = -0.75;
    plgs[6].vertex[2].x = -0.25;
    plgs[6].vertex[2].y = 0.25;
    plgs[6].vertex[2].z = -0.25;
    plgs[6].vertex[3].x = -0.25;
    plgs[6].vertex[3].y = -0.25;
    plgs[6].vertex[3].z = -0.25;

    //Left wall
	plgs[7].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[7].normal.x = 0;
    plgs[7].normal.y = -1;
    plgs[7].normal.z = 0;
    plgs[7].length = POINTS_IN_QUADRANGLE;
	plgs[7].vertex[0].x = 0.25;
    plgs[7].vertex[0].y = -0.25;
    plgs[7].vertex[0].z = -0.75;
    plgs[7].vertex[1].x = -0.25;
    plgs[7].vertex[1].y = -0.25;
    plgs[7].vertex[1].z = -0.75;
    plgs[7].vertex[2].x = -0.25;
    plgs[7].vertex[2].y = -0.25;
    plgs[7].vertex[2].z = -0.25;
    plgs[7].vertex[3].x = 0.25;
    plgs[7].vertex[3].y = -0.25;
    plgs[7].vertex[3].z = -0.25;

    //Right wall
	plgs[8].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[8].normal.x = 0;
    plgs[8].normal.y = 1;
    plgs[8].normal.z = 0;
    plgs[8].length = POINTS_IN_QUADRANGLE;
	plgs[8].vertex[0].x = -0.25;
    plgs[8].vertex[0].y = 0.25;
    plgs[8].vertex[0].z = -0.75;
    plgs[8].vertex[1].x = 0.25;
    plgs[8].vertex[1].y = 0.25;
    plgs[8].vertex[1].z = -0.75;
    plgs[8].vertex[2].x = 0.25;
    plgs[8].vertex[2].y = 0.25;
    plgs[8].vertex[2].z = -0.25;
    plgs[8].vertex[3].x = -0.25;
    plgs[8].vertex[3].y = 0.25;
    plgs[8].vertex[3].z = -0.25;

    //Bottom wall
	plgs[9].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[9].normal.x = 0;
    plgs[9].normal.y = 0;
    plgs[9].normal.z = -1;
    plgs[9].length = POINTS_IN_QUADRANGLE;
	plgs[9].vertex[0].x = 0.25;
    plgs[9].vertex[0].y = -0.25;
    plgs[9].vertex[0].z = -0.75;
    plgs[9].vertex[1].x = 0.25;
    plgs[9].vertex[1].y = 0.25;
    plgs[9].vertex[1].z = -0.75;
    plgs[9].vertex[2].x = -0.25;
    plgs[9].vertex[2].y = 0.25;
    plgs[9].vertex[2].z = -0.75;
    plgs[9].vertex[3].x = -0.25;
    plgs[9].vertex[3].y = -0.25;
    plgs[9].vertex[3].z = -0.75;

	//Top wall
	plgs[10].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[10].normal.x = 0;
    plgs[10].normal.y = 0;
    plgs[10].normal.z = 1;
    plgs[10].length = POINTS_IN_QUADRANGLE;
	plgs[10].vertex[0].x = 0.25;
    plgs[10].vertex[0].y = -0.25;
    plgs[10].vertex[0].z = -0.25;
    plgs[10].vertex[1].x = -0.25;
    plgs[10].vertex[1].y = -0.25;
    plgs[10].vertex[1].z = -0.25;
    plgs[10].vertex[2].x = -0.25;
    plgs[10].vertex[2].y = 0.25;
    plgs[10].vertex[2].z = -0.25;
    plgs[10].vertex[3].x = 0.25;
    plgs[10].vertex[3].y = 0.25;
    plgs[10].vertex[3].z = -0.25;

	//Front wall
	plgs[11].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[11].normal.x = 1;
    plgs[11].normal.y = 0;
    plgs[11].normal.z = 0;
    plgs[11].length = POINTS_IN_QUADRANGLE;
	plgs[11].vertex[0].x = 0.25;
    plgs[11].vertex[0].y = -0.25;
    plgs[11].vertex[0].z = -0.75;
    plgs[11].vertex[1].x = 0.25;
    plgs[11].vertex[1].y = -0.25;
    plgs[11].vertex[1].z = -0.25;
    plgs[11].vertex[2].x = 0.25;
    plgs[11].vertex[2].y = 0.25;
    plgs[11].vertex[2].z = -0.25;
    plgs[11].vertex[3].x = 0.25;
    plgs[11].vertex[3].y = 0.25;
    plgs[11].vertex[3].z = -0.75;


	//Left-back wall
	plgs[12].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[12].normal.x = -sqrt(2.0) / 2.0;
	plgs[12].normal.y = -sqrt(2.0) / 2.0;
	plgs[12].normal.z = 0;
	plgs[12].length = POINTS_IN_QUADRANGLE;
	plgs[12].vertex[0].x = -0.65;
    plgs[12].vertex[0].y = -0.9;
    plgs[12].vertex[0].z = -0.9;
    plgs[12].vertex[1].x = -0.9;
    plgs[12].vertex[1].y = -0.65;
    plgs[12].vertex[1].z = -0.9;
    plgs[12].vertex[2].x = -0.9;
    plgs[12].vertex[2].y = -0.65;
    plgs[12].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[12].vertex[3].x = -0.65;
    plgs[12].vertex[3].y = -0.9;
    plgs[12].vertex[3].z = -0.9 + sqrt(0.125);


	//right-back wall
	plgs[13].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[13].normal.x = -sqrt(2.0) / 2.0;
	plgs[13].normal.y = sqrt(2.0) / 2.0;
	plgs[13].normal.z = 0;
	plgs[13].length = POINTS_IN_QUADRANGLE;
	plgs[13].vertex[0].x = -0.9;
    plgs[13].vertex[0].y = -0.65;
    plgs[13].vertex[0].z = -0.9;
    plgs[13].vertex[1].x = -0.65;
    plgs[13].vertex[1].y = -0.4;
    plgs[13].vertex[1].z = -0.9;
    plgs[13].vertex[2].x = -0.65;
    plgs[13].vertex[2].y = -0.4;
    plgs[13].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[13].vertex[3].x = -0.9;
    plgs[13].vertex[3].y = -0.65;
    plgs[13].vertex[3].z = -0.9 + sqrt(0.125);


    //bottom wall
	plgs[14].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[14].normal.x = 0;
	plgs[14].normal.y = 0;
	plgs[14].normal.z = -1;
	plgs[14].length = POINTS_IN_QUADRANGLE;
	plgs[14].vertex[0].x = -0.65;
    plgs[14].vertex[0].y = -0.9;
    plgs[14].vertex[0].z = -0.9;
    plgs[14].vertex[1].x = -0.4;
    plgs[14].vertex[1].y = -0.65;
    plgs[14].vertex[1].z = -0.9;
    plgs[14].vertex[2].x = -0.65;
    plgs[14].vertex[2].y = -0.4;
    plgs[14].vertex[2].z = -0.9;
    plgs[14].vertex[3].x = -0.9;
    plgs[14].vertex[3].y = -0.65;
    plgs[14].vertex[3].z = -0.9;


	//left-front wall
	plgs[15].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[15].normal.x = sqrt(2.0) / 2;
	plgs[15].normal.y = -sqrt(2.0) / 2;
	plgs[15].normal.z = 0;
	plgs[15].length = POINTS_IN_QUADRANGLE;
	plgs[15].vertex[0].x = -0.65;
    plgs[15].vertex[0].y = -0.9;
    plgs[15].vertex[0].z = -0.9;
    plgs[15].vertex[1].x = -0.65;
    plgs[15].vertex[1].y = -0.9;
    plgs[15].vertex[1].z = -0.9 + sqrt(0.125);
    plgs[15].vertex[2].x = -0.4;
    plgs[15].vertex[2].y = -0.65;
    plgs[15].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[15].vertex[3].x = -0.4;
    plgs[15].vertex[3].y = -0.65;
    plgs[15].vertex[3].z = -0.9;


    //right-front wall
	plgs[16].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[16].normal.x = sqrt(2.0) / 2;
	plgs[16].normal.y = sqrt(2.0) / 2;
	plgs[16].normal.z = 0;
	plgs[16].length = POINTS_IN_QUADRANGLE;
	plgs[16].vertex[0].x = -0.4;
    plgs[16].vertex[0].y = -0.65;
    plgs[16].vertex[0].z = -0.9;
    plgs[16].vertex[1].x = -0.4;
    plgs[16].vertex[1].y = -0.65;
    plgs[16].vertex[1].z = -0.9 + sqrt(0.125);
    plgs[16].vertex[2].x = -0.65;
    plgs[16].vertex[2].y = -0.4;
    plgs[16].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[16].vertex[3].x = -0.65;
    plgs[16].vertex[3].y = -0.4;
    plgs[16].vertex[3].z = -0.9;


    //top wall
	plgs[17].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[17].normal.x = 0;
	plgs[17].normal.y = 0;
	plgs[17].normal.z = 1;
	plgs[17].length = POINTS_IN_QUADRANGLE;
	plgs[17].vertex[0].x = -0.65;
    plgs[17].vertex[0].y = -0.9;
    plgs[17].vertex[0].z = -0.9 + sqrt(0.125);
    plgs[17].vertex[1].x = -0.9;
    plgs[17].vertex[1].y = -0.65;
    plgs[17].vertex[1].z = -0.9 + sqrt(0.125);
    plgs[17].vertex[2].x = -0.65;
    plgs[17].vertex[2].y = -0.4;
    plgs[17].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[17].vertex[3].x = -0.4;
    plgs[17].vertex[3].y = -0.65;
    plgs[17].vertex[3].z = -0.9 + sqrt(0.125);

	return plgs;
}

int createPatchesFromQuadrangle(int polygonIndex, int ptCount) {
    //Initialize step vectors
    point stepV[3];
    for (int i = 1; i < 4; ++i) {
        stepV[i - 1] = mult(sub(poly[polygonIndex].vertex[i],
								poly[polygonIndex].vertex[0]), 1.0 / ptCount);
    }

    //Create matrix of polygons
    patch ** patches = calloc(ptCount, sizeof(*patches));
    for (int i = 0; i < ptCount; ++i) {
        patches[i] = calloc(ptCount, sizeof(*patches[i]));
        for (int j = 0; j < ptCount; ++j) {
            patches[i][j].vertex = calloc(POINTS_IN_QUADRANGLE,
                                          sizeof(*patches[i][j].vertex));
			patches[i][j].length = POINTS_IN_QUADRANGLE;
			patches[i][j].normal = poly[polygonIndex].normal;
            if (i == 0) {
                if (j == 0) {
                    patches[i][j].vertex[0] = poly[polygonIndex].vertex[0];
                    patches[i][j].vertex[1] = sum(patches[i][j].vertex[0], stepV[0]);
                    patches[i][j].vertex[2] = sum(patches[i][j].vertex[0], stepV[1]);
                    patches[i][j].vertex[3] = sum(patches[i][j].vertex[0], stepV[2]);
                } else {
                    patches[i][j].vertex[0] = patches[i][j - 1].vertex[3];
                    patches[i][j].vertex[1] = patches[i][j - 1].vertex[2];
                    patches[i][j].vertex[2] = sum(patches[i][j].vertex[0], stepV[1]);
                    patches[i][j].vertex[3] = sum(patches[i][j].vertex[0], stepV[2]);
                }
            } else {
                if (j == 0) {
                    patches[i][j].vertex[0] = patches[i - 1][j].vertex[1];
                    patches[i][j].vertex[1] = sum(patches[i][j].vertex[0], stepV[0]);
                    patches[i][j].vertex[2] = sum(patches[i][j].vertex[0], stepV[1]);
                    patches[i][j].vertex[3] = patches[i - 1][j].vertex[2];
                } else {
                    patches[i][j].vertex[0] = patches[i - 1][j].vertex[1];
                    patches[i][j].vertex[1] = patches[i][j - 1].vertex[2];
                    patches[i][j].vertex[2] = sum(patches[i][j].vertex[0], stepV[1]);
                    patches[i][j].vertex[3] = patches[i - 1][j].vertex[2];
                }
            }
        }
    }
    pt_poly[polygonIndex].patches = patches;
    pt_poly[polygonIndex].axis1 = pt_poly[polygonIndex].axis2 = ptCount;
    return 1;
}


int computeFormFactorForScene() {
	printf("FF start\n");
    for (int i = 0; i < polygonCount; ++i) {
        for (int j = i + 1; j < polygonCount; ++j) {
			clock_t tm = clock();
            computeFormFactorForPolygons(i, j);
            printf("%d %d %f\n", i, j, (float)(clock() - tm) / CLOCKS_PER_SEC);
        }
    }
    return 1;
}


int computeFormFactorForPolygons(int p1, int p2) {
    for (int i1 = 0; i1 < pt_poly[p1].axis1; ++i1) {
        for (int j1 = 0; j1 < pt_poly[p1].axis2; ++j1) {
            for (int i2 = 0; i2 < pt_poly[p2].axis1; ++i2) {
                for (int j2 = 0; j2 < pt_poly[p2].axis2; ++j2) {
					int index1 = i1 * pt_poly[p1].axis2 + j1 + ptindoffsets[p1];
					int index2 = i2 * pt_poly[p2].axis2 + j2 + ptindoffsets[p2];
                    ff[index1][index2] = computeFormFactorForPatches(pt_poly[p1].patches[i1][j1],
																	pt_poly[p2].patches[i2][j2], p1, p2);
                    ff[index2][index1] = ff[index1][index2];
                    ff[index1][index2] /= square(pt_poly[p1].patches[i1][j1]);
                    ff[index2][index1] /= square(pt_poly[p2].patches[i2][j2]);
                }
            }
        }
    }
    return 1;
}


float computeFormFactorForPatches(patch p1, patch p2, int pl1, int pl2) {
	float result = 0;
	int cnt = 0;
	float xx[MONTE_KARLO_ITERATIONS_COUNT];
	float yy[MONTE_KARLO_ITERATIONS_COUNT];
	for (int i = 0; i < MONTE_KARLO_ITERATIONS_COUNT; ++i) {
        //Hammersley Point Set
        float u = 0;
        int kk = i;

        for (float p = 0.5; kk; p *= 0.5, kk >>= 1)
            if (kk & 1)
                u += p;

        float v = (i + 0.5) / MONTE_KARLO_ITERATIONS_COUNT;
        xx[i] = u;
        yy[i] = v;
	}
    for (int i = 0; i < MONTE_KARLO_ITERATIONS_COUNT; ++i) {
        for (int j = 0; j < MONTE_KARLO_ITERATIONS_COUNT; ++j) {
            float iter_res = 0;

            point on_p1 = sum(p1.vertex[0], sum(mult(sub(p1.vertex[1], p1.vertex[0]), xx[i]), mult(sub(p1.vertex[3], p1.vertex[0]), yy[i])));
            point on_p2 = sum(p2.vertex[0], sum(mult(sub(p2.vertex[1], p2.vertex[0]), xx[j]), mult(sub(p2.vertex[3], p2.vertex[0]), yy[j])));

            point r = sub(on_p1, on_p2);
            float lr = length(r);
            if (fabs(lr) < DBL_EPSILON) {
                cnt++;
                continue;
            }

            //Visibility function
            int flag = 0;
            for (int j = 0; j < polygonCount && !flag; ++j) {
                if (j == pl1 || j == pl2) continue;
                flag = checkIntersection(poly[j], on_p1, on_p2);
            }
            if (flag)
                continue;

            iter_res = cosV(r, p2.normal);
            iter_res *= cosV(mult(r, -1), p1.normal);
            if (iter_res < 0) continue;
            iter_res /=  lr * lr;
            if (iter_res > 10) continue;
            result += iter_res;
        }
    }
    result /= MONTE_KARLO_ITERATIONS_COUNT * MONTE_KARLO_ITERATIONS_COUNT - cnt;
    result /= M_PI;
	result *= square(p1) * square(p2);
    return result;
}


int computeRadiosity(int iterCount) {
    clock_t t_start = clock();

	for (int i = 0; i < patchCount; ++i)
	{
		radio[i].excident = radio[i].emission;
		radio[i].emission.x = 0;
		radio[i].emission.y = 0;
		radio[i].emission.z = 0;
		radio[i].deposit = sum(radio[i].deposit, radio[i].excident);
	}


	//Precompute
	for (int i = 0; i < patchCount; i++) {
		if (radio[i].excident.x != 0) {
			for (int j = 0; j < patchCount; j++) {
			radio[j].incident = sum(radio[j].incident,
										mult(radio[i].excident, ff[j][i]));
			}
		}
	}
	for (int i = 0; i < patchCount; ++i) {
		radio[i].excident.x = radio[i].incident.x * radio[i].reflectance.x;
		radio[i].excident.y = radio[i].incident.y * radio[i].reflectance.y;
		radio[i].excident.z = radio[i].incident.z * radio[i].reflectance.z;
		radio[i].deposit = sum(radio[i].deposit, radio[i].excident);
	}

	for (int iter = 0; iter < iterCount - 1; iter++) {
		for (int i = 0; i < patchCount; i++) {
			radio[i].incident.x = 0;
			radio[i].incident.y = 0;
			radio[i].incident.z = 0;
			for (int j = 0; j < patchCount; j++) {
				radio[i].incident = sum(radio[i].incident,
											mult(radio[j].excident, ff[i][j]));
			}
		}
		for (int i = 0; i < patchCount; ++i) {
			radio[i].excident.x = radio[i].incident.x * radio[i].reflectance.x;
			radio[i].excident.y = radio[i].incident.y * radio[i].reflectance.y;
			radio[i].excident.z = radio[i].incident.z * radio[i].reflectance.z;
			radio[i].deposit = sum(radio[i].deposit, radio[i].excident);
		}
	}
	printf("Compute radiosity time %f\n", (float)(clock() - t_start) / CLOCKS_PER_SEC);
	return 1;
}


int computeRadiosityCL(int iterCount, GLuint gl_buf, cl_context context, cl_device_id device_id) {
	char *KernelSource;
	int sourceLen;

	int err;

	float4 *vec_data = calloc(sizeof(*vec_data), patchCount);
	float *ff_data = calloc(sizeof(*ff_data), patchCount * patchCount);


    cl_command_queue commands;          // compute command queue
    cl_program program;                 // compute program
    cl_kernel kernel;                   // compute kernel

	cl_kernel ker2;

    cl_mem cl_incident;
    cl_mem cl_excident;
    cl_mem cl_ff;
    cl_mem cl_reflectance;
    cl_mem cl_emission;
    cl_mem cl_deposit;

	size_t local;
	size_t global;

    commands = clCreateCommandQueue(context, device_id, 0, &err);               CHECK_CL(err);

	loadSource("kernels/kernel1", &KernelSource, &sourceLen);
    program = clCreateProgramWithSource(context, 1, &KernelSource, NULL, &err); CHECK_CL(err);
    CHECK_CL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));
    kernel = clCreateKernel(program, "ker1", &err);                             CHECK_CL(err);

    ker2 = clCreateKernel(program, "ker2", &err);

	clock_t t_start = clock();
	clock_t loc_st;
	cl_incident = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(*incident) * patchCount, NULL, &err); CHECK_CL(err);
	cl_excident = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(*excident) * patchCount, NULL, &err); CHECK_CL(err);
	cl_reflectance = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(*reflectance) * patchCount, NULL, &err); CHECK_CL(err);
	cl_emission = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(*emission) * patchCount, NULL, &err); CHECK_CL(err);
	//cl_deposit = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(*deposit) * patchCount, NULL, NULL);
	cl_deposit = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, gl_buf, &err); CHECK_CL(err);
	cl_ff = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(*ff_data) * patchCount * patchCount, NULL, &err); CHECK_CL(err);

    CHECK_CL(clEnqueueAcquireGLObjects(commands, 1, &cl_deposit, 0, 0, 0));

	//printf("Step 1 time: %f\n", (float)(clock() - loc_st) / CLOCKS_PER_SEC);
	//loc_st = clock();

	omp_set_dynamic(0);
    omp_set_num_threads(4);

	//Add excident to opencl
	#pragma parallel for
	for (int i = 0; i < patchCount; ++i) {
		//Excident should be initialized by emission
		vec_data[i].x = radio[i].emission.x;
		vec_data[i].y = radio[i].emission.y;
		vec_data[i].z = radio[i].emission.z;
    }
    CHECK_CL(clEnqueueWriteBuffer(commands, cl_excident, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));
	//CHECK_CL(clEnqueueWriteBuffer(commands, cl_excident, CL_TRUE, 0, sizeof(*emission) * patchCount, emission, 0, NULL, NULL));
	//Add deposit to opencl
	//CHECK_CL(clEnqueueWriteBuffer(commands, cl_deposit, CL_TRUE, 0, sizeof(*emission) * patchCount, emission, 0, NULL, NULL));
	CHECK_CL(clEnqueueWriteBuffer(commands, cl_deposit, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));

	//Add emission to opencl
	#pragma parallel for
	for (int i = 0; i < patchCount; ++i) {
		vec_data[i].x = radio[i].reflectance.x;
		vec_data[i].y = radio[i].reflectance.y;
		vec_data[i].z = radio[i].reflectance.z;
		vec_data[i].w = 0;
    }
	//CHECK_CL(clEnqueueWriteBuffer(commands, cl_reflectance, CL_TRUE, 0, sizeof(*reflectance) * patchCount, reflectance, 0, NULL, NULL));
	CHECK_CL(clEnqueueWriteBuffer(commands, cl_reflectance, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));

	//Add formfactors to opencl
	#pragma parallel for
	for (int i = 0; i < patchCount; ++i) {
		for (int j = 0; j < patchCount; ++j) {
			ff_data[i * patchCount + j] = ff[i][j]; //squash formfactor matrix
		}
    }
	CHECK_CL(clEnqueueWriteBuffer(commands, cl_ff, CL_TRUE, 0, sizeof(*ff_data) * patchCount * patchCount, ff_data, 0, NULL, NULL));

	//printf("Step 2 time: %f\n", (float)(clock() - loc_st) / CLOCKS_PER_SEC);
	//loc_st = clock();

	global = patchCount;

	CHECK_CL(clSetKernelArg(kernel, 0, sizeof(cl_incident), &cl_incident));
	CHECK_CL(clSetKernelArg(kernel, 1, sizeof(cl_excident), &cl_excident));
	CHECK_CL(clSetKernelArg(kernel, 2, sizeof(cl_ff), &cl_ff));
	CHECK_CL(clSetKernelArg(kernel, 3, sizeof(cl_reflectance), &cl_reflectance));
	CHECK_CL(clSetKernelArg(kernel, 4, sizeof(cl_deposit), &cl_deposit));

	/*CHECK_CL(clSetKernelArg(ker2, 0, sizeof(cl_incident), &cl_incident));
	CHECK_CL(clSetKernelArg(ker2, 1, sizeof(cl_excident), &cl_excident));
	CHECK_CL(clSetKernelArg(ker2, 2, sizeof(cl_reflectance), &cl_reflectance));
	CHECK_CL(clSetKernelArg(ker2, 3, sizeof(cl_deposit), &cl_deposit));*/

	//printf("Step 3 time: %f\n", (float)(clock() - loc_st) / CLOCKS_PER_SEC);
	loc_st = clock();

	for (int iter = 0; iter < iterCount; iter++) {
		CHECK_CL(clEnqueueNDRangeKernel(commands, kernel, 1, 0, &global, NULL, 0, NULL, NULL));
		//CHECK_CL(clEnqueueNDRangeKernel(commands, ker2, 1, 0, &global, NULL, 0, NULL, NULL));
	}
	clFinish(commands);

	printf("Step 4 time: %f\n", (float)(clock() - loc_st) / CLOCKS_PER_SEC);

	CHECK_CL(clEnqueueReleaseGLObjects(commands, 1, &cl_deposit, 0, 0, 0));
	//loc_st = clock();

	//Take incident from OpenCL
    /*CHECK_CL(clEnqueueReadBuffer(commands, cl_incident, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));
    #pragma parallel for
    for (int i = 0; i < patchCount; ++i) {
        radio[i].incident.x = vec_data[i].x;
        radio[i].incident.y = vec_data[i].y;
        radio[i].incident.z = vec_data[i].z;
    }*/

    //Take excident from OpenCL
	//CHECK_CL(clEnqueueReadBuffer(commands, cl_excident, CL_TRUE, 0, sizeof(*excident) * patchCount, excident, 0, NULL, NULL));
	/*CHECK_CL(clEnqueueReadBuffer(commands, cl_excident, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));
    #pragma parallel for
    for (int i = 0; i < patchCount; ++i) {
        radio[i].excident.x = vec_data[i].x;
        radio[i].excident.y = vec_data[i].y;
        radio[i].excident.z = vec_data[i].z;
    }*/

    //Take deposit from OpenCL
    //CHECK_CL(clEnqueueReadBuffer(commands, cl_deposit, CL_TRUE, 0, sizeof(*deposit) * patchCount, deposit, 0, NULL, NULL));
    //CHECK_CL(clEnqueueReadBuffer(commands, cl_deposit, CL_TRUE, 0, sizeof(*vec_data) * patchCount, vec_data, 0, NULL, NULL));
    /*#pragma parallel for
    for (int i = 0; i < patchCount; ++i) {
        radio[i].deposit.x = vec_data[i].x;
        radio[i].deposit.y = vec_data[i].y;
        radio[i].deposit.z = vec_data[i].z;
    }*/

	free(vec_data);
	free(ff_data);

	//printf("Step 5 time: %f\n", (float)(clock() - loc_st) / CLOCKS_PER_SEC);

	printf("Compute radiosity time %f\n", (float)(clock() - t_start) / CLOCKS_PER_SEC);
	return 1;
}


int computeRadiositySSE(int iterCount) {
    clock_t t_start = clock();

    __m128 srad[VECTORS_IN_RADIOSITY][patchCount];
    for (int i = 0; i < patchCount; ++i) {
        srad[0][i] = _mm_setzero_ps();//emission
        srad[1][i] = _mm_set_ps(0.0f, radio[i].reflectance.x, radio[i].reflectance.y, radio[i].reflectance.z);//reflectance
        srad[2][i] = _mm_set_ps(0.0f, radio[i].incident.x, radio[i].incident.y, radio[i].incident.z);//incident
        srad[3][i] = _mm_set_ps(0.0f, radio[i].emission.x, radio[i].emission.y, radio[i].emission.z);//excident
        srad[4][i] = _mm_add_ps(_mm_set_ps(0.0f, radio[i].deposit.x, radio[i].deposit.y, radio[i].deposit.z), srad[3][i]);//deposit
    }


	//Precompute
	for (int i = 0; i < patchCount; i++) {
		if (radio[i].emission.x != 0) {
			for (int j = 0; j < patchCount; j++) {
               srad[2][j] = _mm_add_ps(srad[2][j], _mm_mul_ps(srad[3][i], _mm_set1_ps(ff[j][i])));
			}
		}
	}
	for (int i = 0; i < patchCount; ++i) {
        srad[3][i] = _mm_mul_ps(srad[2][i], srad[1][i]);
        srad[4][i] = _mm_add_ps(srad[4][i], srad[3][i]);
	}

    omp_set_dynamic(0);
    omp_set_num_threads(4);
	for (int iter = 0; iter < iterCount - 1; iter++) {
		#pragma omp parallel for
		for (int i = 0; i < patchCount; i++) {
            srad[2][i] = _mm_setzero_ps();
			for (int j = 0; j < patchCount; j++) {
			    srad[2][i] = _mm_add_ps(srad[2][i], _mm_mul_ps(srad[3][j], _mm_set1_ps(ff[i][j])));
			}
		}
		for (int i = 0; i < patchCount; ++i) {
		    srad[3][i] = _mm_mul_ps(srad[2][i], srad[1][i]);
            srad[4][i] = _mm_add_ps(srad[4][i], srad[3][i]);
		}
	}
    float tmp[4];
	for (int i = 0; i < patchCount; ++i) {
        _mm_store_ps(tmp, srad[0][i]);
        radio[i].emission.x = tmp[2];
        radio[i].emission.y = tmp[1];
        radio[i].emission.z = tmp[0];
        _mm_store_ps(tmp, srad[1][i]);
        radio[i].reflectance.x = tmp[2];
        radio[i].reflectance.y = tmp[1];
        radio[i].reflectance.z = tmp[0];
        _mm_store_ps(tmp, srad[2][i]);
        radio[i].incident.x = tmp[2];
        radio[i].incident.y = tmp[1];
        radio[i].incident.z = tmp[0];
        _mm_store_ps(tmp, srad[3][i]);
        radio[i].excident.x = tmp[2];
        radio[i].excident.y = tmp[1];
        radio[i].excident.z = tmp[0];
        _mm_store_ps(tmp, srad[4][i]);
        radio[i].deposit.x = tmp[2];
        radio[i].deposit.y = tmp[1];
        radio[i].deposit.z = tmp[0];
	}
	printf("Compute radiosity time %f\n", (float)(clock() - t_start) / CLOCKS_PER_SEC);
	return 1;
}


int drawScene(HDC hdc) {

	const int magic_const = 4 * 4;
    static int light = 4;
    radio[light].emission.x = 0;
    radio[light].emission.y = 0;
    radio[light].emission.z = 0;
    for (int i = 0; i < patchCount; ++i) {
        radio[i].deposit.x = 0;
        radio[i].deposit.y = 0;
        radio[i].deposit.z = 0;
    }
    light = (light + 1) % magic_const + magic_const * 4;
    radio[light].emission.x = magic_const;
    radio[light].emission.y = magic_const;
    radio[light].emission.z = magic_const;
	computeRadiosity(2);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();//load identity matrix

    glTranslatef(0.0f,-0.4f,-3.8f);//move forward 4 units

    for (int i = 0; i < polygonCount; ++i) {
		if (i == 5) continue;
		for (int i1 = 0; i1 < pt_poly[i].axis1; ++i1) {
			for (int j1 = 0; j1 < pt_poly[i].axis2; ++j1) {
				int pt_ind = ptindoffsets[i] + pt_poly[i].axis2 * i1 + j1;

				//Add gamma-correction
				glColor3f(pow(radio[pt_ind].deposit.x, 1.0 / 2),
						  pow(radio[pt_ind].deposit.y, 1.0 / 2),
						  pow(radio[pt_ind].deposit.z, 1.0 / 2));
				glBegin(GL_POLYGON);
				patch loc_pt = pt_poly[i].patches[i1][j1];
				for (int i2 = 0; i2 < loc_pt.length; ++i2) {
                    glVertex3d(loc_pt.vertex[i2].y, loc_pt.vertex[i2].z + 0.2, loc_pt.vertex[i2].x);
				}
				glEnd();
			}
        }
    }
    SwapBuffers(hdc);
    return light - magic_const * 4;
}


// проверка статуса param шейдера shader
GLint ShaderStatus(GLuint shader, GLenum param)
{
        GLint status, length;
        GLchar buffer[1024];

        // получим статус шейдера
        glGetShaderiv(shader, param, &status);

        // в случае ошибки запишем ее в лог-файл
        if (status != GL_TRUE)
        {
                glGetShaderInfoLog(shader, 1024, &length, buffer);
                printf("Shader: %s\n", (const char*)buffer);
        }

        // проверим не было ли ошибок OpenGL
        OPENGL_CHECK_FOR_ERRORS();

        // вернем статус
        return status;
}

// проверка статуса param шейдерной программы program
GLint ShaderProgramStatus(GLuint program, GLenum param)
{
        GLint status, length;
        GLchar buffer[1024];

        // получим статус шейдерной программы
        glGetProgramiv(program, param, &status);

        // в случае ошибки запишем ее в лог-файл
        if (status != GL_TRUE)
        {
                glGetProgramInfoLog(program, 1024, &length, buffer);
                printf("Shader program: %s\n", (const char*)buffer);
        }

        // проверим не было ли ошибок OpenGL
        OPENGL_CHECK_FOR_ERRORS();

        // вернем статус
        return status;
}


int useShaders(HDC hdc) {
	//Init functions
	OPENGL_GET_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram)
	OPENGL_GET_PROC(PFNGLCREATESHADERPROC, glCreateShader)
	OPENGL_GET_PROC(PFNGLSHADERSOURCEPROC, glShaderSource)
	OPENGL_GET_PROC(PFNGLCOMPILESHADERPROC, glCompileShader)
	OPENGL_GET_PROC(PFNGLATTACHSHADERPROC, glAttachShader)
	OPENGL_GET_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram)
	OPENGL_GET_PROC(PFNGLUSEPROGRAMPROC, glUseProgram)
	OPENGL_GET_PROC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
	OPENGL_GET_PROC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
	OPENGL_GET_PROC(PFNGLGENBUFFERSPROC, glGenBuffers)
	OPENGL_GET_PROC(PFNGLBINDBUFFERPROC, glBindBuffer)
	OPENGL_GET_PROC(PFNGLBUFFERDATAPROC, glBufferData)
	OPENGL_GET_PROC(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation)
	OPENGL_GET_PROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)
	OPENGL_GET_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
	OPENGL_GET_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)
	OPENGL_GET_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
	OPENGL_GET_PROC(PFNGLGETSHADERIVPROC, glGetShaderiv)
	OPENGL_GET_PROC(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
	OPENGL_GET_PROC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
	OPENGL_GET_PROC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)
	OPENGL_GET_PROC(PFNGLVALIDATEPROGRAMPROC, glValidateProgram)

	GLuint vertexShader, fragmentShader, geometryShader;
	GLuint shaderProgram;
	GLuint meshVAO;

	// переменные для хранения загруженных файлов
	char *shaderSource;
	int sourceLength;

	// создадим шейдерную программу и шейдеры для нее
	shaderProgram  = glCreateProgram();                                          CHECK_GL_ERRORS
	vertexShader   = glCreateShader(GL_VERTEX_SHADER);                           CHECK_GL_ERRORS
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);                         CHECK_GL_ERRORS
	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);                         CHECK_GL_ERRORS

	// загрузим вершинный шейдер
	loadSource("shaders/vertex shader", &shaderSource, &sourceLength);           CHECK_GL_ERRORS

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(vertexShader, 1, (const GLchar**)&shaderSource,
                (const GLint*)&sourceLength);                                    CHECK_GL_ERRORS
	glCompileShader(vertexShader);                                               CHECK_GL_ERRORS

	free(shaderSource);

	if (ShaderStatus(vertexShader, GL_COMPILE_STATUS) != GL_TRUE)
        printf("BUGURT!!! Line: %d\n", __LINE__);

    loadSource("shaders/geometry shader", &shaderSource, &sourceLength);           CHECK_GL_ERRORS

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(geometryShader, 1, (const GLchar**)&shaderSource,
                (const GLint*)&sourceLength);                                    CHECK_GL_ERRORS
	glCompileShader(geometryShader);                                               CHECK_GL_ERRORS

	free(shaderSource);

	if (ShaderStatus(geometryShader, GL_COMPILE_STATUS) != GL_TRUE)
        printf("BUGURT!!! Line: %d\n", __LINE__);

	// загрузим фрагментный шейдер
	loadSource("shaders/fragment shader", &shaderSource, &sourceLength);         CHECK_GL_ERRORS

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(fragmentShader, 1, (const GLchar**)&shaderSource,
                (const GLint*)&sourceLength);                                    CHECK_GL_ERRORS
	glCompileShader(fragmentShader);                                             CHECK_GL_ERRORS

	free(shaderSource);
	if (ShaderStatus(vertexShader, GL_COMPILE_STATUS) != GL_TRUE)
        printf("BUGURT!!! Line: %d\n", __LINE__);

	// присоединим загруженные шейдеры к шейдерной программе
	glAttachShader(shaderProgram, vertexShader);                                 CHECK_GL_ERRORS
	glAttachShader(shaderProgram, geometryShader);                               CHECK_GL_ERRORS
	glAttachShader(shaderProgram, fragmentShader);                               CHECK_GL_ERRORS


	// сборка шейдерной программы из прикрепленных шейдеров
	glLinkProgram(shaderProgram);                                                CHECK_GL_ERRORS
	if (ShaderProgramStatus(shaderProgram, GL_LINK_STATUS) != GL_TRUE)
        printf("BUGURT!!! Line: %d\n", __LINE__);

	// сделаем шейдерную программу активной
	glUseProgram(shaderProgram);                                                 CHECK_GL_ERRORS



	//Set geometry
	GLuint meshVBO;
	GLuint meshVBO2;
	glGenVertexArrays(1, &meshVAO);                                              CHECK_GL_ERRORS
	glBindVertexArray(meshVAO);                                                  CHECK_GL_ERRORS

    float * colors = calloc(6 * patchCount * COLOR_COUNT, sizeof(*colors));
    float * coords = calloc(6 * patchCount * COORDS_COUNT, sizeof(*coords));
    float * sides = calloc(6 * patchCount * COORDS_COUNT, sizeof(*sides));
    float * normals = calloc(6 * patchCount * COORDS_COUNT, sizeof(*normals));

	const int magic_const = 16;
    for (int i = 0; i < patchCount; ++i) {
		//deposit[i].x = deposit[i].y = deposit[i].z = 0;
        radio[i].deposit.x = 0;
        radio[i].deposit.y = 0;
        radio[i].deposit.z = 0;
    }
	int light = magic_const * magic_const * 4 + magic_const * magic_const / 2 + magic_const / 2;
    radio[light].emission.x = magic_const * magic_const / 4;
	radio[light].emission.y = magic_const * magic_const / 4;
	radio[light].emission.z = magic_const * magic_const / 4;
	//emission[light].x = emission[light].y = emission[light].z = magic_const * magic_const / 4;
    light--;
    radio[light].emission.x = magic_const * magic_const / 4;
	radio[light].emission.y = magic_const * magic_const / 4;
	radio[light].emission.z = magic_const * magic_const / 4;
	//emission[light].x = emission[light].y = emission[light].z = magic_const * magic_const / 4;
	light -= magic_const;
	radio[light].emission.x = magic_const * magic_const / 4;
	radio[light].emission.y = magic_const * magic_const / 4;
	radio[light].emission.z = magic_const * magic_const / 4;
	//emission[light].x = emission[light].y = emission[light].z = magic_const * magic_const / 4;
	light++;
	radio[light].emission.x = magic_const * magic_const / 4;
	radio[light].emission.y = magic_const * magic_const / 4;
	radio[light].emission.z = magic_const * magic_const / 4;
	//emission[light].x = emission[light].y = emission[light].z = magic_const * magic_const / 4;

    cl_device_id device_id;             // compute device id
    cl_context context;                 // compute context
	cl_platform_id platforms[30];
	int platfcnt;
    int err;


    CHECK_CL(clGetPlatformIDs(30, platforms, &platfcnt));

    CHECK_CL(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &device_id, NULL));

    //context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);              CHECK_CL(err);
    cl_context_properties properties[] = {
    CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), // WGL Context
    CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), // WGL HDC
    CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0],// OpenCL platform
    0 };
    // Create a context using the supported devices
    context = clCreateContext(properties, 1, &device_id, NULL, 0, &err); CHECK_CL(err);
    printf("OK\n");

    GLuint gl_color_buf;
	glGenBuffers(1, &gl_color_buf);                                             CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, gl_color_buf);                                CHECK_GL_ERRORS
	glBufferData(GL_ARRAY_BUFFER, patchCount * sizeof(float4), 0, GL_STATIC_DRAW); CHECK_GL_ERRORS

    //glBindBuffer(GL_ARRAY_BUFFER, 0);

	computeRadiosityCL(2, gl_color_buf, context, device_id);

    glBindBuffer(GL_ARRAY_BUFFER_ARB, gl_color_buf);

	int vertexOffsetPosition = 0;
	int vertexOffsetColor = 0;
	GLint positionLocation, colorLocation, sideLocation, normalsLocation;

    colorLocation = glGetAttribLocation(shaderProgram, "color");                 CHECK_GL_ERRORS
	if (colorLocation != -1)
	{
			glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, sizeof(float4),
                         (const GLvoid*)vertexOffsetColor);                      CHECK_GL_ERRORS
			glEnableVertexAttribArray(colorLocation);                            CHECK_GL_ERRORS
	}

    int pt = 0;

    for (int i = 0; i < polygonCount; ++i) {
		//if (i == 5) continue;
		for (int i1 = 0; i1 < pt_poly[i].axis1; ++i1) {
			for (int j1 = 0; j1 < pt_poly[i].axis2; ++j1) {
                if (i != 5) {
                    int pt_ind = ptindoffsets[i] + pt_poly[i].axis2 * i1 + j1;
                    point ct = pt_poly[i].patches[i1][j1].vertex[0];
                    /*colors[pt * COLOR_COUNT] = (float)pow(radio[pt_ind].deposit.x,
                                                             1.0 / 2);
                    colors[pt * COLOR_COUNT + 1] = (float)pow(radio[pt_ind].deposit.y,
                                                             1.0 / 2);
                    colors[pt * COLOR_COUNT + 2] = (float)pow(radio[pt_ind].deposit.z,
                                                             1.0 / 2);*/
                    /*colors[pt * COLOR_COUNT] = (float)pow(deposit[pt_ind].x, 1.0 / 2);
                    colors[pt * COLOR_COUNT + 1] = (float)pow(deposit[pt_ind].y, 1.0 / 2);
                    colors[pt * COLOR_COUNT + 2] = (float)pow(deposit[pt_ind].z, 1.0 / 2);*/
                    coords[pt * COORDS_COUNT] = (float)ct.y;
                    coords[pt * COORDS_COUNT + 1] = (float)ct.z;
                    coords[pt * COORDS_COUNT + 2] = (float)ct.x - 3.0f;
                    point side = sub(pt_poly[i].patches[i1][j1].vertex[1],
                                     pt_poly[i].patches[i1][j1].vertex[0]);
                    //printf("%f %f %f\n", side.x, side.y, side.z);
                    sides[pt * COORDS_COUNT] = (float)side.y;
                    sides[pt * COORDS_COUNT + 1] = (float)side.z;
                    sides[pt * COORDS_COUNT + 2] = (float)side.x;
                    normals[pt * COORDS_COUNT] = (float)poly[i].normal.y;
                    normals[pt * COORDS_COUNT + 1] = (float)poly[i].normal.z;
                    normals[pt * COORDS_COUNT + 2] = (float)poly[i].normal.x;
                }
                pt++;
			}
        }
    }

    glGenBuffers(2, &meshVBO);                                                   CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);                                      CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*coords) * COORDS_COUNT, coords,
                 GL_STATIC_DRAW);                                                CHECK_GL_ERRORS


	positionLocation = glGetAttribLocation(shaderProgram, "position");           CHECK_GL_ERRORS
	if (positionLocation != -1)
	{
			glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*coords) * COORDS_COUNT,
                         (const GLvoid*)vertexOffsetPosition);                   CHECK_GL_ERRORS
			glEnableVertexAttribArray(positionLocation);                         CHECK_GL_ERRORS
	}

	/*glGenBuffers(2, &meshVBO2);                                                  CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO2);                                     CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*colors) * COLOR_COUNT, colors,
                 GL_STATIC_DRAW);                                                CHECK_GL_ERRORS


	colorLocation = glGetAttribLocation(shaderProgram, "color");                 CHECK_GL_ERRORS
	if (colorLocation != -1)
	{
			glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*colors) * COLOR_COUNT,
                         (const GLvoid*)vertexOffsetColor);                      CHECK_GL_ERRORS
			glEnableVertexAttribArray(colorLocation);                            CHECK_GL_ERRORS
	}*/

	glGenBuffers(3, &meshVBO);                                                   CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);                                      CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*sides) * COORDS_COUNT, sides,
                 GL_STATIC_DRAW);                                                CHECK_GL_ERRORS


	sideLocation = glGetAttribLocation(shaderProgram, "side");                   CHECK_GL_ERRORS
	if (sideLocation != -1)
	{
			glVertexAttribPointer(sideLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*sides) * COORDS_COUNT, (const GLvoid*)0);            CHECK_GL_ERRORS
			glEnableVertexAttribArray(sideLocation);                             CHECK_GL_ERRORS
	}

	glGenBuffers(4, &meshVBO);                                                   CHECK_GL_ERRORS
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);                                      CHECK_GL_ERRORS
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*normals) * COORDS_COUNT, normals,
                 GL_STATIC_DRAW);                                                CHECK_GL_ERRORS


	normalsLocation = glGetAttribLocation(shaderProgram, "normal");              CHECK_GL_ERRORS
	if (normalsLocation != -1)
	{
			glVertexAttribPointer(normalsLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*normals) * COORDS_COUNT, (const GLvoid*)0);          CHECK_GL_ERRORS
			glEnableVertexAttribArray(normalsLocation);                         CHECK_GL_ERRORS
	}

    free(colors);
    free(coords);
    free(sides);
    free(normals);



	//Set uniforms
	// массив для хранения перспективной матрици проекции
	float projectionMatrix[16];

	// переменная для хранения индекса юниформа
	GLint projectionMatrixLocation;

	// кожффициент отношения сторон окна OpenGL
	const float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;


	// создадим перспективную матрицу проекции
	Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 0.0f, 5.0f);

	// получим индекс юниформа projectionMatrix из шейдерной программы
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix"); CHECK_GL_ERRORS

	// передадим матрицу в шейдер
	if (projectionMatrixLocation != -1)
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix);CHECK_GL_ERRORS


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                          CHECK_GL_ERRORS

	// делаем шейдерную программу активной
	glUseProgram(shaderProgram);                                                 CHECK_GL_ERRORS

	// для рендеринга исопльзуем VAO
	glBindVertexArray(meshVAO);                                                  CHECK_GL_ERRORS

	// рендер треугольника из VBO привязанного к VAO
	glPointSize(2.0f);                                                           CHECK_GL_ERRORS
	glDrawArrays(GL_POINTS, 0, pt);                                              CHECK_GL_ERRORS
	SwapBuffers(hdc);                                                            CHECK_GL_ERRORS
}


int loadSource(char * shaderName, char **textOut, int *textLen) {
	FILE *input;

	input = fopen(shaderName, "r");

	fseek(input, 0, SEEK_END);
	*textLen = ftell(input);
	rewind(input);

	*textOut = calloc(sizeof(*textOut), *textLen + 1);

	*textLen = fread(*textOut, sizeof(**textOut), *textLen, input);
	close(input);
	return 1;
}

void Matrix4Perspective(float *M, float fovy, float aspect, float znear, float zfar)
{
        // fovy передается в градусах - сконвертируем его в радианы
        float f = 1 / tanf(fovy * M_PI / 360),
              A = (zfar + znear) / (znear - zfar),
              B = (2 * zfar * znear) / (znear - zfar);

        M[ 0] = f / aspect; M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
        M[ 4] = 0;          M[ 5] =  f; M[ 6] =  0; M[ 7] =  0;
        M[ 8] = 0;          M[ 9] =  0; M[10] =  A; M[11] =  B;
        M[12] = 0;          M[13] =  0; M[14] = -1; M[15] =  0;
}


///OPERATORS
///*****************************************************************************
inline point sub(point p1, point p2) {
    point res = {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
    return res;
}


inline point mult(point p, float k) {
    point res = {p.x * k, p.y * k, p.z * k};
    return res;
}


inline point sum(point p1, point p2) {
    point res = {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
    return res;
}


float square(polygon plg) {
	float res = 0;
	for (int i = 2; i < plg.length; ++i) {
        res += length(multV(sub(plg.vertex[i], plg.vertex[0]),
							sub(plg.vertex[i - 1], plg.vertex[0])));
	}
	return res / 2;
}


point multV(point p1, point p2) {
    point res = {p1.y * p2.z - p1.z * p2.y,
				 p1.z * p2.x - p1.x * p2.z,
				 p1.x * p2.y - p1.y * p2.x};
	return res;
}


inline float length(point p) {
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

//Unused!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
point normal(polygon p) {
	point res = multV(sub(p.vertex[1], p.vertex[0]),
					sub(p.vertex[2], p.vertex[0]));
    return mult(res, 1.0 / length(res));
}


point center(patch p) {
    point c = {0, 0, 0};
    for (int i = 0; i < p.length; ++i) {
        c = sum(c, p.vertex[i]);
    }
    return mult(c, 1.0 / p.length);
}


point randomPointInSquare(polygon p) {
    point p1 = sub(p.vertex[1], p.vertex[0]);
    point p2 = sub(p.vertex[3], p.vertex[0]);
    float l1 = (float)rand() / RAND_MAX;
    float l2 = (float)rand() / RAND_MAX;
    return sum(p.vertex[0], sum(mult(p1, l1), mult(p2, l2)));
}

point polarizePointInPolygon(polygon pl, point pnt) {
    point c = center(pl);
    return sum(c, sub(c, pnt));
}

point randomPoint(patch p) {
    float * weightes = calloc(p.length, sizeof(*weightes));
    float s = 0;
    for (int i = 0; i < p.length; ++i) {
        weightes[i] = rand();
        s += weightes[i];
    }
    for (int i = 0; i < p.length; ++i) {
        weightes[i] /= s;
    }
    point c = center(p);
    point res = c;
    for (int i = 0; i < p.length; ++i) {
        res = sum(res, mult(sub(p.vertex[i], c), weightes[i]));
    }
    free(weightes);
    return res;
}


float multS(point p1, point p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}


float cosV(point p1, point p2) {
	return multS(p1, p2) / length(p1) / length(p2);
}


int inPolygon(polygon pl, point p) {
	float sq = 0;
	for (int i = 1; i <= pl.length; ++i) {
        sq += length(multV(sub(pl.vertex[i % pl.length], p),
							sub(pl.vertex[i - 1], p)));
	}
	return fabs(sq / 2 - square(pl)) < DBL_EPSILON;
}


float distance(polygon pl, point p) {
    float d = -multS(pl.normal, pl.vertex[0]);
    return multS(pl.normal, p) + d;
}


int checkIntersection(polygon pl, point p1, point p2) {
    point aug = sub(p2, p1);
    float d = -multS(pl.normal, pl.vertex[0]);
    float t = multS(pl.normal, aug);
    if (fabs(t) < DBL_EPSILON) return 0;
    t =  -(d + multS(pl.normal, p1)) / t;
    if (t <= 0 || t > 1) {
        return 0;
    }
    return inPolygon(pl, sum(p1, mult(aug, t)));
}
