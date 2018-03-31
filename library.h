#ifndef SOKOBAN_LIBRARY_H
#define SOKOBAN_LIBRARY_H

#define GRID_W 8
#define GRID_H 8
#define GRID_WH (GRID_W*GRID_H)
#define RW_STEP (((GRID_W+2)+(GRID_H+2))*0.5)
#define IS_INSIDE(x,y) ((x)>=0&&(x)<GRID_W&&(y)>=0&&(y)<GRID_H)
#define MAX_NUM_BOX 7
#define MAX_POSITION 10000000
#define DEPTH_PRIOR 300
#include<vector>
typedef unsigned long long stateType;
struct C_posNode {
    stateType state;
    int edge[8];//-1 not expand,-2 not allowed
    int fullExpanded;
    int step;
    int swap;
    int score;
    int curBoxPos;
    C_posNode() :fullExpanded(0), step(0), swap(1000), score(0), curBoxPos(-1){ for (int i = 0; i < 8; i++) { edge[i] = -1; } }
};
inline stateType encodeState(unsigned char* boxPos, unsigned char playerPos, int numBox) {

    stateType ret = 0;
    for (int i = 0; i < numBox; i++) { ret = (ret + boxPos[i]) << 8; }
    return ret + playerPos;
}
inline void decodeState(stateType* ps,unsigned char* boxPos, unsigned char* playerPos, int numBox) {
    stateType s = *ps;
    *playerPos = s & 0xff;
    for (int i = numBox-1; i >=0; i--) {
        s = s >> 8;
        boxPos[i]= s & 0xff;
    }
}
int sokoban(char* grid, stateType* goal, stateType* s0,int numBox, std::vector<C_posNode>& path,char* fn);
#endif