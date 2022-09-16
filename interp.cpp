#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby }, //
   {"border" , &interpreter::do_border }, //
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  }, //
   {"triangle" , &interpreter::make_polygon }, //
   {"equilateral", &interpreter::make_equil  }, //
   
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::check(param begin, param end, int i){
   if(distance(begin, end) != i) throw runtime_error ("syntax error");
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(distance(begin, end) < 2) throw runtime_error ("syntax error");
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   //itor->second->draw (where, color);
   object myShape;
   myShape.pshape = itor->second;
   myShape.center = where;
   myShape.color = color;
   
   if(window::select_copy == 0){
      myShape.is_selected = true;
   }
   myShape.obj_num = window::select_copy++;
   window::objects.push_back(myShape);
}

void interpreter::do_moveby (param begin, param end){
   DEBUGF ('f', range (begin, end));
   check(begin, end, 1);
   object::do_moveby = from_string<GLfloat> (*begin);
}

void interpreter::do_border (param begin, param end){
   DEBUGF ('f', range (begin, end));
   check(begin, end, 2);
   object::bcolor = *++begin;
   object::thick = from_string<GLfloat>(*begin);
}




shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string texts;
   auto font = shape::fontcode.find(*begin);
   if(font == shape::fontcode.end()){
      throw runtime_error ("font does not exist");
   }
   ++begin;
   while(begin != end){
      texts += *begin++ + " ";
   }
   return make_shared<text> (font->second, texts);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 2);
   
   GLfloat width = from_string<GLfloat>(*begin++);
   //*begin++;
   GLfloat length = from_string<GLfloat>(*begin);
   
   return make_shared<ellipse> (width, length);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 1);
   GLfloat dia = from_string<GLfloat>(*begin);
   return make_shared<circle> (dia);
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   vertex_list vertList;
   vertex vert;
   while(begin != end){
      vert.xpos = from_string<GLfloat>(*begin++);
      vert.ypos = from_string<GLfloat>(*begin++);
      vertList.push_back(vert);
   }
   return make_shared<polygon> (vertList);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 2);
   GLfloat width = from_string<GLfloat>(*begin++);
   GLfloat length = from_string<GLfloat>(*begin);
   return make_shared<rectangle> (width, length);
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 1);
   GLfloat length = from_string<GLfloat>(*begin);
   return make_shared<square> (length);
}

shape_ptr interpreter::make_equil (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 1);
   int coor = stoi(*begin);
   
   coor /= 2;
   parameters par;
   
   par.push_back(to_string(coor));
   par.push_back(*begin);
   
   par.push_back("0");
   par.push_back("0");
   
   par.push_back(*begin);
   par.push_back("0");
   param beg_ = par.cbegin();
   return make_polygon(beg_, par.cend());
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   check(begin, end, 2);
   GLfloat width = from_string<GLfloat>(*begin++);
   GLfloat length = from_string<GLfloat>(*begin);
   return make_shared<diamond> (width, length);
}

