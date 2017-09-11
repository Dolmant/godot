/* pathfinding.cpp */
#include <vector>
#include <queue>
#include <tuple>
#include <cstdlib>
#include <climits>
#include <functional>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <fstream>
#include "dictionary.h"
#include "math_2d.h"
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "ustring.h"
#include "pathfinding.h"

int ExploredNodes;
vector<int> Landmarks;
vector<vector<int>> LD;

void Pathfinding::Init(String pMap_init) {
    // nMapHeight = nMapHeight_init;
    // nMapWidth = nMapWidth_init;
    // pMap = pMap_init;
    // nOutBufferSize = nOutBufferSize_init;
    vector<int> traversable;

    std::ifstream map (pMap_init.c_str());
    if (map.is_open()) {

        string temp;
        getline(map, temp);
        map >> temp; map >> nMapHeight;
        map >> temp; map >> nMapWidth;
        getline(map, temp);
        getline(map, temp);

        vector<unsigned char> passable{'.', 'G', 'S'};
        // TODO make this all vector/pos based
        // todo create a separate pathfinder for multiple landing spots
        // todo create different types of terrain which can affect the pathfinder in different ways
        pMap = new unsigned char[nMapHeight*nMapWidth];
        for (int i = 0; i < nMapWidth; i++) {
            for (int j = 0; j < nMapHeight; j++) {
                char c; map >> c;
                pMap[nMapWidth*j + i] = (count(passable.begin(), passable.end(), c) > 0);
                if (count(passable.begin(), passable.end(), c) > 0)
                    traversable.push_back(nMapWidth*j + i);
            }
        }
        map.close();

        boost::random::random_device rng;
        boost::random::uniform_int_distribution<> uniform(0, traversable.size() - 1);
        const int TIMES = 5;
        vector<Vector2> tests;

        for (int t = 0; t < TIMES; t++) {
            int u = traversable[uniform(rng)];
            int v = traversable[uniform(rng)];
            tests.push_back(Vector2(u % nMapWidth, u / nMapWidth));
            tests.push_back(Vector2(v % nMapWidth, v / nMapWidth));
        }

        
        //                                     //
        // BENCHMARKING CODE //
        //                                     //
        
        // for (auto PathFinder : {
        // // make_pair(&Pathfinding::BFSFindPath, "BFS"),
        // make_pair(&Pathfinding::AStarFindPath, "AStar")
        // // make_pair(&Pathfinding::AStarFindPathLandmarks, "AStarLandmarks"),
        // // make_pair(&Pathfinding::AStarFindPathNoTie, "AStarNoTie"),
        // // make_pair(&Pathfinding::BFSFindPathDiag, "BFSDiag"),
        // // make_pair(&Pathfinding::AStarFindPathDiag, "AStarDiag"),
        // // make_pair(&Pathfinding::AStarFindPathLandmarksDiag, "AStarLandmarksDiag"),
        // // make_pair(&Pathfinding::AStarFindPathNoTieDiag, "AStarNoTieDiag")
        // }) {
        //     if (!strncmp(PathFinder.second, "AStarLandmarks", 14)) {
        //         Landmarks.clear();
        //         LD.clear();
        //         InitializeLandmarks(8);
        //     }
    
        //     if (!strncmp(PathFinder.second, "AStarLandmarksDiag", 18)) {
        //         Landmarks.clear();
        //         LD.clear();
        //         InitializeLandmarksDiag(8);
        //     }
    
        //     // printf("%12s -> %12s :: %s (%s)\n", "from", "to", "distance", "efficiency");
        //     long long tot = 0;
        //     const clock_t begin_time = clock();

        //     for (int t = 0; t < TIMES; t++) {
        //         Vector2 u = tests[2*t], v = tests[2*t + 1];
        //         (this->*PathFinder.first)(u, v);
        //         // printf("(%4d, %4d) -> (%4d, %4d) :: %8d (%.2f)\n", u.first, u.second, v.first, v.second,
        //         // 	         dist, (dist == -1) ? 0.0 : 100.0 * dist/ExploredNodes);
        //         tot += ExploredNodes;
        //     }
        //     double ticks = clock() - begin_time;
        //     printf("%20s -- avg time: %-3.3fms\tavg nodes: %8lld\n", PathFinder.second, double(ticks/CLOCKS_PER_SEC)*1000/TIMES, tot/TIMES);
        // }
        //                                             //
        // END BENCHMARKING CODE //
        //                                             //
        
        
        // delete []pMap;
        // delete []pOutBuffer;
    }
};

Array Pathfinding::GetBuffer() {
    return pOutBuffer;
}

Array Pathfinding::GetMultiple() {
    return pOutBuffer;
}

int Pathfinding::BFSFindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    ExploredNodes = 0;
    vector<int> p(n), d(n, INT_MAX);
    d[startPos] = 0;
    queue<int> q;
    q.push(startPos);
    while (!q.empty()) {
        int u = q.front(); q.pop(); ExploredNodes++;
        for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
	            continue;
            if (0 <= v && v < n && d[v] == INT_MAX && pMap[v]) {
                p[v] = u;
                d[v] = d[u] + 1;
                if (v == targetPos)
                    goto end;
                q.push(v);
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = Vector2(curr % nMapWidth, curr / nMapWidth);
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos];
}

int Pathfinding::BFSFindPathDiag(const int nStartX, const int nStartY,
		        const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    ExploredNodes = 0;
    std::vector<int> p(n), d(n, INT_MAX);
    std::queue<int> q;
    d[startPos] = 0;
    q.push(startPos);
    while (!q.empty()) {
        int u = q.front(); q.pop(); ExploredNodes++;
        for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	    +1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
	    || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
	continue;
            if (0 <= v && v < n && d[v] == INT_MAX && pMap[v]) {
	p[v] = u;
	d[v] = d[u] + 1;
	if (v == targetPos)
	    goto end;
	q.push(v);
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}

Array Pathfinding::AStarFindPath(const Vector2 nStart, const Vector2 nTarget) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    auto h = [=](int u) -> int { // lower bound distance to target from u
        int x = u % nMapWidth, y = u / nMapWidth;
        return abs(x-int(nTarget.x)) + abs(y-int(nTarget.y));
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(int(nStart.x), int(nStart.y)), targetPos = idx(int(nTarget.x), int(nTarget.y));

    int discovered = 0; ExploredNodes = 0;
    std::vector<int> p(n), d(n, INT_MAX);
    std::priority_queue<tuple<int, int, int>,
        std::vector<tuple<int, int, int>>,
        std::greater<tuple<int, int, int>>> pq; // A* with tie breaking
    d[startPos] = 0;
    pq.push(make_tuple(0 + h(startPos), 0, startPos));
    while (!pq.empty()) {
        int previous_node = std::get<2>(pq.top()); pq.pop(); ExploredNodes++;
        for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
            int test_node = previous_node + e;
            if ((e == 1 && (test_node % nMapWidth == 0)) || (e == -1 && (previous_node % nMapWidth == 0))) {
                // test all 4 boundaries? 
                // -a no because this is ensuring we check around the x boundary and dont wrap
                // remove this if you want wrapping
                // the check below ensures we dont fall off the y axis
                continue;
            }
            if (0 <= test_node && test_node < n && d[test_node] > d[previous_node] + 1 && pMap[test_node]) {
                p[test_node] = previous_node;
                d[test_node] = d[previous_node] + 1;
                if (test_node == targetPos)
                    goto end;
                pq.push(make_tuple(d[test_node] + h(test_node), ++discovered, test_node));
            }
        }
    }
end:
 
    if (d[targetPos] == INT_MAX) {
        printf("\n");
        printf("Path couldnt be found");
        return -1;
    }
    int curr = targetPos;
    pOutBuffer.clear();
    for (int i = d[targetPos] - 1; i >= 0; i--) {
        pOutBuffer.push_back(Vector2(curr % nMapWidth, curr / nMapWidth));
        curr = p[curr];
    }
    return pOutBuffer;
}

int Pathfinding::AStarFindPathDiag(const int nStartX, const int nStartY,
		            const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    auto h = [=](int u) -> int { // lower bound distance to target from u
        int x = u % nMapWidth, y = u / nMapWidth;
        return max(abs(x-nTargetX), abs(y-nTargetY));
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    int discovered = 0; ExploredNodes = 0;
    std::vector<int> p(n), d(n, INT_MAX);
    std::priority_queue<tuple<int, int, int>,
        std::vector<tuple<int, int, int>>,
		std::greater<tuple<int, int, int>>> pq; // A* with tie breaking
    d[startPos] = 0;
    pq.push(make_tuple(0 + h(startPos), 0, startPos));
    while (!pq.empty()) {
        int u = std::get<2>(pq.top()); pq.pop(); ExploredNodes++;
        for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	    +1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
                    || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
                continue;
            if (0 <= v && v < n && d[v] > d[u] + 1 && pMap[v]) {
                p[v] = u;
                d[v] = d[u] + 1;
                if (v == targetPos)
                    goto end;
                pq.push(make_tuple(d[v] + h(v), ++discovered, v));
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}

void Pathfinding::InitializeLandmarks(size_t k) {

    std::vector<int> traversable;
    for (int i = 0; i < nMapWidth; i++)
        for (int j = 0; j < nMapHeight; j++)
            if (pMap[nMapWidth*j + i])
	traversable.push_back(nMapWidth*j + i);

    while (Landmarks.size() < k) {

        if (Landmarks.empty()) {
            boost::random::random_device rng;
            boost::random::uniform_int_distribution<> uniform(0, traversable.size() - 1);
            Landmarks.push_back(traversable[uniform(rng)]);
            continue;
        }

        const int n = nMapWidth*nMapHeight;
        vector<int> p(n), d(n, INT_MAX);
        queue<int> q;
        for (auto s : Landmarks) {
            d[s] = 0;
            q.push(s);
        }
        int farthest = -1, maxdist = -1;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            if (d[u] > maxdist)
	maxdist = d[u], farthest = u;
            for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
	int v = u + e;
	if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
	    continue;
	if (0 <= v && v < n && d[v] == INT_MAX && pMap[v]) {
	    p[v] = u;
	    d[v] = d[u] + 1;
	    q.push(v);
	}
            }
        }

        Landmarks.push_back(farthest); // works well when the graph is not too disconnected
    }

    LD.resize(Landmarks.size());
    for (size_t i = 0; i < Landmarks.size(); i++) {
        const int n = nMapWidth*nMapHeight;
        std::vector<int> p(n); LD[i].resize(n, INT_MAX);
        std::queue<int> q;
        int s = Landmarks[i];
        LD[i][s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
	int v = u + e;
	if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
	    continue;
	if (0 <= v && v < n && LD[i][v] == INT_MAX && pMap[v]) {
	    p[v] = u;
	    LD[i][v] = LD[i][u] + 1;
	    q.push(v);
	}
            }
        }
    }
}

int Pathfinding::AStarFindPathLandmarks(const int nStartX, const int nStartY,
			     const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    auto h = [=](int u) { // lower bound distance to target from u
        int m = 0;
        for (size_t i = 0; i < Landmarks.size(); i++) // global vector<int> Landmarks
            m = max(m, LD[i][targetPos] - LD[i][u]); // global vector<vector<int>> LD
        return m;
    };

    int discovered = 0; ExploredNodes = 0;
    std::vector<int> p(n), d(n, INT_MAX);
    std::priority_queue<tuple<int, int, int>,
         std::vector<tuple<int, int, int>>,
		 std::greater<tuple<int, int, int>>> pq; // A* with tie breaking
    d[startPos] = 0;
    pq.push(make_tuple(0 + h(startPos), 0, startPos));
    while (!pq.empty()) {
        int u = std::get<2>(pq.top()); pq.pop(); ExploredNodes++;
        for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
	continue;
            if (0 <= v && v < n && d[v] > d[u] + 1 && pMap[v]) {
	p[v] = u;
	d[v] = d[u] + 1;
	if (v == targetPos)
	    goto end;
	pq.push(make_tuple(d[v] + h(v), ++discovered, v));
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}

void Pathfinding::InitializeLandmarksDiag(size_t k) {

    vector<int> traversable;
    for (int i = 0; i < nMapWidth; i++)
        for (int j = 0; j < nMapHeight; j++)
            if (pMap[nMapWidth*j + i])
	traversable.push_back(nMapWidth*j + i);

    while (Landmarks.size() < k) {

        if (Landmarks.empty()) {
            boost::random::random_device rng;
            boost::random::uniform_int_distribution<> uniform(0, traversable.size() - 1);
            Landmarks.push_back(traversable[uniform(rng)]);
            continue;
        }

        const int n = nMapWidth*nMapHeight;
        std::vector<int> p(n), d(n, INT_MAX);
        std::queue<int> q;
        for (auto s : Landmarks) {
            d[s] = 0;
            q.push(s);
        }
        int farthest = -1, maxdist = -1;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            if (d[u] > maxdist)
	maxdist = d[u], farthest = u;
            for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	        +1, -1, +nMapWidth, -nMapWidth}) {
	int v = u + e;
	if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
	        || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
	    continue;
	if (0 <= v && v < n && d[v] == INT_MAX && pMap[v]) {
	    p[v] = u;
	    d[v] = d[u] + 1;
	    q.push(v);
	}
            }
        }

        Landmarks.push_back(farthest); // works well when the graph is not too disconnected
    }

    LD.resize(Landmarks.size());
    for (size_t i = 0; i < Landmarks.size(); i++) {
        const int n = nMapWidth*nMapHeight;
        std::vector<int> p(n); LD[i].resize(n, INT_MAX);
        std::queue<int> q;
        int s = Landmarks[i];
        LD[i][s] = 0;
        q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	        +1, -1, +nMapWidth, -nMapWidth}) {
	int v = u + e;
	if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
	        || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
	    continue;
	if (0 <= v && v < n && LD[i][v] == INT_MAX && pMap[v]) {
	    p[v] = u;
	    LD[i][v] = LD[i][u] + 1;
	    q.push(v);
	}
            }
        }
    }
}

int Pathfinding::AStarFindPathLandmarksDiag(const int nStartX, const int nStartY,
			             const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    auto h = [=](int u) { // lower bound distance to target from u
        int m = 0;
        for (size_t i = 0; i < Landmarks.size(); i++) // global vector<int> Landmarks
            m = max(m, LD[i][targetPos] - LD[i][u]); // global vector<vector<int>> LD
        return m;
    };

    int discovered = 0; ExploredNodes = 0;
    std::vector<int> p(n), d(n, INT_MAX);
    std::priority_queue<tuple<int, int, int>,
		 vector<tuple<int, int, int>>,
		 greater<tuple<int, int, int>>> pq; // A* with tie breaking
    d[startPos] = 0;
    pq.push(make_tuple(0 + h(startPos), 0, startPos));
    while (!pq.empty()) {
        int u = std::get<2>(pq.top()); pq.pop(); ExploredNodes++;
        for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	    +1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
	    || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
	continue;
            if (0 <= v && v < n && d[v] > d[u] + 1 && pMap[v]) {
	p[v] = u;
	d[v] = d[u] + 1;
	if (v == targetPos)
	    goto end;
	pq.push(make_tuple(d[v] + h(v), ++discovered, v));
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}

int Pathfinding::AStarFindPathNoTie(
    const int nStartX, const int nStartY,
    const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    auto h = [=](int u) { // lower bound distance to target from u
        int x = u % nMapWidth, y = u / nMapWidth;
        return abs(x-nTargetX) + abs(y-nTargetY);
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    ExploredNodes = 0;
    vector<int> p(n), d(n, INT_MAX);
    std::priority_queue<pair<int, int>,
         std::vector<pair<int, int>>,
         std::greater<pair<int, int>>> pq;
    d[startPos] = 0;
    pq.push(make_pair(0 + h(startPos), startPos));
    while (!pq.empty()) {
        int u = pq.top().second; pq.pop(); ExploredNodes++;
        for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
	continue;
            if (0 <= v && v < n && d[v] > d[u] + 1 && pMap[v]) {
	p[v] = u;
	d[v] = d[u] + 1;
	if (v == targetPos)
	    goto end;
	pq.push(make_pair(d[v] + h(v), v));
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}

int Pathfinding::AStarFindPathNoTieDiag(const int nStartX, const int nStartY,
			     const int nTargetX, const int nTargetY) {

    auto idx = [this](int x, int y) {
        return x + y*nMapWidth;
    };

    auto h = [=](int u) { // lower bound distance to target from u
        int x = u % nMapWidth, y = u / nMapWidth;
        return max(abs(x-nTargetX), abs(y-nTargetY));
    };

    const int n = nMapWidth*nMapHeight;
    const int startPos = idx(nStartX, nStartY), targetPos = idx(nTargetX, nTargetY);

    ExploredNodes = 0;
    vector<int> p(n), d(n, INT_MAX);
    priority_queue<pair<int, int>,
		 vector<pair<int, int>>,
		 greater<pair<int, int>>> pq;
    d[startPos] = 0;
    pq.push(make_pair(0 + h(startPos), startPos));
    while (!pq.empty()) {
        int u = pq.top().second; pq.pop(); ExploredNodes++;
        for (auto e : {-nMapWidth-1, -nMapWidth+1, +nMapWidth-1, +nMapWidth+1,
	    +1, -1, +nMapWidth, -nMapWidth}) {
            int v = u + e;
            if (((e == 1 || e == -nMapWidth+1 || e == nMapWidth+1) && (v % nMapWidth == 0))
	    || ((e == -1 || e == -nMapWidth-1 || e == nMapWidth-1) && (u % nMapWidth == 0)))
	continue;
            if (0 <= v && v < n && d[v] > d[u] + 1 && pMap[v]) {
	p[v] = u;
	d[v] = d[u] + 1;
	if (v == targetPos)
	    goto end;
	pq.push(make_pair(d[v] + h(v), v));
            }
        }
    }
 end:

    if (d[targetPos] == INT_MAX) {
        return -1;
    } else if (d[targetPos] <= nOutBufferSize) {
        int curr = targetPos;
        for (int i = d[targetPos] - 1; i >= 0; i--) {
            pOutBuffer[i] = curr;
            curr = p[curr];
        }
        return d[targetPos];
    }

    return d[targetPos]; // buffer size too small
}


void Pathfinding::_bind_methods() {
        ObjectTypeDB::bind_method("Init",&Pathfinding::Init);
        ObjectTypeDB::bind_method("Get_Buffer",&Pathfinding::Get_Buffer);
        ObjectTypeDB::bind_method("BFSFindPath",&Pathfinding::BFSFindPath);
        ObjectTypeDB::bind_method("BFSFindPathDiag",&Pathfinding::BFSFindPathDiag);
        ObjectTypeDB::bind_method("AStarFindPath",&Pathfinding::AStarFindPath);
        ObjectTypeDB::bind_method("AStarFindPathDiag",&Pathfinding::AStarFindPathDiag);
        ObjectTypeDB::bind_method("InitializeLandmarks",&Pathfinding::InitializeLandmarks);
        ObjectTypeDB::bind_method("AStarFindPathLandmarks",&Pathfinding::AStarFindPathLandmarks);
        ObjectTypeDB::bind_method("InitializeLandmarksDiag",&Pathfinding::InitializeLandmarksDiag);
        ObjectTypeDB::bind_method("AStarFindPathLandmarksDiag",&Pathfinding::AStarFindPathLandmarksDiag);
        ObjectTypeDB::bind_method("AStarFindPathNoTie",&Pathfinding::AStarFindPathNoTie);
        ObjectTypeDB::bind_method("AStarFindPathNoTieDiag",&Pathfinding::AStarFindPathNoTieDiag);
}

Pathfinding::Pathfinding() {
}
