/*
clang++ -std=c++20 shape4.cc \
  -I ~/opt/include \
  -L ~/opt/lib -lsfml-graphics -lsfml-window -lsfml-system
*/


#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <memory>
#include <tuple>

#include <SFML/Graphics.hpp>


//=========================================================
using Color = sf::Color;

std::ostream &operator<<(std::ostream &os, const Color &c)
{
  auto i = c.toInteger();
  os << std::setw(8) << std::setfill('0') << std::hex << i << std::dec;
  return os;
}


//=========================================================
class Indenter {
public:

  Indenter(int num_space = 2) : m_num_space(num_space)
  {
    m_ilevel += m_num_space;
  }

  ~Indenter()
  {
    m_ilevel -= m_num_space;
    if (m_ilevel < 0)
      m_ilevel = 0;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  friend std::ostream& operator<<(std::ostream &os, const Indenter &ind)
  {
    for (int i = 0; i < ind.m_ilevel; ++i)
      os << ' ';
    return os;
  }

//---------------------------------------------------------
private:
  static int m_ilevel;
  int m_num_space;
};

int Indenter::m_ilevel = 0;


//=========================================================
class Circle;
class Triangle;
class Rectangle;
class Drawing;

//=========================================================
class Visitor {
public:

  virtual ~Visitor() {}

  virtual void visit(Circle &s) = 0;
  virtual void visit(Triangle &s) = 0;
  virtual void visit(Rectangle &s) = 0;
  virtual void visit(Drawing &s) = 0;
};



//=========================================================
class Window : public sf::RenderWindow {
public:
  Window(const int width, const int height, const std::string &title)
    : sf::RenderWindow(sf::VideoMode(width, height), title.c_str())
  {
    setVerticalSyncEnabled(true);
  }

  void show(Drawing &d);
};



//=========================================================
class Shape {
public:
  virtual ~Shape() {}

  virtual void setFillColor (const Color &color) = 0;
  virtual void draw(Window &w) = 0;

  virtual void accept(Visitor &visitor) = 0;
};


//=========================================================
class Circle : public Shape {
public:

  Circle(float x, float y, float radius) : m_shape(radius)
  {
    m_shape.setPosition(sf::Vector2f(x, y));
  }

  void setFillColor(const Color &color) { m_shape.setFillColor(color); }
  const Color &getFillColor() const { return m_shape.getFillColor(); }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void setPosition(float x, float y) { m_shape.setPosition(x, y); }

  std::tuple<float, float> getPosition() const
  {
    auto pos = m_shape.getPosition();
    return std::make_tuple(pos.x, pos.y);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(Window &w) { w.draw(m_shape); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void accept(Visitor &visitor) { visitor.visit(*this); }

//---------------------------------------------------------
private:
  sf::CircleShape m_shape;
};


//=========================================================
class Triangle : public Shape {
public:

  Triangle(float x, float y, float radius) : m_shape(radius, 3)
  {
    m_shape.setPosition(sf::Vector2f(x, y));
  }

  void setFillColor(const Color &color) { m_shape.setFillColor(color); }
  const Color &getFillColor() const { return m_shape.getFillColor(); }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void setPosition(float x, float y) { m_shape.setPosition(x, y); }

  std::tuple<float, float> getPosition() const
  {
    auto pos = m_shape.getPosition();
    return std::make_tuple(pos.x, pos.y);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(Window &w) { w.draw(m_shape); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void accept(Visitor &visitor) { visitor.visit(*this); }

//---------------------------------------------------------
private:
  sf::CircleShape m_shape;
};


//=========================================================
class Rectangle : public Shape {
public:

  Rectangle(float x, float y, float w, float h) : m_shape(sf::Vector2f(w, h))
  {
    m_shape.setPosition(sf::Vector2f(x, y));
  }

  void setFillColor(const Color &color) { m_shape.setFillColor(color); }
  const Color &getFillColor() const { return m_shape.getFillColor(); }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void setPosition(float x, float y) { m_shape.setPosition(x, y); }

  std::tuple<float, float> getPosition() const
  {
    auto pos = m_shape.getPosition();
    return std::make_tuple(pos.x, pos.y);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  std::tuple<int, int> getSize() const
  {
    auto sz = m_shape.getSize();
    return std::make_tuple(sz.x, sz.y);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(Window &w) { w.draw(m_shape); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void accept(Visitor &visitor) { visitor.visit(*this); }

//---------------------------------------------------------
private:
  sf::RectangleShape m_shape;
};


//=========================================================
class Drawing : public Shape {
public:

  void setFillColor(const Color &c)
  {
   std::for_each(m_shape.begin(), m_shape.end(), [&](auto &s) { s->setFillColor(c); });
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(Window &w)
  { 
    std::for_each(m_shape.begin(), m_shape.end(), [&](auto &s) { s->draw(w); });
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void accept(Visitor &visitor) { visitor.visit(*this); }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template<typename T, typename... A>
  auto &add(A... a)
  {
    auto &tmp = m_shape.emplace_back(std::make_unique<T>(std::forward<A>(a)...));
    return * static_cast<T*>(tmp.get());
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  auto begin() { return m_shape.begin(); }
  auto begin() const { return m_shape.begin(); }
  auto cbegin() const { return m_shape.begin(); }

  auto end() { return m_shape.end(); }
  auto end() const { return m_shape.end(); }
  auto cend() const { return m_shape.end(); }

//---------------------------------------------------------
private:
  std::vector<std::unique_ptr<Shape>> m_shape;
};


//=========================================================
void Window::show(Drawing &d)
{
  while(isOpen())
  {
    sf::Event event;
    while (pollEvent(event))
    {
      switch(event.type)
      {
        case sf::Event::Closed: close(); break;
        case sf::Event::KeyPressed:
          switch (event.key.code)
          {
            case sf::Keyboard::Escape: close(); break;
            default: break;
          }
        break;

        default: break;
      }
    }

    clear();
    d.draw(*this);
    display();
  }
}


//=========================================================
class ToJSON : public Visitor {
public:

  ToJSON(std::ostream &os) : m_os(os) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Circle &s)
  {
    auto [x, y] = s.getPosition();

    m_os  << "\"circle\": {\n"
          << "  \"x\": " << x << ",\n"
          << "  \"y\": " << y << ",\n"
          << "  \"color\": " << s.getFillColor()
          << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Triangle &s)
  {
    auto [x, y] = s.getPosition();

    m_os << "\"triangle\": {\n"
       << "  \"x\": " << x << ",\n"
       << "  \"y\": " << y << ",\n"
       << "  \"color\": " << s.getFillColor()
       << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Rectangle &s)
  {
    auto [x, y] = s.getPosition();
    auto [w, h] = s.getSize();

    m_os << "\"rectangle\": {\n"
       << "  \"x\": " << x << ",\n"
       << "  \"y\": " << y << ",\n"
       << "  \"w\": " << w << ",\n"
       << "  \"h\": " << h << ",\n"
       << "  \"color\": " << s.getFillColor()
       << "\n}";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Drawing &d)
  {
    const char *p = "";
    const char *postfix = ",\n";

    m_os << "\"drawing\": [\n";
    std::for_each(d.begin(), d.end(), [&](auto &s)
    {
      m_os << p;
      s->accept(*this);
      p = postfix;
    });

    m_os << "]\n";
  }

//---------------------------------------------------------
private:
  std::ostream &m_os;
};


//=========================================================
class ToYAML : public Visitor {
public:

  ToYAML(std::ostream &os) : m_os(os) {}

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Circle &s)
  {
    Indenter ind;
    auto [x, y] = s.getPosition();

    m_os << "circle:\n"
       << ind << "- x: " << x << '\n'
       << ind << "- y: " << y << '\n'
       << ind << "- color: " << s.getFillColor() << '\n';
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Triangle &s)
  {
    Indenter ind;
    auto [x, y] = s.getPosition();

    m_os << "triangle:\n"
       << ind << "- x: " << x << '\n'
       << ind << "- y: " << y << '\n'
       << ind << "- color: " << s.getFillColor() << '\n';
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Rectangle &s)
  {
    Indenter ind;
    auto [x, y] = s.getPosition();
    auto [w, h] = s.getSize();

    m_os << "rectangle: \n"
       << ind << "- x: " << x << '\n'
       << ind << "- y: " << y << '\n'
       << ind << "- w: " << w << '\n'
       << ind << "- h: " << h << '\n'
       << ind << "- color: " << s.getFillColor() << '\n';
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void visit(Drawing &d)
  {
    Indenter ind;

    m_os << "drawing:\n";
    std::for_each(d.begin(), d.end(), [&](auto &s) {
      m_os << ind << "- ";
      s->accept(*this);
    });

  }

//---------------------------------------------------------
private:
  std::ostream &m_os;
};


//=========================================================
int main()
{
  Window window(300, 400, "visitor");

  Drawing d;
  d.add<Circle>(100, 100, 50.f).setFillColor(Color::Red);

  auto &triangle = d.add<Triangle>(100, 200, 50.f);
  triangle.setFillColor(Color::Green);

  auto &r1 = d.add<Drawing>();
  r1.add<Rectangle>(50, 50, 25, 50);
  r1.add<Rectangle>(75, 75, 25, 50);
  r1.setFillColor(Color::Blue);

  auto &r2 = r1.add<Drawing>();
  r2.add<Rectangle>(50, 150, 25, 50);
  r2.add<Rectangle>(75, 175, 25, 50);

  window.show(d);

  ToJSON json(std::cout);
  d.accept(json);

  ToYAML yaml(std::cout);
  d.accept(yaml);

  return 0;
}
