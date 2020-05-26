#include<iostream>
#include<fstream>
#include<vector>
#include<math.h>
#include<algorithm>
#include <iomanip> 
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <string>
#include <sstream>
#include <stdlib.h> 
#include "RoundedRectangleShape.h"
#include "triangulation.h"

#define NMAX 100000
#define double long double

using namespace std;

//window dimmensions
double width = 1600, height = 900;
bool inverseColor = false;
vector<Point> points;
VoronoiDelaunay VD;

//sfml shapes
vector<sf::ConvexShape> polygons(NMAX);
vector<sf::CircleShape> circles(NMAX);
vector<sf::Vertex[2]> voronoiLines(NMAX);
vector<sf::Vertex[2]> delaunayLines(NMAX);
vector<sf::CircleShape> delaunayCircles(NMAX);

void createSfmlShapes() {
     int i = 0;
     for (auto e : VD.voronoiEdges) {
          sf::Vertex line[] = {
               sf::Vertex(sf::Vector2f(e.p1.x, e.p1.y), sf::Color(255,150,134)),
               sf::Vertex(sf::Vector2f(e.p2.x, e.p2.y), sf::Color(255,150,134))
          };
          voronoiLines[i][0] = line[0];
          voronoiLines[i][1] = line[1];
          i++;
     }

     for (int i = 0; i < VD.delaunayTriangles.size(); i++) {
          double r = VD.delaunayTriangles[i].circle.r;
          double x = VD.delaunayTriangles[i].circle.x - r;
          double y = VD.delaunayTriangles[i].circle.y - r;

          delaunayCircles[i].setRadius(r);
          delaunayCircles[i].setPosition(x, y);
          delaunayCircles[i].setFillColor(sf::Color::Transparent);
          delaunayCircles[i].setOutlineColor(sf::Color::Color(205, 215, 240, 150));
          delaunayCircles[i].setOutlineThickness(1);
     }

     sf::Color delaunayColor, circleColor;
     if (!inverseColor) delaunayColor = sf::Color::White, circleColor = sf::Color(255, 85, 79);
     else delaunayColor = circleColor = sf::Color::Black;
     i = 0;
     for (auto e : VD.delaunayEdges) {
          sf::Vertex line[] = {
               sf::Vertex(sf::Vector2f(e.p1.x, e.p1.y), delaunayColor),
               sf::Vertex(sf::Vector2f(e.p2.x, e.p2.y), delaunayColor)
          };
          delaunayLines[i][0] = line[0];
          delaunayLines[i][1] = line[1];
          i++;
     }

     for (auto i = 0; i < points.size(); i++) {
          double circleRadius = 6;
          circles[i].setRadius(circleRadius);
          circles[i].setPosition(points[i].x - circleRadius, points[i].y - circleRadius);
          circles[i].setFillColor(circleColor);
     }
}

int pointExists(Point p) {
     for (auto it : points) {
          if (it == p) return 1;
     }
     return 0;
}

int getPointIndex(double x, double y)
{
     int index = -1;
     for (int i = 0; i < points.size(); i++)
     {
          if (abs(x - points[i].x) <= 5 && abs(y - points[i].y) <= 5)
          {
               return i;
          }
     }

     return -1;
}

//reconstructs GUI based on the vector of points 
void reconstructGUI() {
     VD = computeTriangulation(points, width, height);
     createSfmlShapes();
}

//adds a point at position (x,y)
void addPoint(double x, double y, bool updateGui=1) {
    // cout << fixed << setprecision(10) << x << ' ' << y << endl;

     if (pointExists({ x,y })) {
          cout << "Point already exists\n";
          return;
     }
     else if (x > width || y > height) {
          cout << " Point out of bounds\n";
          return;
     }
     cout << "New Point: " << x << " " << y << '\n';
     points.push_back({ x,y });
     if (updateGui) reconstructGUI();
}

//deletes a point at position (x,y)
void deletePoint(double x, double y) {
     int idx = getPointIndex(x, y);
     if (idx == -1) return;
     points.erase(points.begin() + idx);
     reconstructGUI();
}

//new function
void deleteAllPoints() {
     cout << "delete all points\n";
     points.clear();
     reconstructGUI();
}


void tokenize(sf::String str, const char delim,
     std::vector<double>& out)
{
     int index = 0;
     for (int i = 0; i < str.getSize(); i++)
     {
          if (str[i] >= 48 && str[i] <= 57)
          {
               out[index] *= 10;
               out[index] += str[i] - 48;
          }
          else index++;
     }
}


void createBorder(sf::RoundedRectangleShape& box, float x, float y, float width, float height)
{
     box.setSize(sf::Vector2f(width, height));
     box.setPosition(x, y);
     box.setFillColor(sf::Color::White);
     box.setOutlineColor(sf::Color::Color(84, 84, 84));
     box.setOutlineThickness(3);
     box.setCornersRadius(6); //added
     box.setCornerPointCount(300); //added
}
sf::Font font;
void createLabel(sf::Text& label, string text, float x, float y) {
     label.setString("  " + text + "  ");
     label.setFillColor(sf::Color::Black);
     label.setCharacterSize(16);
     label.setPosition(x, y);
     label.setFont(font);
}

void createLabel(sf::Text& label, float x, float y)
{
     sf::Font font;
     if (!font.loadFromFile("fonts/arial.ttf")) return;

     label.setString("Text:");
     label.setFillColor(sf::Color::Black);
     label.setCharacterSize(16);
     label.setPosition(x, y);
     label.setFont(font);
     cout << 2 << endl;
}


int main() {
     sf::RenderWindow window(sf::VideoMode(width, height), "Delaunay Triangulation", sf::Style::Close);
     sf::CircleShape circle;

     if (!font.loadFromFile("fonts/arial.ttf")) return EXIT_FAILURE;
     sf::RoundedRectangleShape inputBorder;
     sf::RoundedRectangleShape saveBorder;
     sf::RoundedRectangleShape clearBorder;
     sf::RoundedRectangleShape importBorder;
     sf::RoundedRectangleShape changeBorder;

     sf::Text inputLabel;
     sf::Text saveLabel;
     sf::Text clearLabel;
     sf::Text importLabel;
     sf::Text changeLabel;


     createLabel(inputLabel, "Text:", 17, 52);
     createLabel(saveLabel, " Save", 17, 82 + 6);
     createLabel(importLabel, "Import", 17, 107 + 12);
     createLabel(clearLabel, " Clear", 17, 132 + 18);
     createLabel(changeLabel, " Color", 17, 180);


     createBorder(inputBorder, 15, 50, 200, 25);
     createBorder(saveBorder, 15, 80 + 6, 65, 25);
     createBorder(importBorder, 15, 105 + 12, 65, 25);
     createBorder(clearBorder, 15, 130 + 18, 65, 25);
     createBorder(changeBorder, 15, 178, 65, 25);


     sf::String yText;
     sf::Event windowEvent;

     stringstream ss;
     string x;

     bool hideVoronoi = 0;
     bool hideCircles = 1;

     int mousePosX, mousePosy;
     std::string position = "";

     while (window.isOpen())
     {
          double x, y;

          sf::Event event;
          sf::RoundedRectangleShape r;
          while (window.pollEvent(event))
          {

               char ch;
               switch (event.type) {
               case sf::Event::TextEntered:
                    ch = char(event.text.unicode);
                    if (ch == 13 && !yText.isEmpty())
                    {
                         vector < double > pos(2, 0);
                         tokenize(yText, ';', pos);
                         x = pos[0];
                         y = pos[1];
                         addPoint(x, y);
                         yText.clear();

                    }
                    else if (ch == '\b' and !yText.isEmpty()) yText.erase(yText.getSize() - 1, 1);
                    else if (ch == ' ' && yText.getSize() && yText.find(' ') == sf::String::InvalidPos) yText += ' ';
                    else if (ch >= '0' && ch<='9' && yText.getSize() < 10) yText += ch;

                    break;
               case sf::Event::MouseMoved:
                    mousePosX = event.mouseMove.x;
                    mousePosy = event.mouseMove.y;

                    position = "x = " + std::to_string(mousePosX) + "    y = " + std::to_string(mousePosy);
                    break;

               case sf::Event::MouseLeft:
                    position = "";
                    break;

               case sf::Event::MouseButtonPressed:
                    if (event.key.code == sf::Mouse::Left) {
                         x = (double)event.mouseButton.x;
                         y = (double)event.mouseButton.y;

                         if (x >= 15 && x <= 80) {
                              if (y >= saveBorder.getPosition().y && y <= saveBorder.getPosition().y + saveBorder.getSize().y) {
                                   cout << "Save clicked\n";
                                   ofstream savedpoints;
                                   savedpoints.open("points.txt");
                                   savedpoints.clear();
                                   for (int i = 0; i < points.size(); i++) {
                                        savedpoints << points[i].x << ' ' << points[i].y << '\n';
                                   }
                                   savedpoints.close();
                              }
                              else if (y >= importBorder.getPosition().y && y <= importBorder.getPosition().y + importBorder.getSize().y) {
                                   cout << "import clicked\n";
                                   points.clear();
                                   reconstructGUI();
                                   ifstream inFile;
                                   inFile.open("points.txt");
                                   double cX, cY;
                                   while (inFile >> cX >> cY)
                                   {
                                        addPoint(cX, cY);
                                   }
                                   inFile.close();
                              }
                              else if (y >= clearBorder.getPosition().y && y <= clearBorder.getPosition().y + clearBorder.getSize().y) {
                                   yText.clear();
                                   deleteAllPoints();
                              }
                              else if (y >= changeBorder.getPosition().y && y <= changeBorder.getPosition().y + changeBorder.getSize().y) {
                                   inverseColor = !inverseColor;
                                   reconstructGUI();
                              }
                              else addPoint(x, y);
                         }
                         else addPoint(x, y);
                    }
                    else if (event.key.code == sf::Mouse::Right) {
                         x = (double)event.mouseButton.x;
                         y = (double)event.mouseButton.y;

                         deletePoint(x, y);
                    }
                    break;
               case sf::Event::KeyReleased:
                    if (event.key.code == sf::Keyboard::V) hideVoronoi = !hideVoronoi;
                    else if (event.key.code == sf::Keyboard::P) hideCircles = !hideCircles;

                    break;
               case sf::Event::Closed:
                    window.close();
                    break;
               }
          }

          inputLabel.setString(yText);


          //update
          if (inverseColor) window.clear(sf::Color::White);
          else window.clear(sf::Color(33, 33, 33));

         for (int i = 0; i < VD.voronoiEdges.size() && !hideVoronoi; i++) {
               window.draw(voronoiLines[i], 2, sf::Lines);
          }

          for (int i = 0; i < VD.delaunayTriangles.size() && !hideCircles; i++) {
               window.draw(delaunayCircles[i]);
          }
          for (int i = 0; i < VD.delaunayEdges.size(); i++) {
               window.draw(delaunayLines[i], 2, sf::Lines);
          }

          for (int i = 0; i < points.size(); i++) {
               window.draw(circles[i]);
               
          }



          inputLabel.setString(yText);



          window.draw(inputBorder);
          window.draw(saveBorder);
          window.draw(importBorder);
          window.draw(clearBorder);
          window.draw(changeBorder);


          window.draw(inputLabel);
          window.draw(saveLabel);
          window.draw(importLabel);
          window.draw(clearLabel);
          window.draw(changeLabel);


          sf::Text mouseValue(position, font, 20);
          mouseValue.setFillColor(sf::Color::White);
          mouseValue.setPosition(15, 20);
          window.draw(mouseValue);
          window.display();
     }

     return 0;
}
