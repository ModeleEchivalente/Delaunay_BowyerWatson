#include <iostream>
#include <vector>
#include <algorithm>
#define double long double

using namespace std;

double eps = 1e-7;


struct Point {
     double x, y;
     Point(const double& x, const double& y) : x(x), y(y) {}

     bool operator==(const Point& other) const {
          return (abs(x - other.x) <= eps && abs(y - other.y) <= eps);
     }

     bool operator!=(const Point& other) const {
          return !operator==(other);
     }
};

// Cartesian distance between 2 points
double dist(Point a, Point b) {
     return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

struct Edge {
     Point p1, p2;
     Edge(const Point& p1, const Point& p2) : p1(p1), p2(p2) {}

     bool operator==(const Edge& other) const {
          return (p1 == other.p1 && p2 == other.p2 || p2 == other.p1 && p1 == other.p2);
     }
};


struct Circle {
     double x, y, r; // (x, y) - Center of the circle, r - radius 
};

struct Triangle {
     Point p1, p2, p3; // vertices of the triangle 
     Edge e1, e2, e3;  // edges of the triangle
     Circle circle;    // circubscribed circle

     Triangle(const Point& p1, const Point& p2, const Point& p3) :
          p1(p1), p2(p2), p3(p3), e1(p1, p2), e2(p2, p3), e3(p1, p3) {

          //find the circumscribed circle
          double A1 = p2.x - p1.x; // A1*x + B1*y = C1 - perpendicular bisector 1
          double B1 = p2.y - p1.y;
          double C1 = (p1.x + p2.x) / 2 * A1 + (p1.y + p2.y) / 2 * B1;

          double A2 = p3.x - p1.x; // A2*x + B2*y = C2 - perpendicular bisector 2
          double B2 = p3.y - p1.y;
          double C2 = (p1.x + p3.x) / 2 * A2 + (p1.y + p3.y) / 2 * B2;

          double det = A1 * B2 - A2 * B1;

          circle.x = (B2 * C1 - B1 * C2) / det;  // intersection points of the
          circle.y = (A1 * C2 - A2 * C1) / det;  // perpendicular bisectors
          circle.r = dist(p1, { circle.x, circle.y });
     }

     bool containsPoint(const Point& p) {
          return dist({ circle.x, circle.y }, p) < circle.r;
     }
};

struct VoronoiDelaunay {
     vector<Triangle> delaunayTriangles;   //delaunay triangles 
     vector<Edge> delaunayEdges;           //edges of delaunay triangulation
     vector<Edge> voronoiEdges;            //edges of Voronoi diagram
};

//Input: vector of points, width and height of the window
//Output: Delaunay triangulation of the points and the corresponding Voronoi diagram
VoronoiDelaunay computeTriangulation(vector<Point> points, int width, int height) {
     if (points.empty()) return VoronoiDelaunay();     
     VoronoiDelaunay VD; // contains the edges of the delaunay triangulation and corresponding Voronoi diagram

     // Create super triangle
     double xmin = 0, ymin = 0, xmax = width, ymax = height;

     double dmax = 3 * max(xmax - xmin, ymax - ymin);
     double xmid = (xmin + xmax) / 2;
     double ymid = (ymin + ymax) / 2;

     double k = 30;
     Point p1(xmid - k * dmax, ymid - dmax);
     Point p2(xmid + k * dmax, ymid - dmax);
     Point p3(xmid, ymid + k * dmax);

     VD.delaunayTriangles.push_back(Triangle(p1, p2, p3)); //insert super triangle into triangulation
     for (auto point : points) {   // iterate over the list of points
          vector<Triangle> currTriangulation;
          vector<Edge> edges;      // edges of bad triangles
          for (auto triangle : VD.delaunayTriangles) {
               if (!triangle.containsPoint(point)) currTriangulation.push_back(triangle); 
               else edges.insert(edges.end(), { triangle.e1, triangle.e2, triangle.e3 });
          }
         
          //detect repeating edges of bad triangles
          vector<bool> isDuplicate(edges.size(), 0);
          for (auto e1 = edges.begin(); e1 != edges.end(); ++e1) {
               for (auto e2 = e1 + 1; e2 != edges.end(); ++e2) {
                    if (*e1 == *e2) {
                         isDuplicate[e1 - edges.begin()] = 1;
                         isDuplicate[e2 - edges.begin()] = 1;
                    }
               }
          }

          //remove repeating edges
          edges.erase(remove_if(edges.begin(), edges.end(),
               [&](const Edge& e) {return isDuplicate[&e - &edges[0]]; }),
               edges.end());

          //retriangulate the polygonal hole
          for (auto e : edges) {
               currTriangulation.push_back(Triangle(e.p1, e.p2, point));
          }
          VD.delaunayTriangles = currTriangulation;
     }

     // insert the corresponding to the triangulation Voronoi edges 
     VD.voronoiEdges.push_back(Edge(Point(1,1), Point(2,3)));
     for (auto it = VD.delaunayTriangles.begin(); it != VD.delaunayTriangles.end(); it++) {
          for (auto it2 = it + 1; it2 != VD.delaunayTriangles.end(); it2++) {
               if (it->e1 == it2->e1 || it->e1 == it2->e2 || it->e1 == it2->e3 ||
                   it->e2 == it2->e1 || it->e2 == it2->e2 || it->e2 == it2->e3 ||
                   it->e3 == it2->e1 || it->e3 == it2->e2 || it->e3 == it2->e3) {
                   VD.voronoiEdges.push_back(Edge({ it->circle.x, it->circle.y }, { it2->circle.x, it2->circle.y }));
               }
          }
     }

     // Remove super triangle 
     for (auto it = VD.delaunayTriangles.begin(); it != VD.delaunayTriangles.end();) {
          if (it->p1 == p1 || it->p2 == p1 || it->p3 == p1) it = VD.delaunayTriangles.erase(it);
          else if (it->p1 == p2 || it->p2 == p2 || it->p3 == p2) it = VD.delaunayTriangles.erase(it);
          else if (it->p1 == p3 || it->p2 == p3 || it->p3 == p3) it = VD.delaunayTriangles.erase(it);

          else ++it;
     }

     // Add edges of triangles to the Delaunay triangulation vector
     for (auto triangle : VD.delaunayTriangles) {
          VD.delaunayEdges.push_back(triangle.e1);
          VD.delaunayEdges.push_back(triangle.e2);
          VD.delaunayEdges.push_back(triangle.e3);
     }

     return VD;
}
