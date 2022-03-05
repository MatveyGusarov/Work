#include <iostream>
#include <vector>
#include <cmath>

const double zero = 1e-6;

class Line;

struct Point {
  Point() = default;

  Point(double x, double y) : x(x), y(y) {}

  Point(const Point &p) : x(p.x), y(p.y) {}

  Point &operator+=(const Point& p) {
    x += p.x;
    y += p.y;
    return *this;
  }

  Point &operator-=(const Point& p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  Point &operator*=(double d) {
    x *= d;
    y *= d;
    return *this;
  }

  Point &operator/=(double d) {
    if (std::abs(d) < zero) {
      return *this;
    }
    x /= d;
    y /= d;
    return *this;
  }

  Point operator-() const {
    return Point(-x, -y);
  }

  double abs() const {
    return sqrt(x * x + y * y);
  }

  void rotate(const Point& center, double angle) {
    *this -= center;
    angle *= M_PI / 180;
    double newX = x * cos(angle) - y * sin(angle);
    y = x * sin(angle) + y * cos(angle);
    x = newX;
    *this += center;
  }

  void reflect(const Point& center) {
    x += 2 * (center.x - x);
    y += 2 * (center.y - y);
  }

  void scale(const Point& center, double coefficient) {
    x += (coefficient - 1) * (x - center.x);
    y += (coefficient - 1) * (y - center.y);
  }

  void reflect(const Line& line);

  double x;
  double y;
};

Point operator+(const Point& p1, const Point& p2) {
  Point copy = p1;
  copy += p2;
  return copy;
}

Point operator-(const Point& p1, const Point& p2) {
  Point copy = p1;
  copy -= p2;
  return copy;
}

Point operator*(double d, const Point& p) {
  Point copy = p;
  copy *= d;
  return copy;
}

Point operator*(const Point& p, double d) {
  Point copy = p;
  copy *= d;
  return copy;
}

Point operator/(double d, const Point& p) {
  Point copy = p;
  copy /= d;
  return copy;
}

Point operator/(const Point& p, double d) {
  Point copy = p;
  copy /= d;
  return copy;
}

bool operator==(const Point& p1, const Point& p2) {
  return (std::abs(p1.x - p2.x) < zero) && (std::abs(p1.y - p2.y) < zero);
}

bool operator!=(const Point& p1, const Point& p2) {
  return !(p1 == p2);
}

std::ostream &operator<<(std::ostream& out, const Point& p) {
  out << "(" << p.x << "," << p.y << ")";
  return out;
}

class Vector : public Point {
 public:
  using Point::x;
  using Point::y;

  void norm() {
    double abs = this->abs();
    if (abs < zero) {
      return;
    }
    x /= abs;
    y /= abs;
  }

  explicit Vector(const Point &p, bool norm = true) : Point::Point(p.x, p.y) {
    if (norm) this->norm();
  }

  explicit Vector(double x, double y, bool norm = true) : Point::Point(x, y) {
    if (norm) this->norm();
  }

  Vector(const Vector &v, bool norm = true) : Point::Point(v.x, v.y) { if (norm) this->norm(); }

  explicit operator bool() const {
    return !(std::abs(x) < zero && std::abs(y) < zero);
  }

};

class Line {
 public:
  Line(const Point& p1, const Point& p2) : a(p2 - p1, true), r0(p1) {}

  Line(double k, double b) : a(1, k, true), r0(0, b) {}

  Line(double k, const Point& p) : a(1, k, true), r0(p) {}

  Line(const Point& p, double k) : a(1, k, true), r0(p) {}

  Vector GetA() const { return a; }

  Point GetR0() const { return r0; }

 private:
  Vector a;
  Point r0;
};

void Point::reflect(const Line& line) {
  double a, b, c, p;
  a = line.GetA().y;
  b = -line.GetA().x;
  c = line.GetA().x * line.GetR0().y - line.GetA().y * line.GetR0().x;
  Vector norm = Vector(a, b, true);
  p = (a * x + b * y + c);
  *this -= 2 * p / sqrt(a * a + b * b) * norm;
}

bool operator==(const Line& l1, const Line& l2) {
  Vector r = Vector(l2.GetR0() - l1.GetR0(), true);
  Vector a = l1.GetA();
  return ((a == l2.GetA()) || (a == -l2.GetA())) && (Vector(l2.GetR0() - l1.GetR0(), false) || (r == a) || (r == -a));
}

bool operator!=(const Line& l1, const Line& l2) {
  return !(l1 == l2);
}

class Shape {
 public:
  virtual double perimeter() = 0;

  virtual double area() = 0;

  virtual bool isCongruentTo(const Shape &another) = 0;

  virtual bool isSimilarTo(const Shape &another) = 0;

  virtual bool containsPoint(const Point &p) = 0;

  virtual bool operator==(const Shape &another) const = 0;

  bool operator!=(const Shape &s) const { return !(*(this) == s); }

  virtual void rotate(const Point &center, double angle) = 0;

  virtual void reflect(const Point &center) = 0;

  virtual void reflect(const Line &axis) = 0;

  virtual void scale(const Point &center, double coefficient) = 0;

  virtual ~Shape() = default;
};

double vectorMultiply(const Vector& v1, const Vector& v2) {
  return v1.x * v2.y - v1.y * v2.x;
}

class Polygon : public Shape {
 public:
  Polygon() = default;

  template<typename ...T>
  Polygon(const Point &p, T... args): Polygon(args...) {
    vertices.push_back(p);
  }

  Polygon(const std::vector<Point> &v) : vertices(v) {  }

  ~Polygon() override = default;

  double perimeter() override {
    double p = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n - 1; ++i) {
      p += (vertices[i] - vertices[i + 1]).abs();
    }
    p += (vertices[0] - vertices[n - 1]).abs();
    return p;
  }

  double area() override {
    double area = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
      area += (vertices[i].x * (vertices[(i + 1) % n].y - vertices[(i + n - 1) % n].y));
    }
    return std::abs(area) / 2;
  }

  bool containsPoint(const Point &p) override {
    size_t intersections = 0;
    size_t n = vertices.size();
    double x1 = p.x, x2 = 1000.0001, y1 = p.y, y2 = 0.001;
    for (size_t i = 1; i <= n; ++i) {
      double x3 = vertices[i % n].x, x4 = vertices[i - 1].x, y3 = vertices[i % n].y, y4 = vertices[i - 1].y;
      double det = (y4 - y3) * (x1 - x2) - (x4 - x3) * (y1 - y2);
      if (std::abs(det) < zero)
        intersections += (
            (std::abs((x1 * y2 - x2 * y1) * (x4 - x3) - (x3 * y4 - x4 * y3) * (x2 - x1)) < zero) &&
                (std::abs((x1 * y2 - x2 * y1) * (y4 - y3) - (x3 * y4 - x4 * y3) * (y2 - y1)) < zero));
      else {
        double k1 = ((x4 - x2) * (y4 - y3) - (x4 - x3) * (y4 - y2)) / det;
        double k2 = ((x1 - x2) * (y4 - y2) - (x4 - x2) * (y1 - y2)) / det;
        intersections += (-zero < k1 && k1 < 1 + zero && -zero < k2 && k2 < 1 + zero);
      }
    }
    return intersections % 2;
  }

  bool isCongruentTo(const Shape& s) override {
    const Polygon* p = dynamic_cast<const Polygon*>(&s);
    size_t n = vertices.size();
    if (p == nullptr || p->vertices.size() != n) return false;
    bool congruent = false;
    size_t i = 1;
    while (!congruent && i <= n) {
      bool t = true;
      for (size_t j = 1; j < n; ++j) {
        t &= (std::abs((vertices[(i + j - 1) % n] - vertices[(i + j) % n]).abs() -
            (p->vertices[(i + j - 1) % n] - p->vertices[(i + j) % n]).abs()) < zero);
      }
      congruent |= t;
      ++i;
    }
    i = 1;
    while (!congruent && i <= n) {
      bool t = true;
      for (size_t j = 1; j < n; ++j) {
        t &= (std::abs((vertices[(n + i - j - 2) % n] - vertices[(n + i - 1 - j) % n]).abs() -
            (p->vertices[(i + j - 1) % n] - p->vertices[(i + j) % n]).abs()) < zero);
      }
      congruent |= t;
      ++i;
    }
    return congruent;
  }

  bool isSimilarTo(const Shape& s) override {
    const Polygon* p = dynamic_cast<const Polygon*>(&s);
    size_t n = vertices.size();
    if (p == nullptr || p->vertices.size() != n) return false;
    bool similar = false;
    size_t i = 1;
    double len1 = (vertices[i - 1] - vertices[i]).abs();
    while (!similar && i <= n) {
      bool t = true;
      double k = len1 / (p->vertices[i - 1] - p->vertices[i % n]).abs();
      for (size_t j = 1; j < n; ++j) {
        t &= (std::abs(k - (vertices[(i + j - 1) % n] - vertices[(i + j) % n]).abs() /
            (p->vertices[(i + j - 1) % n] - p->vertices[(i + j) % n]).abs()) < zero);
      }
      similar |= t;
      ++i;
    }
    i = 1;
    while (!similar && i <= n) {
      bool t = true;
      double k = len1 / (p->vertices[i - 1] - p->vertices[i % n]).abs();
      for (size_t j = 1; j < n; ++j) {
        t &= (std::abs(k - (vertices[(n + i - j - 2) % n] - vertices[(n + i - 1 - j) % n]).abs() /
            (p->vertices[(i + j - 1) % n] - p->vertices[(i + j) % n]).abs()) < zero);
      }
      similar |= t;
      ++i;
    }
    return similar;
  }

  std::vector<Point> getVertices() const { return vertices; }

  bool isConvex() const {
    bool signVec = vectorMultiply(Vector(vertices[1] - vertices[0], false), Vector(vertices[2] - vertices[1], false)) >= 0;
    bool isConvex = true;
    size_t i = 3;
    size_t n = vertices.size();
    while (i < n && isConvex) {
      bool t = signVec;
      signVec =
          vectorMultiply(Vector(vertices[i - 1] - vertices[i - 2], false), Vector(vertices[i] - vertices[i - 1], false)) >=
              0;
      isConvex &= (signVec == t);
      ++i;
    }
    return isConvex;
  }

  bool operator==(const Shape &s) const override {
    const Polygon* p = dynamic_cast<const Polygon*>(&s);
    size_t n = vertices.size();
    if (p == nullptr || p->vertices.size() != n) return false;
    bool equal = false;
    size_t i = 0;

    while (!equal && i < n) {
      bool b = true;
      for (size_t j = 0; j < n; ++j) {
        b &= (p->vertices[(j + i) % n] == vertices[j]);
      }
      equal |= b;
      ++i;
    }
    i = 0;
    while (!equal && i < n) {
      bool t = true;
      for (size_t j = 0; j < n; ++j) {
        t &= (p->vertices[(n + i - 1 - j) % n] == vertices[j]);
      }
      equal |= t;
      ++i;
    }
    return equal;
  }

  void rotate(const Point &center, double angle) override {
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
      vertices[i].rotate(center, angle);
    }
  }

  void reflect(const Point &center) override {
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
      vertices[i].reflect(center);
    }
  }

  void reflect(const Line &axis) override {
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
      vertices[i].reflect(axis);
    }
  }

  void scale(const Point &center, double coefficient) override {
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
      vertices[i].scale(center, coefficient);
    }
  }

 protected:
  std::vector<Point> vertices;
};

class Ellipse : public Shape {
 public:
  Ellipse(const Point& f1, const Point& f2, double s) : f1(f1), f2(f2), a(s / 2) {}

  ~Ellipse() override = default;

  std::pair<Point, Point> focuses() const { return {f1, f2}; }

  double eccentricity() const {
    return c() / a;
  }

  Point center() const { return (f1 + f2) / 2; }

  std::pair<Line, Line> directrices() const {
    double x = a / eccentricity();
    auto n = Vector((f1 - f2).y, -(f1 - f2).x, true);
    auto v = Vector(f1 - f2, true);
    auto p1 = center() - v * x;
    auto p2 = center() + v * x;
    return {Line(p1, p1 + n), Line(p2, p2 + n)};
  }

  bool operator==(const Shape &s) const override {
    const Ellipse* e = dynamic_cast<const Ellipse*>(&s);
    if (e == nullptr) return false;
    return ((a == e->a) && (((f1 == e->f1) && (f2 == e->f2)) || ((f1 == e->f2) && (f2 == e->f1))));
  }


  double perimeter() override {
    return (M_PI * (3 * (a + b()) - sqrt((3 * a + b()) * (a + 3 * b()))));
  }

  double area() override {
    return M_PI * a * b();
  }

  bool containsPoint(const Point &p) override {
    return ((f1 - p).abs() + (f2 - p).abs() - 2 * a < zero);
  }

  bool isCongruentTo(const Shape& s) override {
    const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&s);
    if (ellipse == nullptr) return false;
    Vector v1(f2 - f1, false);
    Vector v2(ellipse->f2 - ellipse->f1, false);
    return ((std::abs(v2.abs() - v1.abs()) < zero) && (std::abs(ellipse->a - a) < zero));
  }

  bool isSimilarTo(const Shape& s) override {
    const Ellipse* ellipse = dynamic_cast<const Ellipse*>(&s);
    if (ellipse == nullptr) return false;
    Vector v1(f2 - f1, false);
    Vector v2(ellipse->f2 - ellipse->f1, false);
    return (v2.abs() * a - v1.abs() * ellipse->a) < zero;
  }

  void rotate(const Point& center, double angle) override {
    f1.rotate(center, angle);
    f2.rotate(center, angle);
  }

  void reflect(const Point& center) override {
    f1.reflect(center);
    f2.reflect(center);
  }

  void reflect(const Line& axis) override {
    f1.reflect(axis);
    f2.reflect(axis);
  }

  void scale(const Point& center, double coefficient) override {
    f1.scale(center, coefficient);
    f2.scale(center, coefficient);
    a *= coefficient;
  }


 protected:
  Point f1;
  Point f2;
  double a;

  double c() const { return (f1 - f2).abs() / 2; }

  double b() const {
    return sqrt(a * a - c() * c());
  }

};

class Circle : public Ellipse {
 public:
  using Ellipse::a;

  ~Circle() override = default;

  Circle(const Point &c, double r) : Ellipse::Ellipse(c, c, 2 * r) {}

  double radius() const { return a; }
};

std::ostream& operator<<(std::ostream& out, const Circle& circle) {
  out << circle.center() << " " << circle.radius() << '\n';
  return out;
}

double scalarMultiply(const Vector &v1, const Vector &v2) {
  return v1.x * v2.x + v1.y * v2.y;
}

class Rectangle : public Polygon {
 public:
  Rectangle(const Point& p1, const Point& p2, double attitude) : Polygon::Polygon(p1, p1, p2, p2) {
    if (attitude > 1) attitude = 1 / attitude;
    double angle = atan(attitude) * 180 / M_PI;
    auto diag = Vector(p2 - p1, false);
    Vector v1(diag, true);
    Vector v2(-diag, true);
    v1.rotate(Point(0, 0), -angle);
    v2.rotate(Point(0, 0), -angle);
    Point p3 = p1 + scalarMultiply(v1, Vector(diag, false)) * v1;
    Point p4 = p2 + scalarMultiply(v2, Vector(-diag, false)) * v2;
    Polygon::vertices[0] = p4;
    Polygon::vertices[2] = p3;
  }

  ~Rectangle() override = default;

  Point center() const {
    return (vertices[0] + vertices[2]) / 2;
  }

  std::pair<Line, Line> diagonals() const {
    return std::make_pair(Line(vertices[0], vertices[2]), Line(vertices[3], vertices[4]));
  }
};

std::ostream& operator<<(std::ostream& out, const Rectangle& r) {
  std::vector<Point> verts = r.getVertices();
  out << verts[0] << " " << verts[1] << " " << verts[2] << " " << verts[3] << '\n';
  return out;
}

class Square : public Rectangle {
 public:
  Square(const Point& p1, const Point& p2) : Rectangle::Rectangle(p1, p2, 1) {}

  ~Square() override = default;

  Circle circumscribedCircle() const {
    return Circle((vertices[0] + vertices[2]) / 2, 1 / sqrt(2) * (vertices[0] - vertices[1]).abs());
  }

  Circle inscribedCircle() const {
    return Circle((vertices[0] + vertices[2]) / 2, (vertices[0] - vertices[1]).abs() / 2);
  }

};


class Triangle : public Polygon {
 public:
  Triangle(const Point &p1, const Point &p2, const Point &p3) : Polygon::Polygon(p1, p2, p3) {}

  ~Triangle() override = default;

  Circle circumscribedCircle() const {
    Point c = GetCircumscribedCircleCenter();
    return Circle(c, (c - vertices[0]).abs());
  }

  Circle inscribedCircle() const {
    double xA = vertices[0].x;
    double yA = vertices[0].y;
    double xB = vertices[1].x;
    double yB = vertices[1].y;
    double xC = vertices[2].x;
    double yC = vertices[2].y;
    double lC = sqrt((xB - xA) * (xB - xA) + (yB - yA) * (yB - yA));
    double lA = sqrt((xC - xB) * (xC - xB) + (yC - yB) * (yC - yB));
    double lB = sqrt((xC - xA) * (xC - xA) + (yC - yA) * (yC - yA));
    double S = std::abs(((xB - xA) * (yC - yA) - (xC - xA) * (yB - yA))) / 2;

    return Circle(Point(((lA * xA + lB * xB + lC * xC) / (lA + lB + lC)),
                        ((lA * yA + lB * yB + lC * yC) / (lA + lB + lC))), S / (lC + lA + lB) * 2);
  }

  Point centroid() const {
    return Point((vertices[0] + vertices[1]+ vertices[2])/3);
  }

  Point orthocenter() const {
    double x1 = vertices[0].x;
    double y1 = vertices[0].y;
    double x2 = vertices[1].x;
    double y2 = vertices[1].y;
    double x3 = vertices[2].x;
    double y3 = vertices[2].y;
    double den = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
    return {(y1 * (x3 * x1 + y2 * y2 - x1 * x2 - y3 * y3) - (x2 * x3 + y1 * y1) * (y2 - y3) +
        y2 * (x1 * x2 + y3 * y3) - y3 * (x3 * x1 + y2 * y2)) / den,
            ((x2 - x3) * (x1 * x1 + y2 * y3) - x1 * (x2 * x2 + y3 * y1 - x3 * x3 - y1 * y2) +
                (x2 * x2 + y3 * y1) * x3 - x2 * (x3 * x3 + y1 * y2)) / den};
  }

  Line EulerLine() const { return Line(centroid(), orthocenter()); }

  Circle ninePointsCircle() const {
    return Triangle((vertices[0] + vertices[1]) / 2, (vertices[1] + vertices[2]) / 2,
                    (vertices[2] + vertices[0]) / 2).circumscribedCircle();
  }

 private:
  Point GetCircumscribedCircleCenter() const {
    double a2 = (vertices[0].x * vertices[0].x + vertices[0].y * vertices[0].y);
    double b2 = (vertices[1].x * vertices[1].x + vertices[1].y * vertices[1].y);
    double c2 = (vertices[2].x * vertices[2].x + vertices[2].y * vertices[2].y);
    double yba = (vertices[0].y - vertices[1].y);
    double ycb = (vertices[1].y - vertices[2].y);
    double yac = (vertices[2].y - vertices[0].y);
    double xab = (vertices[0].x - vertices[1].x);
    double xbc = (vertices[1].x - vertices[2].x);
    double xca = (vertices[2].x - vertices[0].x);
    double x0 = -(yba * c2 + ycb * a2 + yac * b2) / (2 * (xab * yac - yba * xca));
    double y0 = (xab * c2 + xbc * a2 + xca * b2) / (2 * (xab * yac - yba * xca));
    return {x0, y0};
  }

};