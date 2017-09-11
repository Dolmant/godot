/* pathfinding.h */
#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "array.h"
#include "ustring.h"
#include "reference.h"
#include <vector>

using namespace std;

extern int ExploredNodes;
extern vector<int> Landmarks;
extern vector<vector<int>> LD;


class Pathfinding : public Reference {
    OBJ_TYPE(Pathfinding,Reference);
    int nMapHeight;
    int nMapWidth;
    unsigned char* pMap;
    int nOutBufferSize;
    Array pOutBuffer;

protected:
    static void _bind_methods();

public:
    void Init(String pMap_init);

    Array Get_Buffer();

    int BFSFindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);
        // , int* pOutBuffer);

    int BFSFindPathDiag(const int nStartX, const int nStartY,
                const int nTargetX, const int nTargetY);
                // int* pOutBuffer);

    Array AStarFindPath(const Vector2 nStart, const Vector2 nTarget);

    int AStarFindPathDiag(const int nStartX, const int nStartY,
                  const int nTargetX, const int nTargetY);
                //   int* pOutBuffer);

    void InitializeLandmarks(size_t k);
    
    int AStarFindPathLandmarks(const int nStartX, const int nStartY,
                   const int nTargetX, const int nTargetY);
                //    int* pOutBuffer);

    void InitializeLandmarksDiag(size_t k);
    
    int AStarFindPathLandmarksDiag(const int nStartX, const int nStartY,
                       const int nTargetX, const int nTargetY);
                    //    int* pOutBuffer);

    int AStarFindPathNoTie(const int nStartX, const int nStartY,
                   const int nTargetX, const int nTargetY);
                //    int* pOutBuffer);

    int AStarFindPathNoTieDiag(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY);
    // int* pOutBuffer);

    Pathfinding();
};

#endif
