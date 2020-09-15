
## Visitor Pattern

The [GOF book](https://en.wikipedia.org/wiki/Design_Patterns) describes the
[Visitor](https://en.wikipedia.org/wiki/Visitor_pattern) design pattern as:

Represent an operation to be performed on elements of an object structure.  
Visitor lets you define a new operation without changing the classes of the
elements on which it operates.




### The object structure

We start by creating the elements and the structure for a drawing.  
Here elements are shapes (Circles, Triangles and Rectangles) and a drawing is a
collection of shapes.

```C++
class Shape {
public:
  virtual ~Shape() {}

  virtual void setFillColor (const Color &color) = 0;
  virtual void draw(Window &w) = 0;
};
```

We use the [SFML](https://www.sfml-dev.org/index.php) library to render the
shapes in a window.  
We compiled SFML ver 2.5.1 using clang ver 11 and installed it in `~/opt`.

For each shape, we create a
[wrapper](https://en.wikipedia.org/wiki/Adapter_pattern) around the SFML class.

```C++
class Circle : public Shape {
public:

  Circle(float x, float y, float radius);
  void setFillColor(const Color &color);
  void draw(Window &w);

//---------------------------------------------------------
private:
  sf::CircleShape m_shape;
};
```


These wrapper classes allow us to change the interface to shapes without
changing the SFML classes.

A `Drawing` is a [composite](https://en.wikipedia.org/wiki/Composite_pattern)
that represents both a Shape and a container of Shapes.
```C++
class Drawing : public Shape {
public:

  void setFillColor(const Color &c);
  void draw(Window &w);

  template<typename T, typename... A> auto &add(A... a);

//---------------------------------------------------------
private:
  std::vector<std::unique_ptr<Shape>> m_shape;
};
```

Finally we add some boiler-plate from the
[SFML tutorial](https://www.sfml-dev.org/tutorials/2.5/start-linux.php) and
this allows us to display a drawing using something like this:

```C++
  Window window(300, 400, "visitor");

  Drawing d;
  d.add<Circle>(100, 100, 50.f).setFillColor(Color::Red);

  auto &triangle = d.add<Triangle>(100, 200, 50.f);
  triangle.setFillColor(Color::Green);

  window.show(d);
```

You can find the complete implementation for this section in
[shape1.cc](./shape1.cc).  

