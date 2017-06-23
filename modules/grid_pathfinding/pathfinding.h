/* pathfinding.h */
#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "reference.h"
#include <vector>

using namespace std;

class Pathfinding : public Reference {
    OBJ_TYPE(Pathfinding,Reference);
    
protected:
    static void _bind_methods();

public:
    extern int ExploredNodes;
    extern vector<int> Landmarks;
    extern vector<vector<int>> LD;

    void Init();
    
    int BFSFindPath(const int nStartX, const int nStartY,
            const int nTargetX, const int nTargetY,
            const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
            int* pOutBuffer, const int nOutBufferSize);
    
    int BFSFindPathDiag(const int nStartX, const int nStartY,
                const int nTargetX, const int nTargetY,
                const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                int* pOutBuffer, const int nOutBufferSize);
    
    int AStarFindPath(const int nStartX, const int nStartY,
              const int nTargetX, const int nTargetY,
              const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
              int* pOutBuffer, const int nOutBufferSize);
    
    int AStarFindPathDiag(const int nStartX, const int nStartY,
                  const int nTargetX, const int nTargetY,
                  const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                  int* pOutBuffer, const int nOutBufferSize);
    
    void InitializeLandmarks(int k, const unsigned char* pMap, const int nMapWidth, const int nMapHeight);
    
    int AStarFindPathLandmarks(const int nStartX, const int nStartY,
                   const int nTargetX, const int nTargetY,
                   const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                   int* pOutBuffer, const int nOutBufferSize);
    
    void InitializeLandmarksDiag(int k, const unsigned char* pMap, const int nMapWidth, const int nMapHeight);
    
    int AStarFindPathLandmarksDiag(const int nStartX, const int nStartY,
                       const int nTargetX, const int nTargetY,
                       const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                       int* pOutBuffer, const int nOutBufferSize);
    
    int AStarFindPathNoTie(const int nStartX, const int nStartY,
                   const int nTargetX, const int nTargetY,
                   const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                   int* pOutBuffer, const int nOutBufferSize);
    
    int AStarFindPathNoTieDiag(const int nStartX, const int nStartY,
                   const int nTargetX, const int nTargetY,
                   const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
                   int* pOutBuffer, const int nOutBufferSize);

    Pathfinding();
};

#endif
