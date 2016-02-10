//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include "pxl/algorithms/AutoLayout.hh"

#include <algorithm>

namespace pxl {

bool Vector2::parallelTo(const Vector2 &b)
{
	if (x == 0)
	{
		if (b.x == 0)
			return true;
		else
			return false;
	}

	if (b.x == 0)
		return false;

	if (y / x == b.y / b.x)
		return true;

	return false;
}


bool Line::intersection(const Line& line, Vector2& intersection)
{
	Vector2 r1 = b - a;
	Vector2 r2 = line.b - line.a;
	Vector2 s = line.a - a;

	double t2 = (r1.x*s.y - r1.y*s.x)/ (r1.y*r2.x - r1.x*r2.y);

	double t1 = (s.x + t2 * r2.x) / r1.x;

	intersection = line.a + r2 * t2;

	if ((t1 > 0.0001) && (t1 < 0.999) && (t2 > 0.0001) && (t2 < 0.999))
		return true;
	else
		return false;
}


void Rect2::expand(const Vector2& point)
{
	ll.x = std::min(ll.x, point.x);
	ll.y = std::min(ll.y, point.y);

	ur.x = std::max(ur.x, point.x);
	ur.y = std::max(ur.y, point.y);
}

bool Rect2::contains(const Vector2& point, double margin) const
{
	if (point.x < (ll.x - margin))
		return false;
	if (point.y < (ll.y - margin))
		return false;

	if (point.x > (ur.x + margin))
		return false;
	if (point.y > (ur.y + margin))
		return false;

	return true;
}

bool Rect2::overlaps(const Rect2& rhs) const
{
	if (ur.x < rhs.ll.x)
		return false;

	if (ur.y < rhs.ll.y)
		return false;

	if (ll.x > rhs.ur.x)
		return false;

	if (ll.y > rhs.ur.y)
		return false;

	return true;
}


Node::Node() : mass(10.0)
{
}

AutoLayout::AutoLayout() :
	_friction(0.9), _spring(0.2), _proximity(0.5),
	_gravity(10.0), _mass(0.1), _overlap(10.0), _steps(400),
	_levels (100000), margin(50.0), grid(100.0), border(100.)
{
#if 1
	/* for finding cirrect values */
/*	std::ifstream file("/root/Desktop/autolayout.txt");
	file >> _friction;
	file >> _spring;
	file >> _proximity;
	file >> _gravity;
	file >> _mass;
	file >> _overlap;
	file >> _steps;
	file >> _levels;
	file >>grid;
	file >>margin;
	*/

#endif
}

AutoLayout::~AutoLayout()
{
}

void AutoLayout::init(const std::vector<Node*>& nodes)
{
	allNodes.clear();
	initialNodes.clear();
	activeNodes.clear();
	
	allNodes.resize(nodes.size());
	std::copy(nodes.begin(), nodes.end(), allNodes.begin());

	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		if ((*i)->mothers.size() == 0)
		{
			(*i)->active = true;
			initialNodes.push_back(*i);
			activeNodes.push_back(*i);
		}
		else
		{
			(*i)->active = false;
		}

	}

	//while (allNodes.size() > activeNodes.size())
		//activateChildren();
}
void Node::updateRect()
{
	if (updated)
		return;

	rect = Rect2(position);

	std::vector<Node *>::iterator i;
	for (i = children.begin(); i != children.end(); i++)
	{
		if ((*i)->active == false)
			continue;

		(*i)->updateRect();
		rect.expand((*i)->rect);
	}

	updated = true;
}

void AutoLayout::reset()
{
	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		(*i)->force.zero();
		(*i)->updated = false;
	}

	for (std::vector<Node*>::iterator i = initialNodes.begin(); i != initialNodes.end(); i++)
		(*i)->updateRect();
}

bool Node::isMotherOf(Node *node)
{
	std::vector<Node *>::iterator i;

	for (i = children.begin(); i != children.end(); i++)
	{
		if (*i == node)
			return true;
	}

	return false;
}

bool Node::isDaughterOf(Node *node)
{
	std::vector<Node *>::iterator i;

	for (i = mothers.begin(); i != mothers.end(); i++)
	{
		if (*i == node)
			return true;
	}

	return false;
}

void AutoLayout::applyGravity()
{
	std::vector<Node*>::iterator i;
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		(*i)->force.x += _gravity;
	}
}
/*
 void AutoLayout::applyProximityForces(Node* a, Node* b)
 {
 if (a->rect.overlaps(b->rect) == false)
 return;

 for (std::vector<Node*>::iterator i = initialNodes.begin(); i != initialNodes.end(); i++)
 {
 }
 }
 */

void AutoLayout::applyProximityForces(Node* lhs, Node* rhs)
{
	if (rhs == lhs)
		return;

	Vector2 r = rhs->position - lhs->position;
	double distance = r.length();
	Vector2 direction;

	if (distance < 1.0)
	{
		lhs->position += Vector2(0, 1);
		direction = Vector2(0.0, 1);
	}
	else
		direction = r.normalized();

	double force = -fabs(grid - distance) * _proximity;

	if (distance < grid)
	{
		lhs->force.y += direction.y * force;
		rhs->force.y -= direction.y * force;
	}
}

void AutoLayout::applyProximityForcesTree(Node* parent, Node* node)
{
	if (parent->rect.contains(node->position, grid) && (parent->lastvisit
			!= node))
	{
		applyProximityForces(parent, node);
		for (std::vector<Node*>::iterator i = parent->children.begin(); i
				!= parent->children.end(); i++)
		{
			Node* child = *i;
			if (child->active == false)
				continue;
			applyProximityForcesTree(child, node);
		}
		parent->lastvisit = node;
	}
}

void AutoLayout::applyProximityForces()
{
	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		Node* node = *i;

		if (node->active == false)
			continue;

		for (std::vector<Node*>::iterator j = initialNodes.begin(); j
				!= initialNodes.end(); j++)
		{
			Node* initial = *j;
			applyProximityForcesTree(initial, node);
		}
	}
}

void AutoLayout::applySpringForce (Node* a, Node* b, double length)
{
	Vector2 r = b->position - a->position;
	double distance = r.length();

	Vector2 direction;
	if (distance == 0.0)
		direction = Vector2(1.0, 0.0);
	else
		direction = r.normalized();

	double force = _spring * (distance - (length/2.0));

	a->force.y += (direction * force).y;
	b->force -= direction * force;
}

void AutoLayout::applySpringForces()
{
	std::vector<Node*>::iterator i;
	for (i = activeNodes.begin(); i != activeNodes.end(); i++)
	{
		Node* mother = *i;

//		Node* sib = 0;
		std::vector<Node*>::iterator j;
		for (j = mother->children.begin(); j != mother->children.end(); j++)
		{
			Node* daughter = *j;

			if (daughter->active == false)
				continue;

			double range = grid;
			if ((daughter->position.x - mother->position.x) > 0)
				range = sqrt(grid*grid + pow(grid*(daughter->position.y - mother->position.y)/(daughter->position.x - mother->position.x), 2));
			applySpringForce (mother, daughter, range);
			/*
			if (sib)
				applySpringForce (daughter, sib, grid);

			sib = daughter;
*/
		}
	}
}

void AutoLayout::applyFrictionForces()
{
	std::vector<Node*>::iterator i;
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		(*i)->force -= (*i)->velocity * _friction;
	}
}

void AutoLayout::applyInitialConstraints()
{
	std::vector<Node*>::iterator i;
	for (i = initialNodes.begin(); i != initialNodes.end(); i++)
	{
		(*i)->force.x = 0;
	}
}

void AutoLayout::step()
{
	std::vector<Node*>::iterator i;
	for (i = activeNodes.begin(); i != activeNodes.end(); i++)
	{
		(*i)->position += (*i)->velocity * 0.1;
		if ((*i)->force.length() > 100.0)
			(*i)->force.scale(100.0);
		(*i)->velocity += (*i)->force * (0.1 / (*i)->mass);
	}
}

void AutoLayout::resolveOverlap(Node *a1, Node *b1, Node *a2, Node *b2)
{
	Line a(a1->position, b1->position);
	Line b(a2->position, b2->position);

	Vector2 intersection;
	if (a.intersection(b, intersection))
	{
		//std::cout << "intersection: " << intersection.x << " " << intersection.y << std::endl;
/*
		Vector2 v = (a1->position - b1->position - a2->position + b2->position) * _overlap;
		b1->force += v;
		b2->force -= v;
*/
		//b1->force += (a1->position - b1->position).scaled(_overlap);
		//b2->force += (a2->position - b2->position).scaled(_overlap);
		//b1->position = a1->position - (a1->position - intersection) * 0.9;
		Vector2 v1 = b1->position - a1->position;
		Vector2 v2 = b2->position - a2->position;
		if (v1.length2() > v2.length2())
		{
			v2.scale (_overlap);
			b2->force -= v2;
			b1->force += v2;
		}
		else
		{
			v1.scale (_overlap);
			b2->force += v1;
			b1->force -= v1;
		}
		//b1->position = intersection;
		//b2->position = intersection;
	}
}

void AutoLayout::resolveOverlaps()
{
	std::vector<Node*>::iterator i;
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		Node *a = *i;
		if (a->active == false)
			continue;

		std::vector<Node*>::iterator j;
		for (j = a->children.begin(); j != a->children.end(); j++)
		{
			Node *b = *j;

			if (b->active == false)
				continue;

			std::vector<Node*>::iterator k = i;
			for (k++; k != allNodes.end(); k++)
			{
				Node *c = *k;

				if (c->active == false)
					continue;

				if (c->isDaughterOf(a))
					continue;

				std::vector<Node*>::iterator l;
				for (l = c->children.begin(); l != c->children.end(); l++)
				{
					Node *d = *l;

					if (d->active == false)
						continue;

					resolveOverlap(a, b, c, d);
				}
			}
		}
	}
}

void AutoLayout::resolveBackpointingChildren()
{
	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		Node *mother = *i;

		if (mother->active == false)
			continue;

		for (std::vector<Node*>::iterator j = mother->children.begin(); j
				!= mother->children.end(); j++)
		{
			Node *daughter = *j;

			if (daughter->active == false)
				continue;

			if (daughter->position.x < mother->position.x)
			{
				daughter->position.x = mother->position.x;
				//if (daughter->velocity.x < 0)
				daughter->force.x += 10;
			}
		}
	}
}

bool AutoLayout::isStable()
{
	std::vector<Node*>::iterator i;
	for (i = activeNodes.begin(); i != activeNodes.end(); i++)
	{
		if ((*i)->force.length() > 0.1 || (*i)->velocity.length() > 0.1)
			return false;
	}

	return true;
}

void AutoLayout::placeInitials()
{
	for (std::vector<Node*>::iterator i = initialNodes.begin(); i != initialNodes.end(); i++)
	{
		(*i)->position.x = margin;
	//	(*i)->position.y = 0;
	}
}


// new algorithm
int AutoLayout::countChildren(Node* object)
{
	object->numberchildren = 0;

	if (object->children.size() != 0)
	{
		std::vector<Node*>::iterator l;

		for (l = object->children.begin(); l != object->children.end(); l++)
		{
			object->numberchildren += countChildren(*l) + 1;
		}
	}
	return object->numberchildren;
}

void AutoLayout::layoutChildren(Node* object, double scale)
{
	box(object,1.0);
	box_resolveOverlaps(object,scale);
/*
	int k = 0;
	std::vector<Node*>::iterator j;

	for (j = object->children.begin(); j != object->children.end(); j++)
	{
		k = k + (*j)->boxednodes;
	}


	if (object->numberchildren != 0 && (object->children.size() + k) == object->numberchildren)
	{
		std::cout<<"children.size == object->numberchildren"<<std::endl;
		box(object,1.0);

			for	(j = object->children.begin(); j != object->children.end(); j++)
			{
//				object->childrenpositionx.push_back((*j)->position.x - object->position.x);
//			  	object->childrenpositiony.push_back((*j)->position.y - object->position.y);
			}
		boxes.push_back(object);
	}
	else
	{
		for (j = object->children.begin(); j != object->children.end(); j++)
		{
			layoutChildren((*j), scale);
		}

	}

	box_resolveOverlaps(object,scale);
*/
}

bool cmp(Node* object1, Node* object2)
{
	return object1->children.size() > object2->children.size();
}
bool cmp2(Node* object1, Node* object2)
{
	return object1->numberchildren > object2->numberchildren;
}

void AutoLayout::box(Node* object, double scale)
{
	double g = grid * 0.9 ;//* 0.5*object->children.size();
	double y = g/2.0;
	int n = 0;

	size_t d;

	double step = g / (object->children.size()-1);

	std::vector<Node*>::iterator j;
	std::vector<Node*>::iterator l;
	std::vector<Node*> v;
	std::vector<Node*> w;
	std::vector<Node*> a;
	std::vector<Node*> b;

	for (j = object->children.begin(); j != object->children.end(); j++)
	{
		v.push_back(*j);
	}


	std::sort( v.begin(), v.end(), cmp);


	for(d = 0; d != v.size(); d++ )
	{
		if(d%2 == 0 )
		a.push_back(v[d]);
		else
		b.push_back(v[d]);
	}

	for(d = 0; d != a.size(); d++ )
	{
		w.push_back(a[d]);
	}
	for(d = 0; d != b.size(); d++ )
	{
		w.push_back(b[b.size()-1-d]);
	}

	for (j = w.begin(); j != w.end(); j++)
	{
		Node* child = *j;

		if (object->children.size() == 1)
			child->position = object->position + Vector2(grid,0);
		else
		{
			child->position = object->position + Vector2(grid,scale*(-y + n*step));
			n++;
		}
	}


	object->boxednodes = object->numberchildren;

}


bool AutoLayout::box_resolveOverlaps(Node* object, double scale)
{
	int n = 0;
	bool overlap;
	std::vector<Node*>::iterator i;
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		Node *a = *i;
		std::vector<Node*>::iterator j;
		for (j = a->children.begin(); j != a->children.end(); j++)
		{
			Node *b = *j;
			std::vector<Node*>::iterator k = i;
			for (k++; k != allNodes.end(); k++)
			{
				Node *c = *k;

				if (c->isDaughterOf(a))
					continue;

				std::vector<Node*>::iterator l;
				for (l = c->children.begin(); l != c->children.end(); l++)
				{
					Node *d = *l;
					overlap = box_resolveOverlap(a, b, c, d, scale);


					do
					{
						overlap = box_resolveOverlap(a, b, c, d, scale*(1+n*0.03));
						n++;

//						std::cout << "Overlap= "<<  overlap <<  std::endl;
//						std::cout << "n= "<<  n <<  std::endl;

//						return true;
//						break;
					}
					while (overlap == true );//&& n!= 500);

					if(overlap == true)
					{
//						box_resolveOverlaps(object, 1.5*scale);
					}

				}
			}

		}
	}
	return false;
}
bool AutoLayout::box_resolveOverlap(Node *a1, Node *b1, Node *a2, Node *b2, double scale)
{
	Line a(a1->position, b1->position);
	Line b(a2->position, b2->position);

	Vector2 intersection;
	if (a.intersection(b, intersection))
	{
		std::vector<Node*>::iterator i;
		std::vector<Node*>::iterator j;

		for (i = allNodes.begin(); i != allNodes.end(); i++)
		{

				Node *c = *i;

					if (c->isMotherOf(a1))
					{
						box(c, scale*1.2);
						layout_children(c);
						return true;
					}
		}

	}


	return false;
}
void AutoLayout::layout_children(Node* object)
{
	std::vector<Node*>::iterator j;
	for (j = object->children.begin(); j != object->children.end(); j++)
	{
		box(*j, 1);
		if((*j)->children.size() != 0)
		{
			layout_children(*j);
		}
	}
}

void AutoLayout::fillNodeGrid(std::vector<std::vector<Node*> >& nodeGrid, Node* node, unsigned int depth)
{
	// create vector if not present yet
	if (nodeGrid.size() <= depth)
	{
		nodeGrid.push_back(std::vector<Node*>());
	}
	
	// fill current node (if not present yet)
	
	if (std::find(nodeGrid[depth].begin(), nodeGrid[depth].end(), node) == nodeGrid[depth].end())
		nodeGrid[depth].push_back(node);
	
	depth++;
	
	//loop over daughters and fill those
	for (std::vector<Node*>::iterator child = node->children.begin(); child != node->children.end(); ++child)
	{
		fillNodeGrid(nodeGrid, *child, depth);
	}
	
	depth--;
}

/// Initial layout
void AutoLayout::new_initial()
{	
	std::vector<std::vector<Node*> > nodeGrid;
	
	// fill the node grid
	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		// start at all nodes without mothers and with daughters
		if ((*i)->mothers.size()==0 && (*i)->children.size() > 0 )
		{
			if ( ! ( (*i)->children.size() == 1 && (*i)->children[0]->children.size() == 0) )
				fillNodeGrid(nodeGrid, *i, 0);
		}
	}

	if (nodeGrid.size()==0)
		return;
	
	// find longest row
	size_t index_lr = 0;
	size_t maxSize=0;

	for (size_t n_x = 0; n_x < nodeGrid.size(); ++n_x)
	{
		size_t size = nodeGrid[n_x].size();
		if (size > maxSize)
		{
			maxSize = size;
			index_lr = n_x;
		}
	}
	
	grid = grid * (1/(1+sqrt(0.1*((double) maxSize))));
	
	// layout longest row
	for (unsigned int n_y = 0; n_y < nodeGrid[index_lr].size(); ++n_y)
	{
		Node* currentNode = nodeGrid[index_lr][n_y];
		currentNode->position.x = margin + 2. * margin * ((double) index_lr);
		// add safety margin according to size of sum of remaining daughter rows
		currentNode->position.y = grid * ((double) n_y + ((double) nodeGrid.size()-index_lr)/2. ) + border;
	}

	// layout mother rows, beginning at previous row
	for (size_t n_x = index_lr-1; n_x < nodeGrid.size(); --n_x)
	{
		double previous_y = -grid;
		for (unsigned int n_y = 0; n_y < nodeGrid[n_x].size(); ++n_y)
		{
			Node* currentNode = nodeGrid[n_x][n_y];
			currentNode->position.x = margin + 2. * margin * ((double) n_x);
			
			double mean_y_daughters = 0.;
			size_t n_mothers = 1;
			for (size_t n_child = 0; n_child < currentNode->children.size(); ++n_child)
			{
				mean_y_daughters += currentNode->children[n_child]->position.y;
				if (currentNode->children[n_child]->mothers.size()>1)
					n_mothers = currentNode->children[n_child]->mothers.size();
			}
			mean_y_daughters = mean_y_daughters / ((double) currentNode->children.size());
			
			//place mother at mean y position of all daughters, but at least one grid below previous one
			previous_y = std::max( std::max(0., previous_y + grid - 0.00001), mean_y_daughters + ((1. - (double) n_mothers)/2.)*grid ); // simple double-rounding protection
			
			currentNode->position.y = previous_y;
		}
	}

	// layout children rows, beginning at next row
	for (size_t n_x = index_lr+1; n_x < nodeGrid.size(); ++n_x)
	{
		double previous_y = -grid;
		for (unsigned int n_y = 0; n_y < nodeGrid[n_x].size(); ++n_y)
		{
			Node* currentNode = nodeGrid[n_x][n_y];
			currentNode->position.x = margin + 2. * margin * ((double) n_x);
			
			double mean_y_mothers = 0.;
			size_t n_sisters = 0;
			// normally, only one mother should be present
			for (size_t n_mother = 0; n_mother < currentNode->mothers.size(); ++n_mother)
			{
				mean_y_mothers += currentNode->mothers[n_mother]->position.y;
				n_sisters += currentNode->mothers[n_mother]->children.size();
			}
			if (currentNode->mothers.size() > 0)
				n_sisters = n_sisters/currentNode->mothers.size();
			if (currentNode->mothers.size() > 0)
				mean_y_mothers = mean_y_mothers / ((double) currentNode->mothers.size());
			
			//place 1st daughter 1/2 (or 1/n_chilren) grid position above mean y position of all mothers, but at least one grid below previous one
			previous_y = std::max( std::max(0., previous_y + grid - 0.00001), mean_y_mothers + ((1. -(double) n_sisters)/2.)*grid ); // simple double-rounding protection
			currentNode->position.y = previous_y;
		}
	}
		
/*
	for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
	{
		countChildren(*i);
		(*i)->boxednodes = 0;
	}

	for (unsigned int n = 0 ; n != 1; n++)
	{
		for (std::vector<Node*>::iterator i = allNodes.begin(); i != allNodes.end(); i++)
		{
			(*i)->active = true;
		}

		std::sort( allNodes.begin(), allNodes.end(), cmp2);


		layoutChildren(allNodes[0+n], 1.0);
	}
*/
}

void AutoLayout::drawbox()
{
}

void AutoLayout::layout_orphans()
{
	double bottom = border;
	std::vector<Node*>::iterator j;

	double max_y = -99999999999999.;
	
	for (j = allNodes.begin(); j != allNodes.end(); j++)
	{

/*		if ((*j)->position.y > bottom)
		{
			bottom = (*j)->position.y;
		}
*/
		if ((*j)->children.size() == 1 && (*j)->mothers.size() == 0)
		{
			if ((*j)->children[0]->children.size() == 0)
			{
				orphans.push_back(*j);
			}
		}
		else
		{
			if ((*j)->children.size() == 0 && (*j)->mothers.size() == 1)
				if ((*j)->mothers[0]->mothers.size()==0)
					continue;
			if ((*j)->position.y > max_y)
				max_y = (*j)->position.y;
		}
	}
	
	bottom += max_y;
	
	int columns = (int)std::ceil(std::sqrt(orphans.size()/2.0));
	int col = 0;
	std::vector<Node*>::iterator i;

	for (i = orphans.begin(); i != orphans.end(); i++)
	{
		(*i)->position.x = (margin + (col * 2 * margin));
		(*i)->position.y = (bottom);
		(*i)->children[0]->position.x = (margin + (col * 2 * margin)) + 70;
		(*i)->children[0]->position.y = (bottom);
		col ++;

		if (col == columns)
		{
			bottom += margin;
			col = 0;
		}
	}

}
// End new algorithm

void AutoLayout::activateChildren()
{

//	int n = 1, m = 1;
	std::vector<Node*> v;

 	std::vector<Node*>::iterator i;

	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		//v.push_back(*i);
		countChildren(*i);

	}
		for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		//Node* node2 = *i;
		//v.push_back(*i);
	}


	for (i = activeNodes.begin(); i != activeNodes.end(); i++)
	{
		Node* node = *i;
		double g = grid * 0.9;
		double y = -g/2.0;

		const double step = g /  node->children.size();

		if (node->children.size() == 0)
			y = 0.0;

		std::vector<Node*>::iterator j;


		for (j = node->children.begin(); j != node->children.end(); j++)
		{
			y += step;
			Node* child = *j;

			if (child->active == false)
			{
				child->active = true;

				v.push_back(*j);


			/*
				if (child->numberchildren > 3)
				{

					child->position = node->position + Vector2(grid, (50/m)*y*pow(-1,n));
						n++;
						m++;
//						std::cout<< countChildren(*j) << std::endl;

				}
				else
			*/
				child->position = node->position + Vector2(grid, y);



			}
		}

	}

	activeNodes.clear();
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{


		if ((*i)->active)
		activeNodes.push_back(*i);
	}


	_levels --;
}



void AutoLayout::layoutSteps()
{
	for (int i = 0; i < _steps; i++)
	{
		reset();
		resolveOverlaps();
		//resolveBackpointingChildren();
		applyGravity();
		applyProximityForces();
		applyFrictionForces();
		applySpringForces();
		applyInitialConstraints();
		step();

		if (isStable())
		{
			if (activeNodes.size() == allNodes.size())
			{
				//std::cout << "all nodes stable: " << allNodes.size() << ", activeNodes: " << activeNodes.size() << std::endl;
				break;
			}
			else
			{
				//std::cout << "all nodes activate: " << allNodes.size() << ", activeNodes: " << activeNodes.size() << std::endl;
				activateChildren();
				i = 0;
			}
		}
		else if (i == (_steps-1) && (activeNodes.size() != allNodes.size()) && (_levels >= 0))
		{
			i = 0;
			//std::cout << "all nodes: " << allNodes.size() << ", activeNodes: " << activeNodes.size() << std::endl;
			activateChildren();
		}
	}

}
void AutoLayout::layout(bool forceBased)
{
	if (forceBased)
	{
		placeInitials();
		layoutSteps();	
		layout_orphans();
	}
	else
	{
		new_initial();
		layout_orphans();
	}

}

/*
void AutoLayout::commitPositions()
{
	double offset = 1.0e32;

	std::vector<Node*>::iterator i;

	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		if ((*i)->position.y < offset)
			offset = (*i)->position.y;
	}

	double bottom = 0;
	for (i = allNodes.begin(); i != allNodes.end(); i++)
	{
		(*i)->commitPositions(margin - offset);
		double y = (*i)->position.y + margin - offset;
		if (y > bottom)
			bottom = y;
	}

	// layout orphans
	bottom += margin;
	int columns = (int)std::ceil(std::sqrt(orphans.size()/2.0));
	int col = 0;
	std::vector<Relative*>::iterator j;
	for (j = orphans.begin(); j != orphans.end(); j++)
	{
		pxl::Relative *object = *j;
		object->layout()->setA(margin + (col * 2 * margin));
		object->layout()->setB(bottom);
		object->layout()->setC(margin + (col * 2 * margin + margin));
		object->layout()->setD(bottom);

		col ++;

		if (col == columns)
		{
			bottom += margin;
			col = 0;
		}
	}
}
*/

} // namespace pxl
