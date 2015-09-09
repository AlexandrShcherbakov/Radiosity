#include"radiositylogic.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <float.h>

//#include <GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include "GL/wglext.h"
#include "GL/glext.h"


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
    ff = calloc(patchCount, sizeof(*radio));
    for (int i = 0; i < patchCount; ++i) {
		ff[i] = calloc(patchCount, sizeof(*radio));
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
    plgs[4].vertex[1].x = -1;
    plgs[4].vertex[1].y = -1;
    plgs[4].vertex[1].z = 1;
    plgs[4].vertex[2].x = -1;
    plgs[4].vertex[2].y = 1;
    plgs[4].vertex[2].z = 1;
    plgs[4].vertex[3].x = 1;
    plgs[4].vertex[3].y = 1;
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
    plgs[6].vertex[1].y = -0.25;
    plgs[6].vertex[1].z = -0.25;
    plgs[6].vertex[2].x = -0.25;
    plgs[6].vertex[2].y = 0.25;
    plgs[6].vertex[2].z = -0.25;
    plgs[6].vertex[3].x = -0.25;
    plgs[6].vertex[3].y = 0.25;
    plgs[6].vertex[3].z = -0.75;

    //Left wall
	plgs[7].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[7].normal.x = 0;
    plgs[7].normal.y = -1;
    plgs[7].normal.z = 0;
    plgs[7].length = POINTS_IN_QUADRANGLE;
	plgs[7].vertex[0].x = 0.25;
    plgs[7].vertex[0].y = -0.25;
    plgs[7].vertex[0].z = -0.75;
    plgs[7].vertex[1].x = 0.25;
    plgs[7].vertex[1].y = -0.25;
    plgs[7].vertex[1].z = -0.25;
    plgs[7].vertex[2].x = -0.25;
    plgs[7].vertex[2].y = -0.25;
    plgs[7].vertex[2].z = -0.25;
    plgs[7].vertex[3].x = -0.5;
    plgs[7].vertex[3].y = -0.25;
    plgs[7].vertex[3].z = -0.75;

    //Right wall
	plgs[8].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[8].normal.x = 0;
    plgs[8].normal.y = 1;
    plgs[8].normal.z = 0;
    plgs[8].length = POINTS_IN_QUADRANGLE;
	plgs[8].vertex[0].x = -0.25;
    plgs[8].vertex[0].y = 0.25;
    plgs[8].vertex[0].z = -0.75;
    plgs[8].vertex[1].x = -0.25;
    plgs[8].vertex[1].y = 0.25;
    plgs[8].vertex[1].z = -0.25;
    plgs[8].vertex[2].x = 0.25;
    plgs[8].vertex[2].y = 0.25;
    plgs[8].vertex[2].z = -0.25;
    plgs[8].vertex[3].x = 0.25;
    plgs[8].vertex[3].y = 0.25;
    plgs[8].vertex[3].z = -0.75;

    //Bottom wall
	plgs[9].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[9].normal.x = 0;
    plgs[9].normal.y = 0;
    plgs[9].normal.z = -1;
    plgs[9].length = POINTS_IN_QUADRANGLE;
	plgs[9].vertex[0].x = 0.25;
    plgs[9].vertex[0].y = -0.25;
    plgs[9].vertex[0].z = -0.75;
    plgs[9].vertex[1].x = -0.25;
    plgs[9].vertex[1].y = -0.25;
    plgs[9].vertex[1].z = -0.75;
    plgs[9].vertex[2].x = -0.25;
    plgs[9].vertex[2].y = 0.25;
    plgs[9].vertex[2].z = -0.75;
    plgs[9].vertex[3].x = 0.25;
    plgs[9].vertex[3].y = 0.25;
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
    plgs[12].vertex[1].x = -0.65;
    plgs[12].vertex[1].y = -0.9;
    plgs[12].vertex[1].z = -0.9 + sqrt(0.125);
    plgs[12].vertex[2].x = -0.9;
    plgs[12].vertex[2].y = -0.65;
    plgs[12].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[12].vertex[3].x = -0.9;
    plgs[12].vertex[3].y = -0.65;
    plgs[12].vertex[3].z = -0.9;


	//right-back wall
	plgs[13].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[13].normal.x = -sqrt(2.0) / 2.0;
	plgs[13].normal.y = sqrt(2.0) / 2.0;
	plgs[13].normal.z = 0;
	plgs[13].length = POINTS_IN_QUADRANGLE;
	plgs[13].vertex[0].x = -0.9;
    plgs[13].vertex[0].y = -0.65;
    plgs[13].vertex[0].z = -0.9;
    plgs[13].vertex[1].x = -0.9;
    plgs[13].vertex[1].y = -0.65;
    plgs[13].vertex[1].z = -0.9 + sqrt(0.125);
    plgs[13].vertex[2].x = -0.65;
    plgs[13].vertex[2].y = -0.4;
    plgs[13].vertex[2].z = -0.9 + sqrt(0.125);
    plgs[13].vertex[3].x = -0.65;
    plgs[13].vertex[3].y = -0.4;
    plgs[13].vertex[3].z = -0.9;


    //bottom wall
	plgs[14].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
	plgs[14].normal.x = 0;
	plgs[14].normal.y = 0;
	plgs[14].normal.z = -1;
	plgs[14].length = POINTS_IN_QUADRANGLE;
	plgs[14].vertex[0].x = -0.65;
    plgs[14].vertex[0].y = -0.9;
    plgs[14].vertex[0].z = -0.9;
    plgs[14].vertex[1].x = -0.9;
    plgs[14].vertex[1].y = -0.65;
    plgs[14].vertex[1].z = -0.9;
    plgs[14].vertex[2].x = -0.65;
    plgs[14].vertex[2].y = -0.4;
    plgs[14].vertex[2].z = -0.9;
    plgs[14].vertex[3].x = -0.4;
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
    for (int i = 0; i < polygonCount; ++i) {
        for (int j = i + 1; j < polygonCount; ++j) {
            computeFormFactorForPolygons(i, j);
            printf("%d %d\n", i, j);
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


double computeFormFactorForPatches(patch p1, patch p2, int pl1, int pl2) {
	double result = 0;
	int cnt = 0;
    for (int i = 0; i < MONTE_KARLO_ITERATIONS_COUNT; ++i) {
		double iter_res = 0;

		//Hammersley Point Set
		float u = 0;
		int kk = i;

		for (float p = 0.5; kk; p *= 0.5, kk >>= 1)
			if (kk & 1)
				u += p;

		float v = (i + 0.5) / MONTE_KARLO_ITERATIONS_COUNT;

        point on_p1 = sum(p1.vertex[0], sum(mult(sub(p1.vertex[1], p1.vertex[0]), u), mult(sub(p1.vertex[3], p1.vertex[0]), v)));
		point on_p2 = sum(p2.vertex[0], sum(mult(sub(p2.vertex[1], p2.vertex[0]), u), mult(sub(p2.vertex[3], p2.vertex[0]), v)));

		point r = sub(on_p1, on_p2);
		double lr = length(r);
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
    result /= MONTE_KARLO_ITERATIONS_COUNT - cnt;
    result /= M_PI;
	result *= square(p1) * square(p2);
    return result;
}


int computeRadiosity(int iterCount) {
	for (int i = 0; i < patchCount; ++i)
	{
		radio[i].excident = radio[i].emmision;
		radio[i].emmision.x = 0;
		radio[i].emmision.y = 0;
		radio[i].emmision.z = 0;
		radio[i].deposit = sum(radio[i].deposit, radio[i].excident);
	}

	for (int iter = 0; iter < iterCount; iter++) {
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
	return 1;
}


int drawScene(HDC hdc) {

	const int magic_const = 256;
    static int light = magic_const * 4;
    radio[light].emmision.x = 0;
    radio[light].emmision.y = 0;
    radio[light].emmision.z = 0;
    for (int i = 0; i < patchCount; ++i) {
        radio[i].deposit.x = 0;
        radio[i].deposit.y = 0;
        radio[i].deposit.z = 0;
    }
    light = (light + 1) % magic_const + magic_const * 4;
    radio[light].emmision.x = magic_const;
    radio[light].emmision.y = magic_const;
    radio[light].emmision.z = magic_const;
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

int initShaders() {
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

	GLuint vertexShader, fragmentShader;

	// переменные для хранения загруженных файлов
	char *shaderSource;
	int sourceLength;

	// создадим шейдерную программу и шейдеры для нее
	shaderProgram  = glCreateProgram();
	vertexShader   = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);


	// загрузим вершинный шейдер
	loadShader("shaders/vertex shader", &shaderSource, &sourceLength);

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(vertexShader, 1, (const GLchar**)&shaderSource, (const GLint*)&sourceLength);
	glCompileShader(vertexShader);

	if (ShaderStatus(vertexShader, GL_COMPILE_STATUS) != GL_TRUE)
        printf("Compile vertex shader error\n");

	free(shaderSource);

	// загрузим фрагментный шейдер
	loadShader("shaders/fragment shader", &shaderSource, &sourceLength);

	// зададим шейдеру исходный код и скомпилируем его
	glShaderSource(fragmentShader, 1, (const GLchar**)&shaderSource, (const GLint*)&sourceLength);
	glCompileShader(fragmentShader);

	if (ShaderStatus(fragmentShader, GL_COMPILE_STATUS) != GL_TRUE)
        printf("Compile frag shader error\n");

	free(shaderSource);

	// присоединим загруженные шейдеры к шейдерной программе
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);


	// сборка шейдерной программы из прикрепленных шейдеров
	glLinkProgram(shaderProgram);

	if (ShaderProgramStatus(shaderProgram, GL_LINK_STATUS) != GL_TRUE)
        printf("Link shaders error\n");

	// сделаем шейдерную программу активной
	glUseProgram(shaderProgram);

	glValidateProgram(shaderProgram);
	if (ShaderProgramStatus(shaderProgram, GL_VALIDATE_STATUS) != GL_TRUE)
		printf("Link shaders error\n");
	return 1;
}


int loadShader(char * shaderName, char **textOut, int *textLen) {
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

int setCenters() {
	GLuint meshVBO;
	GLuint meshVBO2;
	glGenVertexArrays(1, &meshVAO);
	glBindVertexArray(meshVAO);

    float * colors = calloc(patchCount * COLOR_COUNT, sizeof(*colors));
    float * coords = calloc(patchCount * COORDS_COUNT, sizeof(*coords));

	const int magic_const = 256;
    static int light = magic_const * 4;
    radio[light].emmision.x = 0;
    radio[light].emmision.y = 0;
    radio[light].emmision.z = 0;
    for (int i = 0; i < patchCount; ++i) {
        radio[i].deposit.x = 0;
        radio[i].deposit.y = 0;
        radio[i].deposit.z = 0;
    }
    light = (light + 1) % magic_const + magic_const * 4;
    radio[light].emmision.x = magic_const;
    radio[light].emmision.y = magic_const;
    radio[light].emmision.z = magic_const;
	computeRadiosity(2);

    int pt = 0;

    for (int i = 0; i < polygonCount; ++i) {
		if (i == 5) continue;
		for (int i1 = 0; i1 < pt_poly[i].axis1; ++i1) {
			for (int j1 = 0; j1 < pt_poly[i].axis2; ++j1) {
				int pt_ind = ptindoffsets[i] + pt_poly[i].axis2 * i1 + j1;

                colors[pt * COLOR_COUNT] = (float)pow(radio[pt_ind].deposit.x,
														 1.0 / 2);
				colors[pt * COLOR_COUNT + 1] = (float)pow(radio[pt_ind].deposit.y,
														 1.0 / 2);
				colors[pt * COLOR_COUNT + 2] = (float)pow(radio[pt_ind].deposit.z,
														 1.0 / 2);
				point ct = center(pt_poly[i].patches[i1][j1]);
				coords[pt * COORDS_COUNT] = (float)ct.y;
				coords[pt * COORDS_COUNT + 1] = (float)ct.z;
				coords[pt * COORDS_COUNT + 2] = (float)ct.x - 3.0f;
				pt++;
			}
        }
    }

    int vertexOffsetPosition = 0;
	int vertexOffsetColor = 0;3 * sizeof(float) * pt;
	GLint positionLocation, colorLocation;

    glGenBuffers(1, &meshVBO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*coords) * COORDS_COUNT, coords, GL_STATIC_DRAW);


	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
			glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*coords) * COORDS_COUNT, (const GLvoid*)vertexOffsetPosition);
			glEnableVertexAttribArray(positionLocation);
	}

	glGenBuffers(2, &meshVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO2);
    glBufferData(GL_ARRAY_BUFFER, pt * sizeof(*colors) * COLOR_COUNT, colors, GL_STATIC_DRAW);


	colorLocation = glGetAttribLocation(shaderProgram, "color");
	if (colorLocation != -1)
	{
			glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
					sizeof(*colors) * COLOR_COUNT, (const GLvoid*)vertexOffsetColor);
			glEnableVertexAttribArray(colorLocation);
	}

    free(colors);
    free(coords);
}

int setTriangle() {
	// количество вершин в нашей геометрии, у нас простой треугольник
const int vertexCount = 3;
	// размер одной вершины меша в байтах - 6 float на позицию и на цвет вершины
	const int vertexSize = 6 * sizeof(float);

	// подготовим данные для вывода треугольника, всего 3 вершины по 6 float на каждую
	static const float triangleMesh[3 * 6] = {
			/* 1 вершина, позиция: */ -0.5f, -0.5f, -2.0f, /* цвет: */ 1.0f, 0.0f, 0.0f,
			/* 2 вершина, позиция: */  0.0f,  0.5f, -2.0f, /* цвет: */ 0.0f, 1.0f, 0.0f,
			/* 3 вершина, позиция: */  0.5f, -0.5f, -2.0f, /* цвет: */ 0.0f, 0.0f, 1.0f
	};

	// переменные для хранения индексов VAO и VBO
	GLuint meshVBO;

	// создадим Vertex Array Object (VAO)
	glGenVertexArrays(1, &meshVAO);

	// установим созданный VAO как текущий
	glBindVertexArray(meshVAO);

	// создадим Vertex Buffer Object (VBO)
	glGenBuffers(1, &meshVBO);

	// устанавливаем созданный VBO как текущий
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);

	// заполним VBO данными треугольника
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, triangleMesh, GL_STATIC_DRAW);

	return 1;
}

int setAttrib() {

	const int vertexSize = 6 * sizeof(float);
	// переменные для хранения индексов атрибутов
	GLint positionLocation, colorLocation;

	// смещения данных внутри вершинного буфера
	const int vertexOffsetPosition = 0;
	const int vertexOffsetColor    = 3 * sizeof(float);


	// получим индекс атрибута 'position' из шейдера
	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
			// укажем параметры доступа вершинного атрибута к VBO
			glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE,
					vertexSize, (const GLvoid*)vertexOffsetPosition);
			// разрешим использование атрибута
			glEnableVertexAttribArray(positionLocation);
	}

	// получим индекс атрибута 'color' из шейдера
	colorLocation = glGetAttribLocation(shaderProgram, "color");
	if (colorLocation != -1)
	{
			// укажем параметры доступа вершинного атрибута к VBO
			glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE,
					vertexSize, (const GLvoid*)vertexOffsetColor);
			// разрешим использование атрибута
			glEnableVertexAttribArray(colorLocation);
	}
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

int setUniforms() {
	// массив для хранения перспективной матрици проекции
	float projectionMatrix[16];

	// переменная для хранения индекса юниформа
	GLint projectionMatrixLocation;

	// кожффициент отношения сторон окна OpenGL
	const float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;


	// создадим перспективную матрицу проекции
	Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 0.0f, 5.0f);

	// получим индекс юниформа projectionMatrix из шейдерной программы
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projectionMatrix");

	// передадим матрицу в шейдер
	if (projectionMatrixLocation != -1)
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_TRUE, projectionMatrix);
	return 1;
}


///OPERATORS
///*****************************************************************************
inline point sub(point p1, point p2) {
    point res = {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
    return res;
}


inline point mult(point p, double k) {
    point res = {p.x * k, p.y * k, p.z * k};
    return res;
}


inline point sum(point p1, point p2) {
    point res = {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
    return res;
}


double square(polygon plg) {
	double res = 0;
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


inline double length(point p) {
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
    double l1 = (double)rand() / RAND_MAX;
    double l2 = (double)rand() / RAND_MAX;
    return sum(p.vertex[0], sum(mult(p1, l1), mult(p2, l2)));
}

point polarizePointInPolygon(polygon pl, point pnt) {
    point c = center(pl);
    return sum(c, sub(c, pnt));
}

point randomPoint(patch p) {
    double * weightes = calloc(p.length, sizeof(*weightes));
    double s = 0;
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


double multS(point p1, point p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}


double cosV(point p1, point p2) {
	return multS(p1, p2) / length(p1) / length(p2);
}


int inPolygon(polygon pl, point p) {
	double sq = 0;
	for (int i = 1; i <= pl.length; ++i) {
        sq += length(multV(sub(pl.vertex[i % pl.length], p),
							sub(pl.vertex[i - 1], p)));
	}
	return fabs(sq / 2 - square(pl)) < DBL_EPSILON;
}


double distance(polygon pl, point p) {
    double d = -multS(pl.normal, pl.vertex[0]);
    return multS(pl.normal, p) + d;
}


int checkIntersection(polygon pl, point p1, point p2) {
    point aug = sub(p2, p1);
    double d = -multS(pl.normal, pl.vertex[0]);
    double t = multS(pl.normal, aug);
    if (fabs(t) < DBL_EPSILON) return 0;
    t =  -(d + multS(pl.normal, p1)) / t;
    if (t <= 0 || t > 1) {
        return 0;
    }
    return inPolygon(pl, sum(p1, mult(aug, t)));
}
