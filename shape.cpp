// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $

#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"

unordered_map<void*,string> shape::fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

unordered_map<string,void*> shape::fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

//given by example code
ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

//vertex list for drawing diamonds and rectangels
vertex_list rectVert(GLfloat width, GLfloat height){
   vertex_list list;
   vertex rect_vertices {width, height};
   list.push_back(rect_vertices);
   return list;
}

vertex_list diaVert(GLfloat width, GLfloat height){
   vertex_list list;
   vertex rect_vertices {width, height};
   list.push_back(rect_vertices);
   list.push_back(rect_vertices);
   return list;
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon(rectVert(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon(diaVert(width, height)) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

//use draw polygon or ellipse for shapes
void drawNumber(const vertex& center, const rgbcolor& color,
 const size_t& obj_num){
    string readable = "black";
    if(string (color).compare("0x000000") == 0){
       readable = "white";
    }
    rgbcolor num_color {readable};
    glColor3ubv (num_color.ubvec);
    glRasterPos2f (center.xpos, center.ypos);
    glutBitmapString (GLUT_BITMAP_TIMES_ROMAN_24,
     reinterpret_cast<const GLubyte*>(to_string(obj_num).c_str()));
}

 //draws round shapes
 void ellipse::draw (const vertex& center, const rgbcolor& color,
 const bool& selected, const GLfloat& thick, const string& bcolor, 
 const size_t& obj_num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   
   glEnable (GL_LINE_SMOOTH);
   glColor3ubv (color.ubvec);
   glBegin (GL_POLYGON);
   const float delta = 2 * M_PI / 32;
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float ypos = (dimension.ypos/2) * sin (theta) + center.ypos;
      float xpos = (dimension.xpos/2) * cos (theta) + center.xpos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   drawNumber(center, color, obj_num);
   if (selected){
      glLineWidth (thick);
      rgbcolor border {bcolor};
      glEnable (GL_LINE_SMOOTH);
      glColor3ubv (border.ubvec);
      glBegin (GL_LINE_LOOP);
      const float delta = 2 * M_PI / 32;
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float ypos = (dimension.ypos/2) * sin (theta) + center.ypos;
         float xpos = (dimension.xpos/2) * cos (theta) + center.xpos;
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }
} 
 
void text::draw (const vertex& center, const rgbcolor& color,
 const bool& selected, const GLfloat& thick, const string& bcolor, 
 const size_t& obj_num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   
   string textdata_ = textdata + "(" + to_string(obj_num) + ")";
   auto u_str = reinterpret_cast<const GLubyte*>(textdata_.c_str());
   glColor3ubv (color.ubvec);
   glutBitmapString (glut_bitmap_font, u_str);
   glRasterPos2f (center.xpos, center.ypos);
   if (selected){
      rgbcolor border {bcolor};
      int height = glutBitmapHeight (glut_bitmap_font);
      int width = glutBitmapLength (glut_bitmap_font, u_str);
      glLineWidth (thick);
      glBegin (GL_LINE_LOOP);
      glColor3ubv (border.ubvec);
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + width, center.ypos + height);
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos, center.ypos + height);
      glEnd();
   }
}

//draws non round shapes
void polygon::draw (const vertex& center, const rgbcolor& color,
 const bool& selected, const GLfloat& thick, const string& bcolor, 
 const size_t& obj_num) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   
   //for line
   if (vertices.size() == 2){
      glBegin(GL_POLYGON);
      glColor3ubv (color.ubvec);
      GLfloat width = vertices.at(0).xpos;
      GLfloat height = vertices.at(0).ypos;
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos + (width/2), center.ypos + (height/2));
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + (width/2), center.ypos - (height/2));
      glEnd();
      vertex box = {center.xpos+(width/2), center.ypos};
      drawNumber(box, color, obj_num);
      if (selected){
         rgbcolor border {bcolor};
         glLineWidth (thick);
         glBegin (GL_LINE_LOOP);
         glColor3ubv (border.ubvec);
         glVertex2f (center.xpos, center.ypos);
         glVertex2f (center.xpos + (width/2), center.ypos + (height/2));
         glVertex2f (center.xpos + width, center.ypos);
         glVertex2f (center.xpos + (width/2), center.ypos - (height/2));
         glEnd();
      }
      return;
   }

   //rectangle/square
   if (vertices.size() == 1){
      glBegin (GL_POLYGON);
      glColor3ubv (color.ubvec);
      GLfloat width = vertices.at(0).xpos;
      GLfloat height = vertices.at(0).ypos;
      glVertex2f (center.xpos, center.ypos);
      glVertex2f (center.xpos + width, center.ypos);
      glVertex2f (center.xpos + width, center.ypos + height);
      glVertex2f (center.xpos, center.ypos + height);
      glEnd();
      vertex box = {center.xpos+(width/2), center.ypos+(height/2)};
      drawNumber(box, color, obj_num);
      
      //selected
      if (selected){
         rgbcolor border {bcolor};
         glLineWidth (thick);
         glBegin (GL_LINE_LOOP);
         glColor3ubv (border.ubvec);
         glVertex2f (center.xpos, center.ypos);
         glVertex2f (center.xpos + width, center.ypos);
         glVertex2f (center.xpos + width, center.ypos + height);
         glVertex2f (center.xpos, center.ypos + height);
         glEnd();
      }
      return;  
   }

   //triangle
   if (vertices.size() == 3){
      vertex avg {0,0};
      
      
      for (int i = 0; i < 3; i++){
         avg.xpos += vertices.at(i).xpos;
         avg.ypos += vertices.at(i).ypos;  
      }
      avg.ypos = avg.ypos/3; 
      avg.xpos = avg.xpos/3;
      //Adjusting origin
      avg.ypos = (center.ypos - avg.ypos);
      avg.xpos = (center.xpos - avg.xpos);
      glBegin(GL_TRIANGLES);
      glColor3ubv (color.ubvec);
      glVertex2f(vertices.at(0).xpos + avg.xpos, 
                 vertices.at(0).ypos + avg.ypos);
      glVertex2f(vertices.at(1).xpos + avg.xpos, 
                 fabs(vertices.at(1).ypos + avg.ypos));
      glVertex2f(fabs(vertices.at(2).xpos + avg.xpos), 
                 vertices.at(2).ypos + avg.ypos);
      glEnd();
      
      drawNumber(center, color, obj_num);

      //selected
      if (selected){
         glLineWidth (thick);
         rgbcolor border {bcolor};
         glBegin(GL_LINE_LOOP);
         glColor3ubv (border.ubvec);
         glVertex2f(vertices.at(0).xpos + avg.xpos, 
                 vertices.at(0).ypos + avg.ypos);
         glVertex2f(vertices.at(1).xpos + avg.xpos, 
                 fabs(vertices.at(1).ypos + avg.ypos));
         glVertex2f(fabs(vertices.at(2).xpos + avg.xpos), 
                 vertices.at(2).ypos + avg.ypos);
         glEnd();
      }
      return;
   }

   //other
   glBegin(GL_POLYGON);
   glColor3ubv (color.ubvec);
   int size = vertices.size();
   vertex avg {0,0};
   for (int i = 0; i < size; i++){
         avg.ypos += vertices.at(i).ypos;  
         avg.xpos += vertices.at(i).xpos;
      }
      avg.ypos = avg.ypos/size;
      avg.xpos = avg.xpos/size;
      avg.ypos = (center.ypos - avg.ypos); 
      avg.xpos = (center.xpos - avg.xpos);
   for (auto& point: vertices){
      glVertex2f(point.xpos+avg.xpos, 
                 point.ypos+avg.ypos);
   }
   glEnd();
   drawNumber(center, color, obj_num);
   //selected
   if (selected){
      glBegin(GL_LINE_LOOP);
      rgbcolor border {bcolor};
      
      glColor3ubv (border.ubvec);
      glLineWidth (thick);
      vertex avg {0,0};
      int size = vertices.size();
      for (int i = 0; i < size; i++){
         avg.ypos += vertices.at(i).ypos;  
         avg.xpos += vertices.at(i).xpos;
      }
      avg.ypos = avg.ypos/size;
      avg.xpos = avg.xpos/size;
      avg.ypos = (center.ypos - avg.ypos); 
      avg.xpos = (center.xpos - avg.xpos);
      for (auto& point: vertices){
         glVertex2f(point.xpos+avg.xpos, 
                    point.ypos+avg.ypos);
      }
   glEnd();
   }
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

