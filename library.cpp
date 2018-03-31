#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<map>
#include<unordered_map>
#include<vector>
#include<list>
#include<set>
#include <algorithm>
#include <assert.h>
#include "library.h"
using namespace std;

void GenRoom(char* grid) {
    int patenX[5][4] = { { -1,0,1,0 },{ 0,0,0,0 },{ -1,0,-1,0 },{ -1,0,0,0 },{ 1,0,0,0 } };
    int patenY[5][4] = { { 0,0,0,0 },{ -1,0,1,0 },{ 0,0,1,1 },{ 0,0,0,1 },{ 0,0,0,1 } };
    memset(grid, 0, GRID_WH);
    int CurPosX = rand() % GRID_W;
    int CurPosY = rand() % GRID_H;
    int direct = rand() % 4;
    int step = 0;
    while (step < RW_STEP) {
        int next_x = CurPosX + (direct == 0 ? -1 : (direct == 2 ? 1 : 0));
        int next_y = CurPosY + (direct == 1 ? -1 : (direct == 3 ? 1 : 0));
        if (IS_INSIDE(next_x, next_y)) {
            int pt = rand() % 5;
            for (int p = 0; p < 4; p++) {
                int px = CurPosX + patenX[pt][p];
                int py = CurPosY + patenY[pt][p];
                if (IS_INSIDE(px, py)) {grid[py*GRID_W + px] = 1;}
            }
            CurPosX = next_x;
            CurPosY = next_y;
            if (rand() % 100<35) { direct = rand() % 4; }
            step++;
        }
        else {	direct = rand() % 4;}
    }
}

inline int stateDist(stateType* ps1, stateType* ps2, int numBox) {
    unsigned char boxPos1[MAX_NUM_BOX], playerPos1;
    unsigned char boxPos2[MAX_NUM_BOX], playerPos2;
    decodeState(ps1, boxPos1, &playerPos1, numBox);
    decodeState(ps2, boxPos2, &playerPos2, numBox);
    int dist = 0;
    for (int i = 0; i < numBox; i++) {
        int px = boxPos1[i] % GRID_W;
        int px2 = boxPos2[i] % GRID_W;
        int py = boxPos1[i] / GRID_W;
        int py2 = boxPos2[i] / GRID_W;
        dist += abs(px - px2) + abs(py - py2);
    }
    return dist;
}

inline void TransState(stateType* ps, int numBox,int action, stateType* pNextS) {
    stateType s = *ps;
    unsigned char boxPos[MAX_NUM_BOX], playerPos;
    decodeState(ps, boxPos, &playerPos, numBox);
    int offset[4] = { -1,1,-GRID_W,GRID_W };
    if (action >= 4) {
        for(int i=0;i<numBox;i++){
            if (boxPos[i] == playerPos - offset[action - 4]) {
                boxPos[i] = playerPos;
                break;
            }
        }
    }
    playerPos += offset[action % 4];
    sort(boxPos, boxPos + numBox);
    *pNextS = encodeState(boxPos, playerPos, numBox);
}
inline void TransOrderState(stateType* ps, int numBox, int action, stateType* pNextS) {
    stateType s = *ps;
    unsigned char boxPos[MAX_NUM_BOX], playerPos;
    decodeState(ps, boxPos, &playerPos, numBox);
    int offset[4] = { -1,1,-GRID_W,GRID_W };
    if (action >= 4) {
        for (int i = 0; i<numBox; i++) {
            if (boxPos[i] == playerPos - offset[action - 4]) {
                boxPos[i] = playerPos;
                break;
            }
        }
    }
    playerPos += offset[action % 4];
    *pNextS = encodeState(boxPos, playerPos, numBox);
}
inline int boxToMove(stateType* ps, int numBox, int action) {
    stateType s = *ps;
    unsigned char playerPos = s & 0xff;
    int offset[4] = { -1,1,-GRID_W,GRID_W };
    return playerPos - offset[action - 4];
}

stateType placeTargetPlayer(char* grid,int numBox) {
    unsigned char boxPos[MAX_NUM_BOX],playerPos;
    for (int b = 0; b < numBox+1; b++) {
        int done = 0;
        while (done == 0) {
            int pos = rand() % GRID_WH;
            if (grid[pos] == 1) {
                (b < numBox ? boxPos[b] : playerPos) = pos;
                done = 1;
                for (int i = 0; i < b; i++) {
                    if (pos == boxPos[i]) { done = 0; }
                }
            }
        }
    }
    sort(boxPos, boxPos + numBox);
    return encodeState(boxPos, playerPos, numBox);
}


//void checkExpandable(char* grid, stateType* state, int numBox,int* act) {
//	unsigned char boxPos[MAX_NUM_BOX], playerPos;
//	decodeState(state, boxPos, &playerPos, numBox);
//	int pX = playerPos%GRID_W, pY = playerPos / GRID_W;
//	int offset[4] = { -1,1,-GRID_W,GRID_W };
//	int pull[4];
//	act[0] = IS_INSIDE(pX - 1, pY);
//	act[1] = IS_INSIDE(pX + 1, pY);
//	act[2] = IS_INSIDE(pX, pY - 1);
//	act[3] = IS_INSIDE(pX, pY + 1);
//	pull[0] = IS_INSIDE(pX + 1, pY);
//	pull[1] = IS_INSIDE(pX - 1, pY);
//	pull[2] = IS_INSIDE(pX, pY + 1);
//	pull[3] = IS_INSIDE(pX, pY - 1);
//	for (int d = 0; d < 4; d++) {
//		if (act[d] && grid[playerPos + offset[d]] != 1) { act[d] = 0; }
//		act[d + 4] = 0;
//		for (int i = 0; i < numBox; i++) {
//			if (act[d] && playerPos + offset[d] == boxPos[i]) { act[d] = 0; }
//		}
//		pull[d] = pull[d] && act[d];
//		for (int i = 0; i < numBox; i++) {
//			if (pull[d] && playerPos - offset[d] == boxPos[i]) { act[d + 4] = 1; }
//		}
//	}
//	for (int d = 0; d < 8; d++) { act[d] -= 2; }
//}
void checkExpandable(char* grid, stateType* state, int numBox, int* act) {
    unsigned char boxPos[MAX_NUM_BOX], playerPos;
    decodeState(state, boxPos, &playerPos, numBox);
    int pX = playerPos%GRID_W, pY = playerPos / GRID_W;
    for (int i = 0; i < numBox; i++) { grid[boxPos[i]] = 2; }
    act[0] = (pX > 0 && grid[playerPos - 1] ==1);
    act[1] = (pX < GRID_W-1 && grid[playerPos + 1] == 1);
    act[2] = (pY > 0 && grid[playerPos - GRID_W] == 1);
    act[3] = (pY < GRID_H-1 && grid[playerPos + GRID_W] == 1);
    act[4] = (act[0] && pX < GRID_W - 1 && grid[playerPos + 1] == 2);
    act[5] = (act[1] && pX > 0 && grid[playerPos - 1] == 2);
    act[6] = (act[2] && pY < GRID_H - 1 && grid[playerPos + GRID_W] == 2);
    act[7] = (act[3] && pY > 0 && grid[playerPos - GRID_W] == 2);
    for (int d = 0; d < 8; d++) { act[d] -= 2; }
    for (int i = 0; i < numBox; i++) { grid[boxPos[i]] = 1; }
}

//int reverseWalk(char* grid, stateType* goal, int numBox, stateType* s0,vector<C_posNode>& path ) {
//	map<stateType, int> stateMap;	//map for check
//	C_posNode* sNode=new C_posNode[MAX_POSITION];
//	vector<int> StateDepth[DEPTH_PRIOR];//list of state in same depth
//	stateMap[*goal] = 0;
//	sNode[0].state = *goal;
//	checkExpandable(grid, goal, numBox, sNode[0].edge);
//	StateDepth[0].push_back(0);
//	int head = 0,sNodeSz=1;
//	int depthResetCount = 0;
//	int valueIterGap = 100;
//	int foldNum = 1;
//	while (foldNum&& stateMap.size()<MAX_POSITION) {
//		if (stateMap.size() % 1000 == 0) { printf("%d ", stateMap.size()); }
//		//deep first expand tree
//		int numUnexpAct = 0;
//		int rndAct[8];
//		for (int i = 0; i < 8; i++) {
//			if (sNode[head].edge[i] == -1) {//unexpand action
//				rndAct[numUnexpAct] = i;
//				numUnexpAct++;
//			}
//		}
//		int nextFound = 0;
//		if (numUnexpAct) {
//			random_shuffle(rndAct, rndAct + numUnexpAct);
//			stateType nextState;
//			for (int i = 0; i < numUnexpAct; i++) {
//				TransState(&sNode[head].state, numBox, rndAct[i], &nextState);
//				map<stateType, int>::iterator it = stateMap.find(nextState);
//				if (it == stateMap.end()) {
//					stateMap[nextState] = sNodeSz;
//					sNode[head].edge[rndAct[i]] = sNodeSz;
//					sNode[sNodeSz].state = nextState;
//					checkExpandable(grid, &sNode[sNodeSz].state, numBox, sNode[sNodeSz].edge);
//					sNode[sNodeSz].step = sNode[head].step + 1;
//					if (sNode[head].step + 1 < DEPTH_PRIOR) {
//						StateDepth[sNode[sNodeSz].step].push_back(sNodeSz);
//						foldNum++;
//						head = sNodeSz;
//						sNodeSz++;
//						nextFound = 1;
//						break;
//					}
//					else {
//						sNodeSz++;
//					}
//				}
//				else { sNode[head].edge[rndAct[i]] = it->second; }
//			}
//		}
//		if (nextFound == 0) {
//			sNode[head].fullExpanded = 1;
//			assert(StateDepth[sNode[head].step].back() == head);
//			StateDepth[sNode[head].step].pop_back();
//			foldNum--;
//			depthResetCount = (depthResetCount + 1) % 10000;
//			//value iterator for depth reset
//			if (depthResetCount == valueIterGap - 1|| foldNum ==0) {
//				//connect all
//				for (int i = 0; i < sNodeSz; i++) {
//					for (int a = 0; a < 8; a++) {
//						if (sNode[i].edge[a] == -1) {//unexpand action
//							stateType nextState;
//							TransState(&sNode[i].state, numBox, a, &nextState);
//							map<stateType, int>::iterator it = stateMap.find(nextState);
//							if (it != stateMap.end()) {
//								sNode[i].edge[a] = it->second;
//							}
//						}
//					}
//				}
//
//				//set<int> dep[2];
//				//dep[0].insert(0);
//				//for (int s = 0; s < DEPTH_PRIOR; s++) {
//				//	set<int>& curDep = dep[s % 2];
//				//	set<int>& nextDep = dep[(s+1) % 2];
//				//	nextDep.clear();
//				//	for (std::set<int>::iterator i = curDep.begin(); i != curDep.end(); ++i) {
//				//		for (int a = 0; a < 8; a++) {
//				//			int t = sNode[*i].edge[a];
//				//			if (t >= 0 && sNode[t].step > s) {
//				//				sNode[t].step = s + 1;
//				//				nextDep.insert(t);
//				//			}
//				//		}
//				//	}
//				//	if (nextDep.size() == 0) {
//				//		break;
//				//	}
//				//}
//				int stepmax = 0;
//				for (int i = 0; i < sNodeSz; i++) {
//					if (sNode[i].step > stepmax) {
//						stepmax = sNode[i].step;
//					}
//				}
//
//				for (int s = 0; s < DEPTH_PRIOR; s++) {
//					int nextDepth = 0;
//					for (int i = 0; i < sNodeSz; i++) {
//						if (sNode[i].step == s) {
//							for (int a = 0; a < 8; a++) {
//								int t = sNode[i].edge[a];
//								if (t >= 0 && sNode[t].step > s) {
//									sNode[t].step = s + 1;
//									nextDepth++;
//								}
//							}
//						}
//					}
//					if (nextDepth == 0) {
//						printf("\n maxdep: %d  %d   ", stepmax, s);
//						break;
//					}
//				}
//
//
//				//set<int> tmpDepth[DEPTH_PRIOR+1];
//				//for (int i = 0; i < sNodeSz; i++) {
//				//	tmpDepth[sNode[i].step].insert(i);
//				//}
//				//int updateNum = 1;
//				//while (updateNum) {
//				//	updateNum = 0;
//				//	for (int s = 0; s < DEPTH_PRIOR; s++) {
//				//		for (std::set<int>::iterator it = tmpDepth[s].begin(); it != tmpDepth[s].end(); ++it) {
//				//			for (int a = 0; a < 8; a++) {
//				//				int t = sNode[(*it)].edge[a];
//				//				if(t>=0&& sNode[t].step>s+1){
//				//					updateNum++;
//				//					tmpDepth[sNode[t].step].erase(t);
//				//					tmpDepth[s+1].insert(t);
//				//					sNode[t].step = s + 1;
//				//				}
//				//			}
//				//		}
//				//	}
//				//}
//				for (int s = 0; s < DEPTH_PRIOR; s++) {
//					StateDepth[s].clear();
//				}
//				foldNum = 0;
//				for (int i = 0; i < sNodeSz; i++) {
//					if (sNode[i].fullExpanded == 0 && sNode[i].step < DEPTH_PRIOR) {
//						StateDepth[sNode[i].step].push_back(i);
//						foldNum++;
//					}
//				}
//			}
//
//			for (int i = DEPTH_PRIOR - 1; i >= 0; i--) {
//				if (StateDepth[i].size()) {
//					head = StateDepth[i].back();
//				}
//			}
//		}
//	}
//	if (1) {
//		//min box swap
//		int stepmax = 0;
//		set<int> tmpDepth[DEPTH_PRIOR + 1];
//		for (int i = 0; i < sNodeSz; i++) {
//			tmpDepth[sNode[i].step].insert(i);
//			if (stepmax < sNode[i].step) { stepmax = sNode[i].step; }
//		}
//		sNode[0].swap = 0;
//		for (int s = 0; s < DEPTH_PRIOR; s++) {
//			for (std::set<int>::iterator it = tmpDepth[s].begin(); it != tmpDepth[s].end(); ++it) {
//				for (int a = 0; a < 8; a++) {
//
//					int t = sNode[(*it)].edge[a];
//					if (t>=0&&a < 4&& sNode[(*it)].swap<sNode[t].swap) {
//						sNode[t].swap = sNode[(*it)].swap;
//						sNode[t].curBoxPos= sNode[(*it)].curBoxPos;
//					}
//					if(t >= 0 && a>=4&&sNode[(*it)].swap<sNode[t].swap){
//						int p1 = sNode[(*it)].curBoxPos;
//						unsigned char playerPos = sNode[(*it)].state & 0xff;
//						int offset[4] = { -1,1,-GRID_W,GRID_W };
//						int p2 = playerPos - offset[a - 4];
//						sNode[t].curBoxPos = playerPos;
//						sNode[t].swap = sNode[(*it)].swap+(p1!=p2);
//					}
//				}
//			}
//		}
//		//order box
//		stateType* orderBox = new stateType[sNodeSz];
//		int* prevStateIdx = new int[sNodeSz];
//		prevStateIdx[0] = -1;
//		orderBox[0] = sNode[0].state;
//		for (int s = 0; s < DEPTH_PRIOR; s++) {
//			for (std::set<int>::iterator it = tmpDepth[s].begin(); it != tmpDepth[s].end(); ++it) {
//				for (int a = 0; a < 8; a++) {
//					int t = sNode[(*it)].edge[a];
//					if (t >= 0) { TransOrderState(&orderBox[(*it)], numBox, a, &orderBox[t]); }
//					if (t >= 0 && sNode[t].step > s) {
//						prevStateIdx[t] = *it;
//					}
//				}
//			}
//		}
//		//score
//		int maxDist = -1,idx=0;
//		for (int i = 0; i < sNodeSz; i++) {
//			int v=stateDist(&orderBox[0], &orderBox[i], numBox)*sNode[i].swap;
//			//v = sNode[i].swap;
//			sNode[i].score = v;
//			if (stepmax-10 < sNode[i].step&&maxDist < v) {
//				*s0 = sNode[i].state;
//				idx = i;
//				maxDist = v;
//			}
//		}
//		//idx = stateMap[3398681074281240];
//		int curIdx = idx;
//		while (curIdx != -1) {
//			path.push_back(sNode[curIdx]);
//			curIdx = prevStateIdx[curIdx];
//		}
//		delete[] orderBox;
//		delete[] prevStateIdx;
//	}
//	delete[] sNode;
//	return 0;
//}
int reverseWalk(char* grid, stateType* goal, int numBox, stateType* s0, vector<C_posNode>& path,char* fn) {
    map <stateType, int> stateMap[2];	//map for check
    C_posNode* sNode = new C_posNode[MAX_POSITION];
    vector<int> StateDepth[DEPTH_PRIOR];//list of state in same depth
    stateMap[0][*goal] = 0;
    sNode[0].state = *goal;
    checkExpandable(grid, goal, numBox, sNode[0].edge);
    StateDepth[0].push_back(0);
    int head = 0, sNodeSz = 1;
    while (head < sNodeSz&& sNodeSz < MAX_POSITION) {
        if (sNodeSz % 100000 == 0) { printf("%d \n", stateMap[0].size()+ stateMap[1].size()); }
        //width first expand tree
        int mapid = (sNode[head].step + 1)%2;
        for (int i = 0; i < 8; i++) {
            if (sNodeSz == MAX_POSITION) { break; }
            if (sNode[head].edge[i] == -1) {//unexpand action
                stateType nextState;
                TransState(&sNode[head].state, numBox, i, &nextState);
                map<stateType, int>::iterator it = stateMap[mapid].find(nextState);
                if (it == stateMap[mapid].end()) {
                    stateMap[mapid][nextState] = sNodeSz;
                    sNode[head].edge[i] = sNodeSz;
                    sNode[sNodeSz].state = nextState;
                    checkExpandable(grid, &sNode[sNodeSz].state, numBox, sNode[sNodeSz].edge);
                    sNode[sNodeSz].step = sNode[head].step + 1;
                    sNodeSz++;
                }
                else { sNode[head].edge[i] = it->second; }
            }
        }
        head++;
    }
    //printf("done \n");
    if (1) {
        //min box swap
        int stepmax = 0;
        stateType* orderBox = new stateType[sNodeSz];
        int* prevStateIdx = new int[sNodeSz];
        prevStateIdx[0] = -1;
        orderBox[0] = sNode[0].state;
        sNode[0].swap = 0;
        for (int i = 0; i < sNodeSz; i++) {
            if (stepmax < sNode[i].step) { stepmax = sNode[i].step; }
            for (int a = 0; a < 8; a++) {
                int t = sNode[i].edge[a];
                if (t >= 0 && a < 4 && sNode[i].swap<sNode[t].swap) {
                    sNode[t].swap = sNode[i].swap;
                    sNode[t].curBoxPos = sNode[i].curBoxPos;
                }
                if (t >= 0 && a >= 4 && sNode[i].swap<sNode[t].swap) {
                    int p1 = sNode[i].curBoxPos;
                    unsigned char playerPos = sNode[i].state & 0xff;
                    int offset[4] = { -1,1,-GRID_W,GRID_W };
                    int p2 = playerPos - offset[a - 4];
                    sNode[t].curBoxPos = playerPos;
                    sNode[t].swap = sNode[i].swap + (p1 != p2);
                }
                if (t > i) {
                    TransOrderState(&orderBox[i], numBox, a, &orderBox[t]);
                    prevStateIdx[t] = i;
                }
            }
        }
        //score
        int maxDist = -1, idx = 0;
        for (int i = 0; i < sNodeSz; i++) {
            int v = stateDist(&orderBox[0], &orderBox[i], numBox)*sNode[i].swap;
            //v = sNode[i].swap;
            sNode[i].score = v;
            if (stepmax - 10 < sNode[i].step&&maxDist < v) {
                *s0 = sNode[i].state;
                idx = i;
                maxDist = v;
            }
        }
        int curIdx = idx;
        while (curIdx != -1) {
            path.push_back(sNode[curIdx]);
            curIdx = prevStateIdx[curIdx];
        }
        if (0&&path.size() > 50) {
            FILE* f = fopen(fn, "wb");
            char gw = GRID_W, gh = GRID_H;
            fwrite(&gw, 1, 1, f);
            fwrite(&gh, 1, 1, f);
            fwrite(grid, 1, GRID_W*GRID_H, f);
            fwrite(&numBox, sizeof(int), 1, f);
            fwrite(&sNodeSz, sizeof(int), 1, f);
            for (int i = 0; i < sNodeSz; i++) {
                fwrite(&(sNode[i].state), sizeof(stateType), 1, f);
                fwrite(&(prevStateIdx[i]), sizeof(int), 1, f);
            }
            int psz = path.size();
            fwrite(&psz, sizeof(int), 1, f);
            for (int i = 0; i < psz; i++) {
                fwrite(&(path[i].state), sizeof(stateType), 1, f);
            }
            fclose(f);
        }


        delete[] orderBox;
        delete[] prevStateIdx;
    }
    delete[] sNode;
    return 0;
}
int sokoban(char* grid, stateType* goal, stateType* s0,int numBox, vector<C_posNode>& path,char* fn) {
    GenRoom(grid);
    *goal=placeTargetPlayer(grid, numBox);
    reverseWalk(grid, goal, numBox,s0, path,fn);

    return 0;

}

void floatGrid2char(char* grid, stateType* s, int numBox,float* gridf,float* boxf,float* playerPosf){
    if(gridf){for(int i=0;i<GRID_WH;i++){gridf[i]=grid[i];}}
    unsigned char boxPos[MAX_NUM_BOX],playerPos;
    decodeState(s,boxPos, &playerPos, numBox);
    memset(boxf,0,GRID_WH*sizeof(float));
    memset(playerPosf,0,GRID_WH*sizeof(float));
    for(int i=0;i<numBox;i++){boxf[boxPos[i]]=1;}
    playerPosf[playerPos]=1;

}
#define POS(x, y) ((x)+(y)*GRID_W)
extern "C" {
int py_sokoban(float *gridf, float *box0, float *box1, float *playerPos0, float *playerPos1, float *nextAction,int numBox, int seed) {
    int ox = 0, oy = 0,pXY=0, pX, pY;
    vector<C_posNode> path;
    char grid[GRID_WH];
    stateType goal, s0;
    srand(seed);
    while(path.size()<10) {
        path.clear();
        GenRoom(grid);
        goal = placeTargetPlayer(grid, numBox);
        reverseWalk(grid, &goal, numBox, &s0, path, 0);
    }
    floatGrid2char(grid, &s0, numBox, gridf, box0, playerPos0);
    floatGrid2char(grid, &goal, numBox, 0, box1, playerPos1);

    for (pXY = 0; pX < GRID_WH; pXY++) {
        if (playerPos0[pXY] > 0) {
            pX = pXY % GRID_W;
            pY = pXY / GRID_W;
            break;
        }
    }
    for(int ii=0;ii<4;ii++) {
        ox = 0; oy = 0;
        if (ii == 0) { ox = -1;}
        else if (ii == 1) {ox = 1;}
        else if (ii == 2) {oy = 1;}
        else if (ii == 3) {oy = -1;}
        nextAction[ii] = ((IS_INSIDE(pX + ox, pY + oy) && gridf[POS(pX + ox, pY + oy)] == 1)&&
                          (box0[POS(pX + ox, pY + oy)] == 0||
                           (IS_INSIDE(pX + ox * 2, pY + oy * 2) &&
                            gridf[POS(pX + ox * 2, pY + oy * 2)] == 1 &&
                            box0[POS(pX + ox * 2, pY + oy * 2)] == 0))) ;
    }

}
int py_move(float *gridf, float *boxf, float *playerPosf, float *boxf2, float *playerPosf2, int action,float *nextAction) {
    //printf("   ---%d---%d--",action,GRID_WH);
    int ox = 0, oy = 0,pXY=0;
    int pX, pY;
    if (action == 0) { ox = -1;}
    else if (action == 1) {ox = 1;}
    else if (action == 2) {oy = 1;}
    else if (action == 3) {oy = -1;}
    for (pXY = 0; pXY < GRID_WH; pXY++) {
        if (playerPosf[pXY] > 0) {
            pX = pXY % GRID_W;
            pY = pXY / GRID_W;
            break;
        }
    }

    memcpy(boxf2, boxf, GRID_WH * sizeof(float));
    memcpy(playerPosf2, playerPosf, GRID_WH * sizeof(float));

    if (IS_INSIDE(pX + ox, pY + oy) && gridf[POS(pX + ox, pY + oy)] == 1) {
        if (boxf[POS(pX + ox, pY + oy)] == 0) {
            playerPosf2[POS(pX, pY)] = 0;
            playerPosf2[POS(pX + ox, pY + oy)] = 1;
            pX+=ox;
            pY+=oy;
        } else if (IS_INSIDE(pX + ox * 2, pY + oy * 2) && gridf[POS(pX + ox * 2, pY + oy * 2)] == 1 &&
                   boxf[POS(pX + ox * 2, pY + oy * 2)] == 0) {
            playerPosf2[POS(pX, pY)] = 0;
            playerPosf2[POS(pX + ox, pY + oy)] = 1;
            boxf2[POS(pX + ox, pY + oy)] = 0;
            boxf2[POS(pX + ox * 2, pY + oy * 2)] = 1;
            pX+=ox;
            pY+=oy;
        }
    }
    for(int ii=0;ii<4;ii++) {
        ox = 0; oy = 0;
        if (ii == 0) { ox = -1;}
        else if (ii == 1) {ox = 1;}
        else if (ii == 2) {oy = 1;}
        else if (ii == 3) {oy = -1;}
        nextAction[ii] = ((IS_INSIDE(pX + ox, pY + oy) && gridf[POS(pX + ox, pY + oy)] == 1)&&
                (boxf2[POS(pX + ox, pY + oy)] == 0||
                        (IS_INSIDE(pX + ox * 2, pY + oy * 2) &&
                         gridf[POS(pX + ox * 2, pY + oy * 2)] == 1 &&
                         boxf2[POS(pX + ox * 2, pY + oy * 2)] == 0))) ;
    }
    return 0;
}

float testroom[4*GRID_WH];
int testtag=0;
int testplayerpos=0;
//int py_newgame_batch(float* outbuff,float *validA,float* rightBox,float* updateTag,int batchSize,int numBox,int* seed){
//    for(int b=0;b<batchSize;b++){
//        if(updateTag[b]==0){continue;}
//        float* cur_outbuff=outbuff+b*GRID_WH*4;
//        memset(cur_outbuff,0,4*GRID_WH*sizeof(float));
//        int ox = 0, oy = 0,pXY=0, pX, pY;
//        vector<C_posNode> path;
//        char grid[GRID_WH];
//        stateType goal, s0;
//        srand(seed[b]);
//        if(testtag==0) {
//            while (path.size() < 10) {
//                path.clear();
//                GenRoom(grid);
//                goal = placeTargetPlayer(grid, numBox);
//                reverseWalk(grid, &goal, numBox, &s0, path, 0);
//            }
//            for (int i = 0; i < GRID_WH; i++) { cur_outbuff[i * 4] = grid[i]; }
//            unsigned char boxPos[MAX_NUM_BOX], playerPos,playerPos2;
//            decodeState(&s0, boxPos, &playerPos, numBox);
//            for (int i = 0; i < numBox; i++) { cur_outbuff[boxPos[i] * 4 + 1] = 1; }
//            cur_outbuff[playerPos * 4 + 3] = 1;
//            decodeState(&goal, boxPos, &playerPos2, numBox);
//            for (int i = 0; i < numBox; i++) { cur_outbuff[boxPos[i] * 4 + 2] = 1; }
//
//            pX = playerPos % GRID_W;
//            pY = playerPos / GRID_W;
//            memcpy(testroom, outbuff, 4*GRID_WH * sizeof(float));
//            testplayerpos=playerPos;
//            testtag=1;
//        }else{
//            memcpy(cur_outbuff, testroom, 4*GRID_WH * sizeof(float));
//            pX = testplayerpos % GRID_W;
//            pY = testplayerpos / GRID_W;
//        }
//
//        for(int ii=0;ii<4;ii++) {
//            ox = 0; oy = 0;
//            if (ii == 0) { ox = -1;}
//            else if (ii == 1) {ox = 1;}
//            else if (ii == 2) {oy = 1;}
//            else if (ii == 3) {oy = -1;}
//            validA[b*4+ii] = ((IS_INSIDE(pX + ox, pY + oy) && cur_outbuff[POS(pX + ox, pY + oy)*4] == 1)&&
//                              (cur_outbuff[POS(pX + ox, pY + oy)*4+1] == 0||(IS_INSIDE(pX + ox * 2, pY + oy * 2) &&
//                                                                             cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4] == 1 &&
//                                                                             cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4+1] == 0))) ;
//        }
//
//        rightBox[b] = 0;
//        for (pXY = 0; pXY < GRID_WH; pXY++) {
//            if (cur_outbuff[pXY*4+1] == 1&& cur_outbuff[pXY*4+2] == 1) {
//                rightBox[b]++;
//            }
//        }
//
//    }
//}
int py_newgame_batch(float* outbuff,float *validA,float* rightBox,float* updateTag,int batchSize,int numBox,int* seed){
    for(int b=0;b<batchSize;b++){
        if(updateTag[b]==0){continue;}
        float* cur_outbuff=outbuff+b*GRID_WH*4;
        memset(cur_outbuff,0,4*GRID_WH*sizeof(float));
        int ox = 0, oy = 0,pXY=0, pX, pY;
        vector<C_posNode> path;
        char grid[GRID_WH];
        stateType goal, s0;

        while (path.size() < 10) {
            path.clear();
            GenRoom(grid);
            goal = placeTargetPlayer(grid, numBox);
            reverseWalk(grid, &goal, numBox, &s0, path, 0);
        }
        for (int i = 0; i < GRID_WH; i++) { cur_outbuff[i * 4] = grid[i]; }
        unsigned char boxPos[MAX_NUM_BOX], playerPos,playerPos2;
        decodeState(&s0, boxPos, &playerPos, numBox);
        for (int i = 0; i < numBox; i++) { cur_outbuff[boxPos[i] * 4 + 1] = 1; }
        cur_outbuff[playerPos * 4 + 3] = 1;
        decodeState(&goal, boxPos, &playerPos2, numBox);
        for (int i = 0; i < numBox; i++) { cur_outbuff[boxPos[i] * 4 + 2] = 1; }

        pX = playerPos % GRID_W;
        pY = playerPos / GRID_W;
        memcpy(testroom, outbuff, 4*GRID_WH * sizeof(float));
        testplayerpos=playerPos;
        testtag=1;

        for(int ii=0;ii<4;ii++) {
            ox = 0; oy = 0;
            if (ii == 0) { ox = -1;}
            else if (ii == 1) {ox = 1;}
            else if (ii == 2) {oy = 1;}
            else if (ii == 3) {oy = -1;}
            validA[b*4+ii] = ((IS_INSIDE(pX + ox, pY + oy) && cur_outbuff[POS(pX + ox, pY + oy)*4] == 1)&&
                              (cur_outbuff[POS(pX + ox, pY + oy)*4+1] == 0||(IS_INSIDE(pX + ox * 2, pY + oy * 2) &&
                                                                             cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4] == 1 &&
                                                                             cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4+1] == 0))) ;
        }
        if(validA[b*4]+validA[b*4+1]+validA[b*4+2]+validA[b*4+3]==0){printf("err in cccc");}

        rightBox[b] = 0;
        for (pXY = 0; pXY < GRID_WH; pXY++) {
            if (cur_outbuff[pXY*4+1] == 1&& cur_outbuff[pXY*4+2] == 1) {
                rightBox[b]++;
            }
        }
        if(rightBox[b]==4){printf("err in c");}

    }
}

int py_move_batch(float* inbuff,float* outbuff,float *validA,int* a,float* rightBox,int batchSize){
    memcpy(outbuff, inbuff, 4*batchSize*GRID_WH * sizeof(float));

    for(int b=0;b<batchSize;b++){
        int action=a[b];
        float* cur_inbuff=inbuff+b*GRID_WH*4;
        float* cur_outbuff=outbuff+b*GRID_WH*4;
        int ox = 0, oy = 0,pXY=0;
        int pX, pY;
        if (action == 0) { ox = -1;}
        else if (action == 1) {ox = 1;}
        else if (action == 2) {oy = 1;}
        else if (action == 3) {oy = -1;}
        //0 grid 1 box 2 boxgoal 3 playerpos
        for (pXY = 0; pXY < GRID_WH; pXY++) {
            if (cur_inbuff[pXY*4+3] > 0) {
                pX = pXY % GRID_W;
                pY = pXY / GRID_W;
                break;
            }
        }
        if (IS_INSIDE(pX + ox, pY + oy) && cur_inbuff[POS(pX + ox, pY + oy)*4] == 1) {
            if (cur_inbuff[POS(pX + ox, pY + oy)*4+1] == 0) {
                cur_outbuff[POS(pX, pY)*4+3] = 0;
                cur_outbuff[POS(pX + ox, pY + oy)*4+3] = 1;
                pX+=ox;
                pY+=oy;
            } else if (IS_INSIDE(pX + ox * 2, pY + oy * 2) && cur_inbuff[POS(pX + ox * 2, pY + oy * 2)*4] == 1 &&
                       cur_inbuff[POS(pX + ox * 2, pY + oy * 2)*4+1] == 0) {
                cur_outbuff[POS(pX, pY)*4+3] = 0;
                cur_outbuff[POS(pX + ox, pY + oy)*4+3] = 1;
                cur_outbuff[POS(pX + ox, pY + oy)*4+1] = 0;
                cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4+1] = 1;
                pX+=ox;
                pY+=oy;
            }
        }
        for(int ii=0;ii<4;ii++) {
            ox = 0; oy = 0;
            if (ii == 0) { ox = -1;}
            else if (ii == 1) {ox = 1;}
            else if (ii == 2) {oy = 1;}
            else if (ii == 3) {oy = -1;}
            validA[b*4+ii] = ((IS_INSIDE(pX + ox, pY + oy) && cur_outbuff[POS(pX + ox, pY + oy)*4] == 1)&&
                    (cur_outbuff[POS(pX + ox, pY + oy)*4+1] == 0||(IS_INSIDE(pX + ox * 2, pY + oy * 2) &&
                            cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4] == 1 &&
                            cur_outbuff[POS(pX + ox * 2, pY + oy * 2)*4+1] == 0))) ;
        }
        rightBox[b]=0;
        for (pXY = 0; pXY < GRID_WH; pXY++) {
            if (cur_outbuff[pXY*4+1] == 1&& cur_outbuff[pXY*4+2] == 1) {
                rightBox[b]++;
            }
        }

    }
}




}

#include<pthread.h>
void* genLevel(void* _id){
    int id=*(int*)_id;
    srand(id);
    for(int i1=0;i1<1000;i1++){
        char fn[260];
        sprintf(fn, "/home/wf/sokobanlv/%d_%d.bin", id,i1);
        FILE* f=fopen(fn,"w");
        for(int i2=0;i2<1000;i2++){
            char grid[GRID_WH];
            stateType goal, s0;
            vector<C_posNode> path;
            while (path.size() < 10) {
                path.clear();
                GenRoom(grid);
                goal = placeTargetPlayer(grid, 4);
                reverseWalk(grid, &goal, 4, &s0, path, 0);
            }
            printf("%d_%d\n",id,i1*1000+i2);
            fwrite(grid,1,GRID_WH,f);
            fwrite(&s0,sizeof(stateType),1,f);
            fwrite(&goal,sizeof(stateType),1,f);
        }
        fclose(f);
    }
    return 0;
}


int main() {
    pthread_t a,b,c,d;
    int v0=0,v1=1,v2=2,v3=3;
    pthread_create(&a, NULL, genLevel, &v0);
    pthread_create(&b, NULL, genLevel, &v1);
    pthread_create(&c, NULL, genLevel, &v2);
    pthread_create(&d, NULL, genLevel, &v3);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);
    pthread_join(d, NULL);
}
//int main() {
//	for (int i = 0; i < 10000; i++) {
//		srand(i);
//		int numBox, sNodeSz;
//		C_posNode* sNode = new C_posNode[MAX_POSITION];
//		char grid[GRID_WH];
//		stateType goal, s0;
//		vector<C_posNode> path;
//		char fn[260];
//		sprintf(fn, "C:\\work\\mcgill\\sokoban\\level\\27.bin", i);
//		FILE* f = fopen(fn, "rb");
//		char gw = GRID_W, gh = GRID_H;
//		fread(&gw, 1, 1, f);
//		fread(&gh, 1, 1, f);
//		fread(grid, 1, GRID_W*GRID_H, f);
//		fread(&numBox, sizeof(int), 1, f);
//		fread(&sNodeSz, sizeof(int), 1, f);
//		for (int i = 0; i < sNodeSz; i++) {
//			fread(&(sNode[i].state), sizeof(stateType), 1, f);
//		}
//		int psz;
//		fread(&psz, sizeof(int), 1, f);
//		path.resize(psz);
//		for (int i = 0; i < psz; i++) {
//			fread(&(path[i].state), sizeof(stateType), 1, f);
//		}
//		fclose(f);
//		goal = path[psz-1].state;
//		s0= path[0].state;
//		play(grid, &goal, &s0, 6, path);
//	}
//	system("pause");
//}