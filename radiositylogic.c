#include"radiositylogic.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <float.h>
#include <gl/gl.h>

#define OPTIMIZE_OUTPUT

#ifdef OPTIMIZE_OUTPUT
#include <time.h>
#endif

#ifdef OPTIMIZE_OUTPUT1
long long count[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif // OPTIMIZE_OUTPUT

int radiosityMain(HDC hdc) {
	#ifdef OPTIMIZE_OUTPUT
	clock_t tm = clock();
	#endif // OPTIMIZE_OUTPUT
	int PLG = 6;
	//srand(1488);
    polygon * hard = hardcodedPolygons();
    patched_polygon plgs[PLG];
    int k = 11;
    #ifdef OPTIMIZE_OUTPUT
    printf("Time for prepare scene: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT


	for (int i = 0; i < PLG; ++i) {
        plgs[i] = getPatchesFromQuadrangle(hard[i], k);
	}

	#ifdef NYAN_CAT
	plgs[0] = getPatchesFromQuadrangle(hard[0], 33);
	#endif
	#ifdef OPTIMIZE_OUTPUT
    printf("Time for create patches: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    double ** ff = computeFormFactorForScene(plgs, PLG);

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for compute form-factors: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

	#ifdef OPTIMIZE_OUTPUT1
    for (int i = 0; i < 11; ++i) {
        printf("Count[%d]: %lld\n", i, count[i]);
    }
	#endif // OPTIMIZE_OUTPUT

	int sideSize = 0;
    for (int i = 0; i < PLG; ++i) {
        sideSize += plgs[i].axis1 * plgs[i].axis2;
    }

	double mx = 0, mn = 2, dif = 0;
	for (int i = 0; i < sideSize; ++i) {
		double res = 0;
        for (int j = 0; j < sideSize; ++j) {
            res += ff[i][j];
            if (i == 130) {
                printf("%lf ", ff[i][j]);
            }
        }
		printf("FF str %d: %lf\n", i, res);
		mx = max(mx, res);
		mn = min(mn, res);
		dif += res;
	}
	printf("OK %lf %lf %lf\n", mn, dif / sideSize, mx);
	//printf("%lf %lf\n", ff[130][30], ff[30][130]);

    patch_radiosity * rad = calloc(sideSize, sizeof(*rad));
    rad[k * k * 4 + k * k / 2].emmision.x = k * k;
    rad[k * k * 4 + k * k / 2].emmision.y = k * k;
    rad[k * k * 4 + k * k / 2].emmision.z = k * k;
    for (int i = 0; i < sideSize; ++i) {
        rad[i].reflectance.x = 0.75;
        rad[i].reflectance.y = 0.75;
        rad[i].reflectance.z = 0.75;
    }
    for (int i = k * k; i < 2 * k * k; ++i) {
        rad[i].reflectance.x = 1;
        rad[i].reflectance.y = 0;
        rad[i].reflectance.z = 0;
    }
    for (int i = 2 * k * k; i < 3 * k * k; ++i) {
        rad[i].reflectance.x = 0;
        rad[i].reflectance.y = 1;
        rad[i].reflectance.z = 0;
    }
    //NYAN CAT!!!!!!!!!!!!!!!!!!!!!!!!!!!
	#ifdef NYAN_CAT
    for (int i = 0; i < 33 * 33; ++i) {
		rad[i].reflectance.x = 16.0 / 255;
		rad[i].reflectance.y = 68.0 / 255;
		rad[i].reflectance.z = 118.0 / 255;
    }

    int blackX[] = {5, 6, 7, 11, 12, 13, 21, 22, 23, 26, 27, 28,
					5, 8, 10, 13, 20, 23, 25, 28,
					5, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
					6, 7, 19, 30,
					6, 18, 22, 23, 24, 25, 26, 27, 28, 31,
					6, 17, 22, 25, 28, 32,
					4, 5, 6, 17, 32,
					2, 3, 6, 17, 21, 22, 26, 28, 29, 32,
					1, 6, 17, 22, 29, 32,
					0, 3, 4, 5, 6, 17, 32,
					0, 3, 6, 18, 31,
					1, 2, 6, 18, 23, 24, 25, 26, 31,
					6, 18, 22, 26, 27, 31,
					6, 18, 21, 26, 28, 31,
					6, 19, 20, 26, 29, 30,
					6, 26,
					6, 26,
					6, 26,
					7, 25,
					8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    int blackY[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					1, 1, 1, 1, 1, 1, 1, 1,
					2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
					3, 3, 3, 3,
					4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
					5, 5, 5, 5, 5, 5,
					6, 6, 6, 6, 6,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					8, 8, 8, 8, 8, 8,
					9, 9, 9, 9, 9, 9, 9,
					10, 10, 10, 10, 10,
					11, 11, 11, 11, 11, 11, 11, 11, 11,
					12, 12, 12, 12, 12, 12,
					13, 13, 13, 13, 13, 13,
					14, 14, 14, 14, 14, 14,
					15, 15,
					16, 16,
					17, 17,
					18, 18,
					19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19};
    setColor(rad, blackX, blackY, 12 + 8 + 23 + 4 + 10 + 6 + 5 + 10 + 6 + 7 + 5 + 9 + 6 + 6 + 6 + 2 + 2 + 2 + 2 + 17, 0, 0, 0);
	int grayX[] = {6, 7, 11, 12, 21, 22, 26, 27,
					6, 7,
					20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
					19, 20, 21, 29, 30,
					18, 21, 23, 24, 26, 27, 29,
					18, 21, 22, 23, 24, 25, 26, 27, 28, 29,
					4, 5, 18, 19, 20, 23, 24, 25, 27, 30, 31,
					2, 3, 4, 5, 18, 19, 20, 23, 24, 25, 26, 27, 30, 31,
					1, 2, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
					1, 2, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
					19, 20, 21, 22, 27, 28, 29, 30,
					19, 20, 21, 28, 29, 30,
					19, 20, 29, 30};
	int grayY[] = {1, 1, 1, 1, 1, 1, 1, 1,
					2, 2,
					3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
					4, 4, 4, 4, 4,
					5, 5, 5, 5, 5, 5, 5,
					6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
					9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
					10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
					11, 11, 11, 11, 11, 11, 11, 11,
					12, 12, 12, 12, 12, 12,
					13, 13, 13, 13};
	setColor(rad, grayX, grayY, 8 + 2 + 10 + 5 + 7 + 10 + 11 + 14 + 16 + 14 + 8 + 6 + 4, 153.0 / 255, 153.0 / 255, 153.0 / 255);
	int yellowX[] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
					7, 8, 9,
					7, 8,
					7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					25, 25, 25, 25,
					7, 8, 24, 25,
					7, 8, 9, 23, 24, 25,
					8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
	int yellowY[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
					4, 4, 4,
					5, 5,
					6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
					12, 13, 14, 15,
					16, 16, 16, 16,
					17, 17, 17, 17, 17, 17,
					18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18};
	setColor(rad, yellowX, yellowY, 11 + 3 + 2 + 10 + 4 + 4 + 6 + 17, 253.0 / 255, 204.0 / 255, 153.0 / 255);

	setLine(rad, 10, 17, 4, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 9, 16, 5, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 16, 6, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 16, 7, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 16, 8, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 16, 9, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 17, 10, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 17, 11, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 17, 12, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 17, 13, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 18, 14, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 8, 24, 15, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 9, 23, 16, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 10, 22, 17, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 23, 24, 12, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 22, 24, 13, 1.0, 153.0 / 255, 1.0);
	setLine(rad, 21, 24, 14, 1.0, 153.0 / 255, 1.0);

	int pinkX[] = {10, 13, 9, 15, 11, 14, 22, 10, 15, 18};
	int pinkY[] = {5, 6, 8, 9, 10, 12, 14, 15, 16, 16};
	setColor(rad, pinkX, pinkY, 10, 238.0 / 255, 54.0 / 255, 150.0 / 255);

	int fpinkX[] = {19, 20, 19, 20, 30, 31, 30, 31};
	int fpinkY[] = {5, 5, 6, 6, 5, 5, 6, 6};
	setColor(rad, fpinkX, fpinkY, 8, 245.0 / 255, 151.0 / 255, 153.0 / 255);

	int whiteX[] = {21, 28};
	int whiteY[] = {8, 8};
	setColor(rad, whiteX, whiteY, 2, 1.0, 1.0, 1.0);
	#endif
    //END NYAN CAT
    /*for (int i = 4 * k * k + 12; i < 5 * k * k; i += k * k) {
        rad[i].emmision.x = k * k / 10.0;
        rad[i].emmision.y = k * k / 10.0;
        rad[i].emmision.z = k * k / 10.0;
    }*/
    #ifdef OPTIMIZE_OUTPUT
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    computeRadiosity(rad, ff, sideSize, 2);

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for compute radiosity: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT
	/*for (int i = 0; i < k *k * 6; ++i) {
        printf("Explicit: %lf %lf %lf\n", rad[i].excident.x, rad[i].excident.y, rad[i].excident.z);
	}*/

	#ifdef OPTIMIZE_OUTPUT
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    drawScene(hard, plgs, PLG, rad, sideSize, hdc);

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for draw scene: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

}

void setColor(patch_radiosity * rad, int *X, int *Y, int count, double R, double G, double B) {
    for (int i = 0; i < count; ++i) {
		rad[X[i] + (Y[i] + 6) * 33].reflectance.x = R;
		rad[X[i] + (Y[i] + 6) * 33].reflectance.y = G;
		rad[X[i] + (Y[i] + 6) * 33].reflectance.z = B;
    }
}

void setLine(patch_radiosity * rad, int X1, int X2, int Y, double R, double G, double B) {
	for (int i = X1; i <= X2; ++i) {
		rad[i + (Y + 6) * 33].reflectance.x = R;
		rad[i + (Y + 6) * 33].reflectance.y = G;
		rad[i + (Y + 6) * 33].reflectance.z = B;
	}
}


polygon * hardcodedPolygons() {
	polygon * plgs = calloc(6, sizeof(*plgs));

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

	return plgs;

	plgs[6].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[6].normal.x = -1;
    plgs[6].normal.y = 0;
    plgs[6].normal.z = 0;
    plgs[6].length = POINTS_IN_QUADRANGLE;
	plgs[6].vertex[0].x = 0.5;
    plgs[6].vertex[0].y = -0.5;
    plgs[6].vertex[0].z = -0.75;
    plgs[6].vertex[1].x = 0.5;
    plgs[6].vertex[1].y = -0.5;
    plgs[6].vertex[1].z = 0.25;
    plgs[6].vertex[2].x = 0.5;
    plgs[6].vertex[2].y = 0.5;
    plgs[6].vertex[2].z = 0.25;
    plgs[6].vertex[3].x = 0.5;
    plgs[6].vertex[3].y = 0.5;
    plgs[6].vertex[3].z = -0.75;

    plgs[7].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[7].normal.x = 1;
    plgs[7].normal.y = 0;
    plgs[7].normal.z = 0;
    plgs[7].length = POINTS_IN_QUADRANGLE;
	plgs[7].vertex[0].x = -0.5;
    plgs[7].vertex[0].y = -0.5;
    plgs[7].vertex[0].z = -0.75;
    plgs[7].vertex[1].x = -0.5;
    plgs[7].vertex[1].y = -0.5;
    plgs[7].vertex[1].z = 0.25;
    plgs[7].vertex[2].x = -0.5;
    plgs[7].vertex[2].y = 0.5;
    plgs[7].vertex[2].z = 0.25;
    plgs[7].vertex[3].x = -0.5;
    plgs[7].vertex[3].y = 0.5;
    plgs[7].vertex[3].z = -0.75;

    //Left wall
	plgs[8].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[8].normal.x = 0;
    plgs[8].normal.y = 1;
    plgs[8].normal.z = 0;
    plgs[8].length = POINTS_IN_QUADRANGLE;
	plgs[8].vertex[0].x = 0.5;
    plgs[8].vertex[0].y = -0.5;
    plgs[8].vertex[0].z = -0.75;
    plgs[8].vertex[1].x = 0.5;
    plgs[8].vertex[1].y = -0.5;
    plgs[8].vertex[1].z = 0.25;
    plgs[8].vertex[2].x = -0.5;
    plgs[8].vertex[2].y = -0.5;
    plgs[8].vertex[2].z = 0.25;
    plgs[8].vertex[3].x = -0.5;
    plgs[8].vertex[3].y = -0.5;
    plgs[8].vertex[3].z = -0.75;

    //Right wall
	plgs[9].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[9].normal.x = 0;
    plgs[9].normal.y = -1;
    plgs[9].normal.z = 0;
    plgs[9].length = POINTS_IN_QUADRANGLE;
	plgs[9].vertex[0].x = 0.5;
    plgs[9].vertex[0].y = 0.5;
    plgs[9].vertex[0].z = -0.75;
    plgs[9].vertex[1].x = -0.5;
    plgs[9].vertex[1].y = 0.5;
    plgs[9].vertex[1].z = -0.75;
    plgs[9].vertex[2].x = -0.5;
    plgs[9].vertex[2].y = 0.5;
    plgs[9].vertex[2].z = 0.25;
    plgs[9].vertex[3].x = 0.5;
    plgs[9].vertex[3].y = 0.5;
    plgs[9].vertex[3].z = 0.25;

    //Bottom wall
	plgs[10].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[10].normal.x = 0;
    plgs[10].normal.y = 0;
    plgs[10].normal.z = 1;
    plgs[10].length = POINTS_IN_QUADRANGLE;
	plgs[10].vertex[0].x = 0.5;
    plgs[10].vertex[0].y = -0.5;
    plgs[10].vertex[0].z = -0.75;
    plgs[10].vertex[1].x = -0.5;
    plgs[10].vertex[1].y = -0.5;
    plgs[10].vertex[1].z = -0.75;
    plgs[10].vertex[2].x = -0.5;
    plgs[10].vertex[2].y = 0.5;
    plgs[10].vertex[2].z = -0.75;
    plgs[10].vertex[3].x = 0.5;
    plgs[10].vertex[3].y = 0.5;
    plgs[10].vertex[3].z = -0.75;

	//Top wall
	plgs[11].vertex = calloc(POINTS_IN_QUADRANGLE, sizeof(point));
    plgs[11].normal.x = 0;
    plgs[11].normal.y = 0;
    plgs[11].normal.z = -1;
    plgs[11].length = POINTS_IN_QUADRANGLE;
	plgs[11].vertex[0].x = 0.5;
    plgs[11].vertex[0].y = -0.5;
    plgs[11].vertex[0].z = 0.25;
    plgs[11].vertex[1].x = -0.5;
    plgs[11].vertex[1].y = -0.5;
    plgs[11].vertex[1].z = 0.25;
    plgs[11].vertex[2].x = -0.5;
    plgs[11].vertex[2].y = 0.5;
    plgs[11].vertex[2].z = 0.25;
    plgs[11].vertex[3].x = 0.5;
    plgs[11].vertex[3].y = 0.5;
    plgs[11].vertex[3].z = 0.25;

	return plgs;
}

patched_polygon getPatchesFromQuadrangle(polygon plg, int amountOfPatches) {
    //Initialize step vectors
    point stepV[3];
    for (int i = 1; i < 4; ++i) {
        stepV[i - 1] = mult(sub(plg.vertex[i], plg.vertex[0]),
                             1.0 / amountOfPatches);
    }

    //Create matrix of polygons
    patch ** patches = calloc(amountOfPatches, sizeof(*patches));
    for (int i = 0; i < amountOfPatches; ++i) {
        patches[i] = calloc(amountOfPatches, sizeof(*patches[i]));
        for (int j = 0; j < amountOfPatches; ++j) {
            patches[i][j].vertex = calloc(POINTS_IN_QUADRANGLE,
                                          sizeof(*patches[i][j].vertex));
			patches[i][j].length = POINTS_IN_QUADRANGLE;
			patches[i][j].normal = plg.normal;
            if (i == 0) {
                if (j == 0) {
                    patches[i][j].vertex[0] = plg.vertex[0];
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
    patched_polygon result = {patches, amountOfPatches, amountOfPatches};
    return result;
}


double ** computeFormFactorForScene(patched_polygon * plgs, int polygonCount) {
	//Initialize Form-Factor
    double ** FF;
    int sideSize = 0;
    for (int i = 0; i < polygonCount; ++i) {
        sideSize += plgs[i].axis1 * plgs[i].axis2;
    }
    FF = calloc(sideSize, sizeof(*FF));
    for (int i = 0; i < sideSize; ++i) {
        FF[i] = calloc(sideSize, sizeof(*FF[i]));
    }


    int offset1 = 0;
    for (int i = 0; i < polygonCount; ++i) {
        int offset2 = offset1 + plgs[i].axis1 * plgs[i].axis2;
        for (int j = i + 1; j < polygonCount; ++j) {
			printf("%d %d\n", i, j);
            computeFormFactorForPolygons(plgs[i], plgs[j], FF, offset1, offset2);
            offset2 += plgs[j].axis1 * plgs[j].axis2;
        }
        offset1 += plgs[i].axis1 * plgs[i].axis2;
    }
    return FF;
}


int computeFormFactorForPolygons(patched_polygon p1, patched_polygon p2,
								double ** FF, int offset1, int offset2) {
    for (int i1 = 0; i1 < p1.axis1; ++i1) {
        for (int j1 = 0; j1 < p1.axis2; ++j1) {
            for (int i2 = 0; i2 < p2.axis1; ++i2) {
                for (int j2 = 0; j2 < p2.axis2; ++j2) {
                    FF[i1 * p1.axis2 + j1 + offset1][i2 * p2.axis2 + j2 + offset2] =
                    computeFormFactorForPatches(p1.patches[i1][j1], p2.patches[i2][j2]);
                    FF[i2 * p2.axis2 + j2 + offset2][i1 * p1.axis2 + j1 + offset1] =
						FF[i1 * p1.axis2 + j1 + offset1][i2 * p2.axis2 + j2 + offset2];
                    FF[i1 * p1.axis2 + j1 + offset1][i2 * p2.axis2 + j2 + offset2]
												/= square(p1.patches[i1][j1]);
                    FF[i2 * p2.axis2 + j2 + offset2][i1 * p1.axis2 + j1 + offset1]
												/= square(p2.patches[i2][j2]);
                }
            }
        }
    }
}


double computeFormFactorForPatches(patch p1, patch p2) {
	double result = 0;
    for (int i = 0; i < MONTE_KARLO_ITERATIONS_COUNT; ++i) {
		//TODO: add visibility function
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		double iter_res = 0;
        //point on_p1 = randomPoint(p1);
        //point on_p2 = randomPoint(p2);
        point on_p1 = randomPointInSquare(p1);
        point on_p2 = randomPointInSquare(p2);
        point r = sub(on_p1, on_p2);
        iter_res = cosV(r, p2.normal);
        iter_res *= cosV(mult(r, -1), p1.normal);
		iter_res /= length(r) * length(r);
		result += iter_res;
    }
    result /= MONTE_KARLO_ITERATIONS_COUNT;
    result /= M_PI;
	/*double iter_res = 0;
	point on_p1 = sum(p1.vertex[0], mult(sub(p1.vertex[2], p1.vertex[0]), 0.5));
	point on_p2 = sum(p2.vertex[0], mult(sub(p2.vertex[2], p2.vertex[0]), 0.5));
	point r = sub(on_p1, on_p2);
	iter_res = cosV(r, p2.normal);
	iter_res *= cosV(mult(r, -1), p1.normal);
	iter_res /= M_PI;
	iter_res /= length(r) * length(r);
	printf("dif: %lf\n", result - iter_res);*/
    return result * square(p1) * square(p2);
}


int computeRadiosity(patch_radiosity *result, double ** ff,
					int patchesCount, int iterCount) {
	//patch_radiosity *result = calloc(patchesCount, sizeof(*result));
	for (int i = 0; i < patchesCount; ++i)
	{
		result[i].excident = result[i].emmision;
		result[i].emmision.x = 0;
		result[i].emmision.y = 0;
		result[i].emmision.z = 0;
		result[i].deposit = sum(result[i].deposit, result[i].excident);
	}

	for (int iter = 0; iter < iterCount; iter++) {
		for (int i = 0; i < patchesCount; i++) {
			result[i].incident.x = 0;
			result[i].incident.y = 0;
			result[i].incident.z = 0;
			for (int j = 0; j < patchesCount; j++) {
				result[i].incident = sum(result[i].incident,
											mult(result[j].excident, ff[i][j]));
			}
		}
		for (int i = 0; i < patchesCount; ++i) {
			result[i].excident.x = result[i].incident.x * result[i].reflectance.x;
			result[i].excident.y = result[i].incident.y * result[i].reflectance.y;
			result[i].excident.z = result[i].incident.z * result[i].reflectance.z;
			result[i].deposit = sum(result[i].deposit, result[i].excident);
		}
	}
}


int drawScene(polygon * pls, patched_polygon * plgs, int polygonCount,
				patch_radiosity *radio, int patchCount, HDC hdc) {
	point cam_center = {1.5, 0, 0};
    point cam_top = {0.5, 0, 1};
    point cam_front = {0.5, 0, 0};
    point cam_right = {0.5, 1, 0};

	polygon screen = {calloc(3, sizeof(point)), sub(cam_front, cam_center), 3};
	screen.vertex[0] = cam_top;
	screen.vertex[1] = cam_front;
	screen.vertex[2] = cam_right;
	com_pare pares[polygonCount];
    for (int i = 0; i < polygonCount; ++i) {
        pares[i].num = i;
        pares[i].distance = -INFINITY;
        for (int j = 0; j < pls[i].length; ++j) {
            pares[i].distance = max(pares[i].distance,
									distance(screen, pls[i].vertex[j]));
        }
        pares[i].distance = pares[i].distance > 0 ? pares[i].distance : INFINITY;
    }
    qsort(pares, polygonCount, sizeof(*pares), compar);
    int offsets[polygonCount + 1];
    offsets[0] = 0;
    for (int i = 1; i <= polygonCount; ++i) {
        offsets[i] = offsets[i - 1] + plgs[i - 1].axis1 * plgs[i - 1].axis2;
    }
    double for_t = distance(screen, cam_center);
    for (int i = 0; i < polygonCount; ++i) {
		if (pares[i].distance == INFINITY) continue;
        patched_polygon loc_pol = plgs[pares[i].num];
		for (int i1 = 0; i1 < loc_pol.axis1; ++i1) {
			for (int j1 = 0; j1 < loc_pol.axis2; ++j1) {
				int pt_ind = offsets[pares[i].num] + i1 * loc_pol.axis2 + j1;

				/*glColor3f(radio[pt_ind].deposit.x,
						  radio[pt_ind].deposit.y,
						  radio[pt_ind].deposit.z);*/
				//Add gamma-correction
				glColor3f(pow(radio[pt_ind].deposit.x, 1.0 / 2),
						  pow(radio[pt_ind].deposit.y, 1.0 / 2),
						  pow(radio[pt_ind].deposit.z, 1.0 / 2));
				glBegin(GL_POLYGON);
				patch loc_pt = plgs[pares[i].num].patches[i1][j1];
				for (int i2 = 0; i2 < loc_pt.length; ++i2) {
					point aug = sub(loc_pt.vertex[i2], cam_center);
					point res_p = sub(sum(cam_center,
									mult(aug, -for_t / multS(aug,
									screen.normal))), cam_front);
                    double xcos = cosV(res_p, sub(cam_right, cam_front)) * length(res_p);
                    double ycos = cosV(res_p, sub(cam_top, cam_front)) * length(res_p);
                    glVertex3d(xcos / SCREEN_WIDTH * SCREEN_HEIGHT, ycos, 0);
				}
				glEnd();
			}
        }
    }
    SwapBuffers(hdc);
}


int compar (const void* p1, const void* p2) {
    com_pare cp1 = *(com_pare *)p1;
    com_pare cp2 = *(com_pare *)p2;
    if (cp2.distance < cp1.distance) {
		return -1;
    } else {
    	return 1;
    }
}



///OPERATORS
///*****************************************************************************
inline point sub(point p1, point p2) {
    point res = {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
    #ifdef OPTIMIZE_OUTPUT1
    count[0]++;
    #endif
    return res;
}


inline point mult(point p, double k) {
	#ifdef OPTIMIZE_OUTPUT1
	count[1]++;
	#endif
    point res = {p.x * k, p.y * k, p.z * k};
    return res;
}


inline point sum(point p1, point p2) {
	#ifdef OPTIMIZE_OUTPUT1
	count[2]++;
	#endif
    point res = {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
    return res;
}


double square(polygon plg) {
	#ifdef OPTIMIZE_OUTPUT1
	count[3]++;
	#endif
	double res = 0;
	for (int i = 2; i < plg.length; ++i) {
        res += length(multV(sub(plg.vertex[i], plg.vertex[0]),
							sub(plg.vertex[i - 1], plg.vertex[0])));
	}
	return res / 2;
}


point multV(point p1, point p2) {
	#ifdef OPTIMIZE_OUTPUT1
	count[4]++;
	#endif
    point res = {p1.y * p2.z - p1.z * p2.y,
				 p1.z * p2.x - p1.x * p2.z,
				 p1.x * p2.y - p1.y * p2.x};
	return res;
}


inline double length(point p) {
	#ifdef OPTIMIZE_OUTPUT1
	count[5]++;
	#endif // OPTIMIZE_OUTPUT1
	return sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
}

//Unused!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
point normal(polygon p) {
	#ifdef OPTIMIZE_OUTPUT1
	count[6]++;
	#endif // OPTIMIZE_OUTPUT1
	point res = multV(sub(p.vertex[1], p.vertex[0]),
					sub(p.vertex[2], p.vertex[0]));
    return mult(res, 1.0 / length(res));
}


point center(polygon p) {
	#ifdef OPTIMIZE_OUTPUT1
	count[7]++;
	#endif // OPTIMIZE_OUTPUT1
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
    //return sum(p.vertex[0], mult(sub(p.vertex[2], p.vertex[0]), 0.5));
    return sum(p.vertex[0], sum(mult(p1, l1), mult(p2, l2)));
}

point polarizePointInPolygon(polygon pl, point pnt) {
    point c = center(pl);
    return sum(c, sub(c, pnt));
}

point randomPoint(polygon p) {
	#ifdef OPTIMIZE_OUTPUT1
	count[8]++;
	#endif // OPTIMIZE_OUTPUT1
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
	#ifdef OPTIMIZE_OUTPUT1
	count[9]++;
	#endif // OPTIMIZE_OUTPUT1
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}


double cosV(point p1, point p2) {
	#ifdef OPTIMIZE_OUTPUT1
	count[10]++;
	#endif // OPTIMIZE_OUTPUT1
	return multS(p1, p2) / length(p1) / length(p2);
}


int inPolygon(polygon pl, point p) {
	double sq = 0;
	for (int i = 1; i <= pl.length; ++i) {
        sq += length(multV(sub(pl.vertex[i % pl.length], p),
							sub(pl.vertex[i - 1], p)));
	}
	return abs(sq / 2 - square(pl)) < DBL_EPSILON;
}


double distance(polygon pl, point p) {
    double d = -multS(pl.normal, pl.vertex[0]);
    return multS(pl.normal, p) + d;
}
