//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#ifndef PXL_ALGORITHMS_AUTOLAYOUT_HH
#define PXL_ALGORITHMS_AUTOLAYOUT_HH

#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

#include "pxl/core/macros.hh"

namespace pxl {

class PXL_DLL_EXPORT Vector2
{
public:
	typedef double value_t;

	value_t x, y;

	Vector2() :
		x(0.0), y(0.0)
	{
	}
	Vector2(const Vector2& copy) :
		x(copy.x), y(copy.y)
	{
	}
	Vector2(value_t a, value_t b) :
		x(a), y(b)
	{
	}

	inline Vector2 operator +(const Vector2& op) const
	{
		return Vector2(x + op.x, y + op.y);
	}

	inline const Vector2& operator +=(const Vector2& op)
	{
		x += op.x;
		y += op.y;
		return *this;
	}

	inline Vector2 operator -(const Vector2& op) const
	{
		return Vector2(x - op.x, y - op.y);
	}

	inline const Vector2& operator -=(const Vector2& op)
	{
		x -= op.x;
		y -= op.y;
		return *this;
	}

	inline Vector2 operator *(value_t op) const
	{
		return Vector2(x * op, y * op);
	}

	inline void normalize()
	{
		value_t ol = 1.0/length();
		x *= ol;
		y *= ol;
	}

	inline void scale(value_t d)
	{
		value_t l = d/length();
		x *= l;
		y *= l;
	}

	inline Vector2 scaled(value_t d)
	{
		value_t l = d/length();
		return Vector2(x * l, y * l);
	}

	inline Vector2 normalized()
	{
		value_t ol = 1.0/length();
		return Vector2(x * ol, y * ol);
	}

	inline value_t length() const
	{
		return sqrt(x*x+y*y);
	}

	inline value_t length2() const
	{
		return (x*x+y*y);
	}

	inline void zero()
	{
		x = 0.0;
		y = 0.0;
	}

	bool parallelTo(const Vector2 &b);
};


class PXL_DLL_EXPORT Line
{
public:
	Vector2 a, b;

	Line()
	{

	}

	Line(const Vector2& v1, const Vector2& v2) :
		a(v1), b(v2)
	{

	}

	bool intersection(const Line& line, Vector2& intersection);
};


class PXL_DLL_EXPORT Rect2
{
public:
	Vector2 ll;
	Vector2 ur;

	Rect2() :
		ll(std::numeric_limits<Vector2::value_t>::max(), std::numeric_limits<
				Vector2::value_t>::max()), ur(std::numeric_limits<
				Vector2::value_t>::min(),
				std::numeric_limits<Vector2::value_t>::min())
	{

	}

	Rect2(const Vector2& point) :
		ll(point), ur(point)
	{

	}

	void expand(const Vector2& point);

	void expand(const Rect2& rect)
	{
		expand(rect.ll);
		expand(rect.ur);
	}

	bool contains(const Vector2& point, double margin = 0) const;

	bool overlaps(const Rect2& rhs) const;
};

/* class Eventview
{
	std::vector<Node*> view;
};*/
class PXL_DLL_EXPORT Node
{
public:
	Vector2 velocity;
	Vector2 force;
	double mass;
	bool active;
	Rect2 rect;
	bool updated;
	Node* lastvisit;

	bool isMotherOf(Node *node);
	bool isDaughterOf(Node *node);
	void updateRect();

	int numberchildren;
	int boxednodes;

//	std::vector<double> childrenpositionx;
//	std::vector<double> childrenpositiony;




	Node();

	Vector2 position;
	std::vector<Node*> mothers;
	std::vector<Node*> children;
	bool isVertex;

};

/**
 algorithm for automatic layouting of decay trees
 */

class PXL_DLL_EXPORT AutoLayout
{
	double _friction;
	double _spring;
	double _proximity;
	double _gravity;
	double _mass;
	double _overlap;
	int _steps;
	int _levels;

public:
	std::vector<Node*> allNodes;
	std::vector<Node*> initialNodes;
	std::vector<Node*> activeNodes;
	std::vector<Node*> orphans;

	double margin;
	double grid;
	double border;

	AutoLayout();
	~AutoLayout();

	void init(const std::vector<Node*>& nodes);
	void layout(bool forceBased);
	void layoutSteps();
	void step();

	void reset();
	void applyGravity();
	void applyProximityForces();
	void applySpringForce (Node* a, Node* b, double length);
	void applySpringForces();
	void applyFrictionForces();
	void applyInitialConstraints();
	void resolveBackpointingChildren();
	void applyProximityForces(Node* lhs, Node* rhs);
	void applyProximityForcesTree(Node* parent, Node* node);

	void resolveOverlap(Node *a1, Node *b1, Node *a2, Node *b2);
	void resolveOverlaps();
	bool isStable();
	void placeInitials();
//	void commitPositions();
	void activateChildren();

	int countChildren(Node* Node);
	void layoutChildren(Node* Node, double scale);
	void box(Node* Node, double scale);
	bool box_resolveOverlaps(Node* Node, double scale);
	bool box_resolveOverlap(Node *a1, Node *b1, Node *a2, Node *b2, double scale);
	void layout_children(Node* Node);
	void layout_orphans();

	void fillNodeGrid(std::vector<std::vector<Node*> >& nodeGrid, Node* node, unsigned int depth);
	void new_initial();
	void drawbox();
};

} // namespace pxl

#endif // PXL_AUTOLAYOUT_HH
