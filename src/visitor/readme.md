
## Visitor Pattern

The [GOF book](https://en.wikipedia.org/wiki/Design_Patterns) describes the
[Visitor](https://en.wikipedia.org/wiki/Visitor_pattern) design pattern as:

*Represent an operation to be performed on elements of an object structure.*  
*Visitor lets you define a new operation without changing the classes of the
elements on which it operates.*

We explore this pattern using shapes from the 
[SFML](https://www.sfml-dev.org/index.php) library to create a drawing.  



-----------------------------------------------------------

### The object structure

We start by creating the elements and the object structure.  
Here elements are shapes (Circles, Triangles and Rectangles) and a drawing is a
collection of shapes.

Only snippets of code is shown here.  
You can find the complete implementation for this section in
[shape1.cc](./shape1.cc).  

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

  void setFillColor(const Color &c)
  {
   std::for_each(m_shape.begin(), m_shape.end(), [&](auto &s) { s->setFillColor(c); });
  }

  void draw(Window &w)
  { 
    std::for_each(m_shape.begin(), m_shape.end(), [&](auto &s) { s->draw(w); });
  }


  template<typename T, typename... A> auto &add(A... a)

  ...

private:
  std::vector<std::unique_ptr<Shape>> m_shape;
};
```

Note that `Drawing` is a container of `Shape`s and therefore iterates over each
shape calling the appropriate method.

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

This completes the initial setup.  


-----------------------------------------------------------

### Adding a new operation (without the Visitor)

To better understand this pattern, we first add a new operation directly to the
`Shape` hierarchy - that is we do NOT use the Visitor pattern.  
Later we compare this with an implementation that uses the Visitor pattern to
add operations without changing the `Shape` hierarchy.

For this example, we decide to save our drawing to a file in
[JSON](https://en.wikipedia.org/wiki/JSON) format. This requires an operation
that serializes the drawing.

First we add the new operation to our base class:

```C++
class Shape {
public:
  virtual ~Shape() {}

  virtual void setFillColor (const Color &color) = 0;
  virtual void draw(Window &w) = 0;

  virtual void toJSON(std::ostream &os) = 0;
};
```

And then we implement the operation for each element:


```C++
class Circle : public Shape {
public:
  ...


  void toJSON(std::ostream &os)
  {
    auto [x, y] = getPosition();

    os << "\"circle\": {\n"
       << "  \"x\": " << x << ",\n"
       << "  \"y\": " << y << ",\n"
       << "  \"color\": " << getFillColor()
       << "\n}";
  }
};
```

Finally, we can use the operation like this:

```C++
  Window window(300, 400, "visitor");

  Drawing d;
  d.add<Circle>(100, 100, 50.f).setFillColor(Color::Red);

  auto &triangle = d.add<Triangle>(100, 200, 50.f);
  triangle.setFillColor(Color::Green);

  window.show(d);
  d.toJSON(std::cout);
```

This works fine but each time a new operation is added, the `Shape` interface
and all of the elements are changed.  
For example, saving to a different format
like [YAML](https://en.wikipedia.org/wiki/YAML):


```C++
class Shape {
public:
  virtual ~Shape() {}

  virtual void setFillColor (const Color &color) = 0;
  virtual void draw(Window &w) = 0;

  virtual void toJSON(std::ostream &os) = 0;
  virtual void toYAML(std::ostream &os) = 0;
};
```

You can find the complete implementation for this section in
[shape2.cc](./shape2.cc) and [shape3.cc](./shape3.cc).

-----------------------------------------------------------

### Adding a new operation using the Visitor pattern

If you don't want to *pollute* the `Shape` interface, then you can use the
Visitor pattern to add new operations. 

For example, you may want to use the `Shape` interface for operations related to
rendering the drawing in a window and use another interface or class hierarchy
for operations related to saving to a file.

We start by defining our `Visitor` interface:

```C++
class Visitor {
public:

  virtual ~Visitor() {}

  virtual void visit(Circle &s) = 0;
  virtual void visit(Triangle &s) = 0;
  virtual void visit(Rectangle &s) = 0;
  virtual void visit(Drawing &s) = 0;
};
```

We then add a method for dispatching to the visitor in our base class:

```C++
class Shape {
public:
  virtual ~Shape() {}

  virtual void setFillColor (const Color &color) = 0;
  virtual void draw(Window &w) = 0;

  virtual void accept(Visitor &visitor) = 0;
};
```

And implement that method for each element:

```C++
class Circle : public Shape {
public:
  ...

  void accept(Visitor &visitor) { visitor.visit(*this); }
};
```

*Note that although we changed the `Shape` interface by adding the `accept()`
method, we can add new operations without further changes to the interface.*

We create a concrete implementation of the `Visitor` for each new operation:


```C++
class ToJSON : public Visitor {
public:

  void visit(Circle &s)
  {
    auto [x, y] = s.getPosition();

    m_os  << "\"circle\": {\n"
          << "  \"x\": " << x << ",\n"
          << "  \"y\": " << y << ",\n"
          << "  \"color\": " << s.getFillColor()
          << "\n}";
  }

  void visit(Triangle &s)
  ...

};

class ToYAML : public Visitor {
public:

  void visit(Circle &s)
  ...

```


Finally, we can use the new operations like this:

```C++
  Window window(300, 400, "visitor");

  Drawing d;
  d.add<Circle>(100, 100, 50.f).setFillColor(Color::Red);

  auto &triangle = d.add<Triangle>(100, 200, 50.f);
  triangle.setFillColor(Color::Green);

  window.show(d);

  ToJSON json(std::cout);
  d.accept(json);

  ToYAML yaml(std::cout);
  d.accept(yaml);
```

The Visitor pattern now allows us to add any number of new operations without
changing the `Shape` interface or any of the elements.

You can find the complete implementation for this section in [shape4.cc](./shape4.cc).

-----------------------------------------------------------

### Using std::visit and std::variant

[std::visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
is an implementation of the Visitor design pattern in the C++ library.  
It uses
[std::variant](https://en.cppreference.com/w/cpp/utility/variant) which is
[tagged union](https://en.wikipedia.org/wiki/Tagged_union#) holding one of
several types.

This section re-writes the previous examples using `std::variant` instead of
inheritance via the `Shape` interface.

Again start by defining the elements of a drawing:


```C++
class Circle {
public:

  Circle(float x, float y, float radius);
  void setFillColor(const Color &color);
  void draw(Window &w);

private:
  sf::CircleShape m_shape;
};

...
using Shape = std::variant<Circle, Triangle, Rectangle, Drawing>;
```

Note that these no longer implement the `Shape` interface. Instead we define a
`Shape` to be a variant of all the elements - that is, a Shape is one of a
`Circle`, `Triangle`, `Rectangle` or `Drawing`.


```C++
class Drawing {
public:
  
  void setFillColor(const Color &c)
  {
    for (auto &s : m_shape)
      std::visit([&](auto&& t) { t.setFillColor(c); }, s);
  }

  void draw(Window &w)
  { 
    for (auto &s : m_shape)
      std::visit([&](auto &&t) { t.draw(w); }, s);
  }

  template<typename T, typename... A> auto &add(A... a)
  ...

private:
  std::vector<Shape> m_shape;

};
```

As in the previous examples, a `Drawing` is a container of `Shape`s and
therefore iterates over each shape. However, the method call cannot be delegated
to the shape directly since it lacks a common interface. Instead
`std::visit` is used with a lambda. In this case all variant types are treated
the same - that is simply delegate to the shape.

We can now define a Visitor that implements the interface expected by `std::visit`:

```C++
class ToJSON {
public:

  void operator()(Circle &s)
  {
    auto [x, y] = s.getPosition();

    m_os  << "\"circle\": {\n"
          << "  \"x\": " << x << ",\n"
          << "  \"y\": " << y << ",\n"
          << "  \"color\": " << s.getFillColor()
          << "\n}";
  }

  void operator()(Triangle &s)
  ...

  void operator()(Drawing &d)
  {
    const char *p = "";
    const char *postfix = ",\n";

    m_os << "\"drawing\": [\n";
    for (auto &s : d)
    {
      m_os << p;
      std::visit(*this, s);
      p = postfix;
    };

    m_os << "]\n";
  }

  ...
};
```

Note that each Visitor overloads `operator()` for every type in the `Shape`
variant.

Finally, we can use the new operations like this:

```C++
  Window window(300, 400, "visitor");

  Drawing d;
  d.add<Circle>(100, 100, 50.f).setFillColor(Color::Red);

  auto &triangle = d.add<Triangle>(100, 200, 50.f);
  triangle.setFillColor(Color::Green);

  window.show(d);

  ToJSON json(std::cout);
  json(d);

  ToYAML yaml(std::cout);
  yaml(d);
```

You can find the complete implementation for this section in
[shape5.cc](./shape5.cc) and [shape6.cc](./shape6.cc).  


-----------------------------------------------------------
