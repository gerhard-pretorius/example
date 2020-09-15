/*
clang++ -std=c++20 shape5.cc \
  -I ~/opt/include \
  -L ~/opt/lib -lsfml-graphics -lsfml-window -lsfml-system
*/


#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <memory>
#include <tuple>
#include <variant>
#include <utility>

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
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


//=========================================================
class Drawing;


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
class Circle {
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

//---------------------------------------------------------
private:
  sf::CircleShape m_shape;
};


//=========================================================
class Triangle {
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

//---------------------------------------------------------
private:
  sf::CircleShape m_shape;
};


//=========================================================
class Rectangle {
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

//---------------------------------------------------------
private:
  sf::RectangleShape m_shape;
};


//=========================================================
using Shape = std::variant<Circle, Triangle, Rectangle, Drawing>;


//=========================================================
class Drawing {
public:
  
  void setFillColor(const Color &c)
  {
    for (auto &s : m_shape) {
     std::visit(overloaded {
       [&](Circle &s) { s.setFillColor(c); },
       [&](Triangle &s) { s.setFillColor(c); },
       [&](Rectangle &s) { s.setFillColor(c); },
       [&](Drawing &s) { s.setFillColor(c); }
     },s);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  void draw(Window &w)
  { 
    for (auto &s : m_shape) {
     std::visit(overloaded {
       [&](Circle &s) { s.draw(w); },
       [&](Triangle &s) { s.draw(w); },
       [&](Rectangle &s) { s.draw(w); },
       [&](Drawing &s) { s.draw(w); }
     },s);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  template<typename T, typename... A>
  auto &add(A... a)
  {
    auto &tmp = m_shape.emplace_back(std::in_place_type<T>, std::forward<A>(a)...);
    return std::get<T>(tmp);
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
  std::vector<Shape> m_shape;
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

  return 0;
}
