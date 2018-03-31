//
// Created by wf on 08/03/18.
//
#include <opencv2/opencv.hpp>
#include<vector>
#include "library.h"
using namespace std;
void play(char* grid, stateType* goal, stateType* s0,int numBox, vector<C_posNode>& path) {
#define SZ_PER_GRID 80
    IplImage* img = cvCreateImage(cvSize(GRID_W, GRID_H), 8, 3);
    IplImage* imgB = cvCreateImage(cvSize(GRID_W*SZ_PER_GRID, GRID_H*SZ_PER_GRID), 8, 3);
    int si =0;
    while (1) {
        for (int h = 0; h < img->height; h++) {
            uchar* pdata = (uchar*)img->imageData + h*img->widthStep;
            for (int w = 0; w < img->width; w++) {
                int pos = h*GRID_W + w;
                if (grid[pos] == 0) {
                    pdata[w * 3] = 64;
                    pdata[w * 3 + 1] = 64;
                    pdata[w * 3 + 2] = 64;
                }
                else {
                    pdata[w * 3] = 255;
                    pdata[w * 3 + 1] = 128;
                    pdata[w * 3 + 2] = 128;
                }

            }
        }
        unsigned char boxPos[MAX_NUM_BOX], playerPos;
        unsigned char boxPos2[MAX_NUM_BOX];
        decodeState(goal, boxPos, &playerPos, numBox);
        decodeState(s0, boxPos2, &playerPos, numBox);
        int rightPos = 0;

        for (int i = 0; i < numBox; i++) {
            int rp = 0;
            for (int j = 0; j < numBox; j++) {
                if (boxPos2[i] == boxPos[j]) {
                    rightPos++;
                    rp++;
                }
            }
            int posH = boxPos2[i] / GRID_W;
            int posW = boxPos2[i] % GRID_W;
            uchar* pdata = (uchar*)img->imageData+posH*img->widthStep;
            if (rp == 0) {
                pdata[posW * 3] = 0;
                pdata[posW * 3 + 1] = 192;
                pdata[posW * 3 + 2] = 192;
            }
            else {
                pdata[posW * 3] = 0;
                pdata[posW * 3 + 1] = 192;
                pdata[posW * 3 + 2] = 0;
            }
        }

        cvResize(img, imgB, 0);
        cvDrawCircle(imgB, cvPoint((playerPos% GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2), (playerPos/ GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2)), SZ_PER_GRID / 3 ,cvScalar(128,0,128), -1);
        for (int i = 0; i < numBox; i++) {
            cvDrawLine(imgB, cvPoint((boxPos[i] % GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 4), (boxPos[i] / GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2)),
                       cvPoint((boxPos[i] % GRID_W)*SZ_PER_GRID + (SZ_PER_GRID * 3 / 4), (boxPos[i] / GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2)), cvScalar(0, 0, 128), 2);
            cvDrawLine(imgB, cvPoint((boxPos[i] % GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2), (boxPos[i] / GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 4)),
                       cvPoint((boxPos[i] % GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2), (boxPos[i] / GRID_W)*SZ_PER_GRID + (SZ_PER_GRID *3 / 4)), cvScalar(0, 0, 128), 2);
            //cvDrawCircle(imgB, cvPoint((boxPos[i] % GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2), (boxPos[i] / GRID_W)*SZ_PER_GRID + (SZ_PER_GRID / 2)), SZ_PER_GRID / 6, cvScalar(0, 0, 128), 2);

        }

        cvShowImage("gridWorld", imgB);
        char a=cvWaitKey();


        int pX = playerPos%GRID_W, pY = playerPos / GRID_W;
        int offset[4] = { -1,1,-GRID_W,GRID_W };

#define POS(x,y) ((x)+(y)*GRID_W)
        int ox = 0, oy = 0;
        if (a == 'a') { ox = -1; oy = 0; }
        else if (a == 'd') { ox = 1; oy = 0; }
        else if (a == 's') { ox = 0; oy = 1; }
        else if (a == 'w') { ox = 0; oy = -1; }
        else if (a == ' ') { break; }
        else if (a == 'r') { *s0=path[0].state; continue;}
        else if (a == 'h') {
            *s0 = path[si].state;
            si=(si+1)% path.size();
            printf("\n %d  %d", path[si].swap,path[si].score);
            continue;
        }
        else { continue; }
        if (IS_INSIDE(pX +ox, pY+oy)&& grid[POS(pX + ox, pY + oy)]==1) {
            int movable = 1;
            for (int i = 0; i < numBox; i++) {
                if (boxPos2[i] == POS(pX + ox, pY + oy)) {
                    if (IS_INSIDE(pX + ox*2, pY + oy*2) && grid[POS(pX + ox * 2, pY + oy * 2)] == 1) {
                        for (int i = 0; i < numBox; i++) {
                            if (boxPos2[i] == POS(pX + ox * 2, pY + oy * 2)) {
                                movable = 0;
                            }
                        }
                        if (movable == 1) { boxPos2[i] = POS(pX + ox * 2, pY + oy * 2); }
                    }else{ movable = 0; }
                }
            }
            if (movable == 1) {
                playerPos = POS(pX + ox, pY + oy);
            }
        }
        sort(boxPos2, boxPos2 + numBox);
        *s0= encodeState(boxPos2, playerPos, numBox);
        //break;
    }

    cvReleaseImage(&img);
    cvReleaseImage(&imgB);

}


//int main() {
//    srand(10);
//    for (int i = 0; i < 200000; i++) {
//
//        char grid[GRID_WH];
//        stateType goal, s0;
//        vector<C_posNode> path;
//        char fn[260];
//        sprintf(fn, "C:\\work\\mcgill\\sokoban\\level\\%d.bin", i);
//        sokoban(grid, &goal, &s0, 4, path,fn);
//        play(grid, &goal, &s0, 4, path);
//        if (path.size() > 50) {
//
//        }else{
//            printf("pass");
//        }
//
//    }
//
//    system("pause");
//}