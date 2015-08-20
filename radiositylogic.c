#include"radiositylogic.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <float.h>
#include <gl/gl.h>
#include <gl/glu.h>

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

	polygonCount = 12;
    poly = hardcodedPolygons();
    pt_poly = calloc(polygonCount, sizeof(*pt_poly));
    ptindoffsets = calloc(polygonCount + 1, sizeof(*pt_poly));
    ptindoffsets[0] = 0;
    int k = 7;

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for prepare scene: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT


	for (int i = 0; i < polygonCount; ++i) {
        createPatchesFromQuadrangle(i, k);
        if (i > 0) {
            ptindoffsets[i] = ptindoffsets[i - 1] + pt_poly[i - 1].axis1 *
													pt_poly[i - 1].axis2;
        }
	}
    patchCount = ptindoffsets[polygonCount] = ptindoffsets[polygonCount - 1] +
			pt_poly[polygonCount - 1].axis1 * pt_poly[polygonCount - 1].axis2;

    radio = calloc(patchCount, sizeof(*radio));
    ff = calloc(patchCount, sizeof(*radio));
    for (int i = 0; i < patchCount; ++i) {
		ff[i] = calloc(patchCount, sizeof(*radio));
    }
	//printf("%lf %lf %lf\n", pt_poly[10].patches[10][10].vertex[0].x, pt_poly[10].patches[10][10].vertex[0].y, pt_poly[10].patches[10][10].vertex[0].z);
	//exit(0);
	#ifdef NYAN_CAT
	plgs[0] = getPatchesFromQuadrangle(hard[0], 33);
	#endif

	#ifdef OPTIMIZE_OUTPUT
    printf("Time for create patches: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    computeFormFactorForScene();

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for compute form-factors: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

	#ifdef OPTIMIZE_OUTPUT1
    for (int i = 0; i < 11; ++i) {
        printf("Count[%d]: %lld\n", i, count[i]);
    }
	#endif // OPTIMIZE_OUTPUT

    radio[k * k * 4 + k * k / 2].emmision.x = k * k;
    radio[k * k * 4 + k * k / 2].emmision.y = k * k;
    radio[k * k * 4 + k * k / 2].emmision.z = k * k;
    for (int i = 0; i < patchCount; ++i) {
        radio[i].reflectance.x = 0.75;
        radio[i].reflectance.y = 0.75;
        radio[i].reflectance.z = 0.75;
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
    for (int i = 6 * k * k; i < patchCount; ++i) {
        radio[i].reflectance.x = 0.75;
        radio[i].reflectance.y = 0.75;
        radio[i].reflectance.z = 0;
    }

    #ifdef OPTIMIZE_OUTPUT
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    computeRadiosity(2);

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for compute radiosity: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

	#ifdef OPTIMIZE_OUTPUT
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

    drawScene(hdc);

    #ifdef OPTIMIZE_OUTPUT
    printf("Time for draw scene: %d\n", clock() - tm);
	tm = clock();
	#endif // OPTIMIZE_OUTPUT

}


polygon * hardcodedPolygons() {
	polygon * plgs = calloc(12, sizeof(*plgs));

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
}


int computeFormFactorForScene() {
    for (int i = 0; i < polygonCount; ++i) {
        for (int j = i + 1; j < polygonCount; ++j) {
            computeFormFactorForPolygons(i, j);
            printf("%d %d\n", i, j);
        }
    }
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
}


double computeFormFactorForPatches(patch p1, patch p2, int pl1, int pl2) {
	double result = 0;
	int cnt = 0;
    for (int i = 0; i < MONTE_KARLO_ITERATIONS_COUNT; ++i) {
		double iter_res = 0;
        point on_p1 = randomPoint(p1);
        point on_p2 = randomPoint(p2);

		//Visibility function
		int flag = 0;
        for (int j = 0; j < polygonCount && !flag; ++j) {
			if (j == pl1 || j == pl2) continue;
			flag = checkIntersection(poly[j], on_p1, on_p2);
        }
        cnt += flag;
		if (flag)
			continue;

        point r = sub(on_p1, on_p2);
        iter_res = cosV(r, p2.normal);
        iter_res *= cosV(mult(r, -1), p1.normal);
        if (iter_res < 0) continue;
        double lr = length(r);
		iter_res /=  lr * lr;
		result += iter_res;
    }
    result /= MONTE_KARLO_ITERATIONS_COUNT;
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
}


int drawScene(HDC hdc) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();//load identity matrix

    glTranslatef(0.0f,-0.4f,-3.8f);//move forward 4 units

    int offset = 0;
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
	/*point cam_center = {1.7, 0, 0};
    point cam_top = {0.7, 0, 1};
    point cam_front = {0.7, 0, 0};
    point cam_right = {0.7, 1, 0};

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
    SwapBuffers(hdc);*/
}


int compar (const void* p1, const void* p2) {
    com_pare cp1 = *(com_pare *)p1;
    com_pare cp2 = *(com_pare *)p2;
    if (cp2.distance < cp1.distance) {
		return -1;
    } else if (cp2.distance > cp1.distance){
    	return 1;
    } else {
    	return rand() % 3 - 1;
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


point center(patch p) {
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

point randomPoint(patch p) {
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
	return fabs(sq / 2 - square(pl)) < DBL_EPSILON;
}


double distance(polygon pl, point p) {
    double d = -multS(pl.normal, pl.vertex[0]);
    return multS(pl.normal, p) + d;
}


int checkIntersection(polygon pl, point p1, point p2) {
    point aug = sub(p2, p1);
    double d = -multS(pl.normal, pl.vertex[0]);
    /*if (abs(d) < DBL_EPSILON) {
        return inPolygon(pl, p1);
    }*/
    double t =  -(d + multS(pl.normal, p1)) / multS(pl.normal, aug);
    if (t <= 0 || t > 1) {
        return 0;
    }
    return inPolygon(pl, sum(p1, mult(aug, t)));
}
