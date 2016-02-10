//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include <iostream>
#include <memory>
#include <iomanip>
#include <cmath>

#include "pxl/core.hh"
#include "pxl/modules.hh"
#include "pxl/hep.hh"

//
//
//
//----------------------------------------------------------------------
/// This program provides code examples for how to use the PXL core libraries.
/// Open the source code to see an illustration of how the basic classes are used.
/// The example demonstrates how to access data and
/// establish mother/daughter relations with other objects. The event container
/// pxl::EventView with physics objects such as pxl::Particle, pxl::Vertex
/// and pxl::Collision is introduced and various code examples are given.



static pxl::Logger logger("pxl::Example");

namespace pxl
{

class Example
{
public:
	void showPhysicsObjects();
	void showUserRecords();
	void showRelations();
	void showObjectOwner();
	void showWeakPointers();
	void showInputOutput();
	void showAdditionalFeatures();
	void showTechnicalDetails();
	void showMatrix();
};
	
	
}


class TestParticle
{
public:
	double x;
	double y;
	double z;
	double t;

	void add(double d)
	{
		x += d;
		y += d;
		z += d;
		t += d;
	}
};

namespace pxl
{
template<>
void ParticleHelper<TestParticle>::transferBack()
{
	particleRef->setP4(object->x, object->y, object->z, object->t);
}

template<>
void ParticleHelper<TestParticle>::setObjectData()
{
	object->x = particleRef->getPx();
	object->y = particleRef->getPy();
	object->z = particleRef->getPz();
	object->t = particleRef->getE();
}
}






namespace pxl
{

void Example::showPhysicsObjects()
{
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 1) Demonstration of Physics Objects ********");

	{
		logger(LOG_LEVEL_INFO, "a) pxl::Vertex" );
		// A simple physics object is the vertex:
		pxl::Vertex vertex1;
		// We can print the standard information with the print method.
		vertex1.print();
		// This is boring, so we set the name and the components of the vertex.
		vertex1.setName("my vertex 1");
		vertex1.setX(4.);
		vertex1.setY(1.);
		vertex1.setZ(0.4);
		// Now the printout is more exciting:
		vertex1.print();
		// We create a second vertex
		pxl::Vertex vertex2;
		vertex2.setName("my vertex 2");
		vertex2.setX(-1.);
		vertex2.setY(3.);
		vertex2.setZ(-9.8);
		vertex2.print();
		// Now we can do more complicated things.
		vertex2 += vertex1;
		vertex2.print();
		// Or, if we have pointers, we can use the addVertex-function.
		pxl::Vertex* vertex1Pointer = &vertex1;
		pxl::Vertex* vertex2Pointer = &vertex2;
		vertex1Pointer->addVertex(vertex2Pointer);
		vertex1.print();
		// More advanced arithmetics with the underlying three vector can be performed by accessing
		// the vector directly.
		logger(LOG_LEVEL_INFO, "Cosine theta of vector 1: "
				,  vertex1.getVector().getCosTheta() );
	}
	

	{
		logger(LOG_LEVEL_INFO, "b) pxl::Particle" );
		pxl::Particle* ptl1 = new pxl::Particle();
		ptl1->print();
		// Set some particle properties.
		// To avoid ambiguities between different representations of fourvectors,
		// you can only set all fourvector components at once.
		// The pxl::Particle uses the (px, py, pz, E)-representation.
		ptl1->setP4(5., 4., 7., 30.);
		ptl1->setCharge(1. / 3.);
		ptl1->setName("b quark");
		ptl1->setPdgNumber(5);
		logger(LOG_LEVEL_INFO, ptl1->print() ,  " charge " ,  ptl1->getCharge()
				,  ", ID " ,  ptl1->getPdgNumber() );
		ptl1->getAs<TestParticle> ()->set()->add(4.);
		logger(LOG_LEVEL_INFO, ptl1->print() );
		delete ptl1; //remember to delete objects created with new
	}
}


void Example::showUserRecords()
{		

	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 2) Demonstration of the UserRecord ********");

	{
		/* Each physics object in PXL has a UserRecord, consisting
		 * of (string, type)-pairs, where type is any basic C++ type,
		 * plus the std::string.
		 */
		pxl::Particle* ptl1 = new pxl::Particle();
		// set a double value
		ptl1->setName("muon");
		ptl1->setUserRecord ("calorimeter isolation", 0.4);
		ptl1->setUserRecord ("number of hits", 4);
		ptl1->setUserRecord ("quality", "good");

		// Print all UserRecord entries.
		ptl1->print();
		// Or print them directly.
		ptl1->getUserRecords().print();

		// Get UserRecord entry, providing no default value. Attention: In case
		// the entry is not present, an exception is thrown.
		logger(LOG_LEVEL_INFO, "Number of hits: " ,  ptl1->getUserRecord (
				"number of hits") );

		// Get UserRecord entry, providing a default value.
		logger(LOG_LEVEL_INFO, "Quality: " ,  ptl1->getUserRecord (
				"quality") );
		// Remove a certain UserRecord entry.
		ptl1->eraseUserRecord("quality");
		//logger(LOG_LEVEL_INFO, "Quality: " ,  ptl1->findUserRecord<std::string> (
		//		"quality", "default") );

		// To check if a UserRecord is present, use one of the following methods. The
		// second one directly fills the according value into the passed variable if present.
		logger(LOG_LEVEL_INFO, "Is an entry named 'quality' present? "
				,  ptl1->hasUserRecord("quality") );

		logger(LOG_LEVEL_INFO, "Is an entry named 'number of hits' present? "
				,  ptl1->hasUserRecord("number of hits") );
		double isolation = 0.;
		if (ptl1->getUserRecord ("calorimeter isolation", isolation))
			logger(LOG_LEVEL_INFO, "Isolation variable present, value " ,  isolation);
		else
			logger(LOG_LEVEL_INFO, "Isolation variable not present." );

		// Looping over the UserRecord is also possible. However, subtleties
		// due to the underlying pxl::Variant are involved.
		for (pxl::UserRecords::const_iterator iter =
				ptl1->getUserRecords().begin(); iter
				!= ptl1->getUserRecords().end(); ++iter)
		{
			logger(LOG_LEVEL_INFO, "Key: " ,  iter->first ,  ", value has type "
					,  iter->second.getTypeName() );
			if (iter->second.getType() == pxl::Variant::TYPE_DOUBLE)
				logger(LOG_LEVEL_INFO, "A UserRecord entry of type double is present, ("
						,  iter->first ,  ", " ,  iter->second.asDouble ()
						,  ")." );
			if (strcmp(iter->second.getTypeName(), "int") == 0)
				logger(LOG_LEVEL_INFO, "A UserRecord entry with type name int is present, ("
						,  iter->first ,  ", " ,  iter->second.asInt32 ()
						,  ")." );
		}

		// Finally, the UserRecord can also be cleared.
		logger(LOG_LEVEL_INFO, "Size of UserRecord: " ,  ptl1->getUserRecords().size());
		ptl1->getUserRecords().clear();
		logger(LOG_LEVEL_INFO, "Size of UserRecord: " ,  ptl1->getUserRecords().size());

		delete ptl1;
	}
}

void Example::showRelations()
{
	
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 3) Demonstration of the Relation Management ********");
	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "a) Mother, Daughter, Flat Relations " );
		logger(LOG_LEVEL_INFO, "");

		pxl::Particle* p1 = new pxl::Particle();
		pxl::Particle* p2 = new pxl::Particle();
		p1->setP4(10., 10., 14.1, 20.);
		p2->setP4(-10., -12., 0., 16.);

		pxl::Particle* m1 = new pxl::Particle();
		pxl::Particle* m2 = new pxl::Particle();
		pxl::Vertex* v1 = new pxl::Vertex();
		pxl::Collision* c1 = new pxl::Collision();

		logger(LOG_LEVEL_INFO, "Link two daughters two one mother." );
		p1->linkMother(m1);
		p2->linkMother(m1);
		m1->printDecayTree();
		logger(LOG_LEVEL_INFO, "Relate to other pxl::Relative derivatives." );
		m1->linkMother(v1);
		v1->linkMother(c1);
		c1->printDecayTree();
		logger(LOG_LEVEL_INFO, "Unlink and print decay tree." );
		c1->unlinkDaughter(v1);
		c1->printDecayTree();

		logger(LOG_LEVEL_INFO, "Set kinematics from daughters." );
		m1->setP4FromDaughters();
		m1->printDecayTree();

		const pxl::Relations& daughterRel = m1->getDaughterRelations();
		logger(LOG_LEVEL_INFO, "Loop over daughter relations of size "
				,  daughterRel.size() ,  "." );

		for (pxl::Relations::const_iterator iter = daughterRel.begin(); iter
				!= daughterRel.end(); ++iter)
		{
			pxl::Relative* daughter = *iter;
			daughter->print();
		}

		logger(LOG_LEVEL_INFO, "Is v1 mother of m1? " ,  v1->getDaughterRelations().has(
				m1) );
		logger(LOG_LEVEL_INFO, "Is v1 mother of c1? " ,  v1->getDaughterRelations().has(
				c1) );

		logger(LOG_LEVEL_INFO, "Get all related pxl::Particles." );
		std::vector<pxl::Particle*> particles;
		int nParticleDaughters = daughterRel.getObjectsOfType<pxl::Particle> (
				particles);
		logger(LOG_LEVEL_INFO, "There are " ,  nParticleDaughters
				,  " daughters of type pxl::Particle." );

		logger(LOG_LEVEL_INFO, "Create and iterate flat relations." );
		m1->linkFlat(m2);
		for (pxl::Relations::const_iterator iter =
				m1->getFlatRelations().begin(); iter
				!= m1->getFlatRelations().end(); ++iter)
		{
			(*iter)->print();
		}

		delete p1;
		delete p2;
		delete m1;
		delete m2;
		delete v1;
		delete c1;
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "b) Soft Relations " );
		logger(LOG_LEVEL_INFO, "");

		pxl::Particle* p1 = new pxl::Particle();
		pxl::Particle* p2 = new pxl::Particle();

		pxl::Vertex* v1 = new pxl::Vertex();
		pxl::Collision* c1 = new pxl::Collision();

		logger(LOG_LEVEL_INFO, "Link a particle p1 to several other objects.");
		p1->linkSoft(p2, "gen-reco");
		p1->linkSoft(v1, "particle-vertex");
		p1->linkSoft(c1, "particle-collision");

		logger(LOG_LEVEL_INFO, "Loop soft relations and look at content." );
		for (pxl::SoftRelations::const_iterator iter =
				p1->getSoftRelations().begin(); iter
				!= p1->getSoftRelations().end(); ++iter)
		{
			logger(LOG_LEVEL_INFO, "Soft Relation Name/UUID: ('" ,  iter->first ,  "', "
					,  iter->second ,  "). " );
		}

		logger(LOG_LEVEL_INFO, "Several checks if soft relations exist." );

		if (p1->getSoftRelations().has(p2))
			logger(LOG_LEVEL_INFO, "Soft relation to p2 exists." );
		if (p1->getSoftRelations().hasType("particle-collision"))
			logger(LOG_LEVEL_INFO, "Soft relation named 'particle-collision' exists.");
		if (p2->getSoftRelations().has(p1, "gen-reco"))
			logger(LOG_LEVEL_INFO, "p2 has a soft relation named 'gen-reco' to p1.");
		logger(LOG_LEVEL_INFO, "p1 has " ,  p1->getSoftRelations().count("gen-reco")
				,  " soft relation(s) of type 'gen-reco'. " );

		logger(LOG_LEVEL_INFO,  "To actually get the related objects, an object container needs to be provided. This will be detailed later.");

		delete p1;
		delete p2;
		delete v1;
		delete c1;
	}

}

void Example::showObjectOwner()
{
	
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 4) Demonstration of the object owner: the Event and the EventView ********");

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "a) The EventView " );
		logger(LOG_LEVEL_INFO, "");

		pxl::EventView* ev = new pxl::EventView();
		logger(LOG_LEVEL_INFO, "Set basic properties of the EventView, e.g. name, UserRecord.");
		ev->setName("reconstructed");
		ev->setUserRecord ("hypothesis nr", 4);

		logger(LOG_LEVEL_INFO, "Create Relative derivatives, e.g. particles and vertices.");
		pxl::Particle* p1 = ev->create<pxl::Particle> ();
		pxl::Particle* p2 = ev->create<pxl::Particle> ();
		pxl::Vertex* v1 = ev->create<pxl::Vertex> ();
		logger(LOG_LEVEL_INFO, "EventView contains (via the ObjectOwner) "
				,  ev->getObjectOwner().size() ,  " objects. " );
		logger(LOG_LEVEL_INFO, "Does p1 belong to this EventView's ObjectOwner? "
				,  ev->getObjectOwner().has(p1) );
		ev->print(1);
		logger(LOG_LEVEL_INFO, "These belong to the ObjectOwner (contained in the EventView).");
		logger(LOG_LEVEL_INFO, "The ObjectOwner has the deletion responsibility.");
		logger(LOG_LEVEL_INFO, "Relations are safe due to the ObjectOwner." );
		p1->linkMother(v1);
		p2->linkMother(v1);
		ev->print();

		logger(LOG_LEVEL_INFO, "Remove an object." );
		ev->removeObject(p1);
		ev->print();

		logger(LOG_LEVEL_INFO, "Loop over the contained objects via the ObjectOwner.");
		for (pxl::ObjectOwner::const_iterator iter =
				ev->getObjectOwner().begin(); iter
				!= ev->getObjectOwner().end(); ++iter)
		{
			pxl::Relative* containedObject = *iter;
			containedObject->print();
		}

		logger(LOG_LEVEL_INFO, "Get particles from the ObjectOwner and iterate.");
		std::vector<pxl::Particle*> particles;
		ev->getObjectsOfType<pxl::Particle> (particles);
		for (std::vector<pxl::Particle*>::const_iterator pIt =
				particles.begin(); pIt != particles.end(); ++pIt)
		{
			pxl::Particle* containedPart = *pIt;
			containedPart->print();
		}

		logger(LOG_LEVEL_INFO, "Alternatively use the TypeIterator." );
		for (pxl::ObjectOwnerTypeIterator<pxl::Particle> it =
				ev->getObjectOwner().begin<pxl::Particle> (); it
				!= ev->getObjectOwner().end(); ++it)
		{
			logger(LOG_LEVEL_INFO, "Particle with name " ,  (*it)->getName() );
		}

		logger(LOG_LEVEL_INFO, "For fast access, an index can be set." );
		ev->setIndexEntry("muon", p2);
		ev->setIndexEntry("production vertex", v1);

		pxl::Particle* ele = ev->findObject<pxl::Particle> ("electron");
		if (ele)
			logger(LOG_LEVEL_INFO, "Found an electron." );
		pxl::Particle* muon = ev->findObject<pxl::Particle> ("muon");
		if (muon)
		{
			logger(LOG_LEVEL_INFO, "Found a muon." );
			muon->print();
		}

		logger(LOG_LEVEL_INFO, "Relations are copied..." );
		pxl::EventView* copyEv = new pxl::EventView(ev);
		copyEv->setName("Copied EventView");
		copyEv->print();
		logger(LOG_LEVEL_INFO, "... as well as the index." );
		pxl::Vertex* prodVertex = ev->findObject<pxl::Vertex> (
				"production vertex");
		if (prodVertex)
			logger(LOG_LEVEL_INFO, "Production vertex found." );

		logger(LOG_LEVEL_INFO, "The CopyHistory can find the new vertex from the original vertex.");
		pxl::Vertex* copyVertex = copyEv->findCopyOf<pxl::Vertex> (v1);
		if (copyVertex)
			logger(LOG_LEVEL_INFO, "Copied vertex was found." );

		delete copyEv;
		delete ev;
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "b) The Event" );
		logger(LOG_LEVEL_INFO, "");

		pxl::Event* event = new pxl::Event();

		logger(LOG_LEVEL_INFO, "The pxl::Event also has an object owner." );
		logger(LOG_LEVEL_INFO, "Create two EventViews, directly make an index entry.");

		pxl::EventView* ev1 =
				event->createIndexed<pxl::EventView> ("generated");
		pxl::EventView* ev2 = event->createIndexed<pxl::EventView> (
				"reconstructed");

		ev1->linkFlat(ev2);
		event->print();

		logger(LOG_LEVEL_INFO, "The event is the basic physical entity, so it has no name, workflag or lock flag.");
		logger(LOG_LEVEL_INFO, "Put information into the UserRecord." );
		event->setUserRecord ("Name", "2 Jet 1 Muon Event");

		logger(LOG_LEVEL_INFO, "Create some objects within the EventViews." );
		pxl::Particle* p1 = ev1->create<pxl::Particle> ();
		pxl::Particle* p2 = ev1->create<pxl::Particle> ();
		pxl::Particle* m1 = ev1->create<pxl::Particle> ();
		p1->linkMother(m1);
		p2->linkMother(m1);
		ev2->create<pxl::Particle> ()->setName("Jet");
		ev2->create<pxl::Particle> ()->setName("Jet");
		ev2->create<pxl::Particle> ()->setName("Muon");

		logger(LOG_LEVEL_INFO, "** Print complete event. **" );
		event->print();
		logger(LOG_LEVEL_INFO, "** End print complete event. **" );

		delete event;
	}

}

void Example::showWeakPointers()
{
	
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 5) Weak pointers, Pointers ********" );

	{
		logger(LOG_LEVEL_INFO, "As an additional example, use std::auto_ptr for the first object created.");
		std::auto_ptr<pxl::EventView> ev(new pxl::EventView());

		logger(LOG_LEVEL_INFO, "We can implicitly convert a returned pointer to a pxl::weak_ptr. ");
		pxl::weak_ptr<pxl::Particle> partWkPtr = ev->create<pxl::Particle> ();

		logger(LOG_LEVEL_INFO, "Access with the -> operator..." );
		partWkPtr->setName("jet");

		logger(LOG_LEVEL_INFO, "... or with the ptr() method..." );
		partWkPtr.ptr()->setP4(10., 10., 10., 30.);

		logger(LOG_LEVEL_INFO, "... or with the object() method..." );

		partWkPtr.object().print();

		logger(LOG_LEVEL_INFO, "An exception is thrown in case the object was deleted. To prevent this, check if the weak_ptr is valid.");

		logger(LOG_LEVEL_INFO, "Remove the object." );
		ev->removeObject(partWkPtr);

		logger(LOG_LEVEL_INFO, "Check if the weak pointer is still valid." );
		if (partWkPtr) // Note the implicit conversion to a pointer.
			logger(LOG_LEVEL_INFO, "wk_ptr still valid." );
		else
			logger(LOG_LEVEL_INFO, "wk_ptr not valid!" );
		logger(LOG_LEVEL_INFO, "Alternative syntax. Valid? " ,  partWkPtr.valid());

		logger(LOG_LEVEL_INFO, "This is not possible for a normal pointer - this would be a hard-to-trace bug.");
	}
}

void Example::showInputOutput()
{
	

	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 6) I/O ********" );

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "a) Writing a file which contains pxl::Events");
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "First create a new pxl::OutputFile object with the filename as constructor argument. ");
		pxl::OutputFile outFile("pxlexample_1.pxlio");
		for (int i = 0; i < 100; ++i)
		{
			std::auto_ptr<pxl::Event> newEvent(new pxl::Event());

			pxl::weak_ptr<pxl::EventView> evtView = newEvent->create<pxl::EventView> ();

			pxl::weak_ptr<pxl::Particle> electron1 = evtView->create<pxl::Particle> ();
			electron1->setP4(40., -20., 31., 80.);
			electron1->setName("electron");
			electron1->setUserRecord ("calo isolation", 0.6);
			if (i == 0)
				electron1->print();
			pxl::weak_ptr<pxl::Particle> electron2 = evtView->create<pxl::Particle> ();
			electron2->setP4(19., 41., 10., 70.);
			electron2->setName("electron");
			electron2->setUserRecord ("calo isolation", 0.3);
			if (i == 0)
				electron2->print();
			pxl::weak_ptr<pxl::Particle> zboson = evtView->create<pxl::Particle> ();
			zboson->setName("Z");
			zboson->linkDaughter(electron1);
			zboson->linkDaughter(electron2);
			zboson->setP4FromDaughters();
			if (i == 0)
				zboson->print();
			electron1->linkSoft(electron2, "we are both electrons");
			if (i == 0)
			{
				logger(LOG_LEVEL_INFO, "Meanwhile: Demonstrate how to access Soft Relations with ObjectOwner.");
				pxl::Relative
						* related =
								(pxl::Relative *) electron2->getSoftRelations().getFirst(
										evtView->getObjectOwner());
				if (related)
					logger(LOG_LEVEL_INFO, "Name of related relative: "
							,  related->getName() );
			}
			if (i == 0)
				logger(LOG_LEVEL_INFO, "For each event, use the streamObject method to write the complete physics event to disk.");
			outFile.streamObject(newEvent.get());
		}
		outFile.close();
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "b) Reading the file containing pxl::Events" );
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "Book an input file.");

		pxl::InputFile inputFile("pxlexample_1.pxlio");

		logger(LOG_LEVEL_INFO, "Size of file: " ,  inputFile.getSize() );
		logger(LOG_LEVEL_INFO, "Current position: " ,  inputFile.getPosition());

		int nEvents = 0;

		while (inputFile.good())
		{
			pxl::Event* event=dynamic_cast<pxl::Event*>(inputFile.readNextObject());
			if(!event) continue;
			nEvents=inputFile.objectCount();

			if (nEvents == 1)
			{
				event->print();
				logger(LOG_LEVEL_INFO, "Get the contained EventViews, and then the Particles contained therein.");
			}
			std::vector<pxl::EventView*> eventViews;
			event->getObjectsOfType<pxl::EventView> (eventViews);

			for (std::vector<pxl::EventView*>::const_iterator it =
				eventViews.begin(); it != eventViews.end(); ++it)
			{
				std::vector<pxl::Particle*> particles;
				pxl::EventView* evView = *it;
				evView->getObjectsOfType<pxl::Particle> (particles);
				for (std::vector<pxl::Particle*>::const_iterator pIt =
					particles.begin(); pIt != particles.end(); ++pIt)
				{
					pxl::Particle* zboson = *pIt;
					if (zboson->getMotherRelations().size() == 0 && nEvents
							== 1)
						zboson->print();
				}
			}
			delete event;
		}
		inputFile.close();
		std::remove( "pxlexample_1.pxlio" );
		logger(LOG_LEVEL_INFO, "Processed " ,  nEvents ,  " events. " );
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "c) Writing a file which contains arbitrary objects.");
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "Create a new pxl::OutputFile. " );
		pxl::OutputFile outFile("pxlexample_2.pxlio");
		for (int i = 0; i < 100; ++i)
		{
			if (i == 0)
				logger(LOG_LEVEL_INFO, "For each object, use the streamObject method to add it to the output stream.");

			pxl::Particle* electron(new pxl::Particle);
			electron->setP4(50., 30., 40., 71.);

			pxl::Vertex* vertex(new pxl::Vertex);
			vertex->setX(0.3);
			vertex->setY(0.8);
			vertex->setZ(-3.8);

			pxl::EventView* ev(new pxl::EventView);
			ev->setUserRecord ("likelihood", 0.7);

			outFile.streamObject(electron);
			outFile.streamObject(vertex);
			outFile.streamObject(ev);

			delete electron;
			delete vertex;
			delete ev;
		}
		outFile.close();
	}

	{
		logger(LOG_LEVEL_INFO, "d) Reading a file which contains arbitrary objects");
		pxl::InputFile inFile("pxlexample_2.pxlio");

		logger(LOG_LEVEL_INFO, "File good? " ,  inFile.good() );

		pxl::Serializable* obj = inFile.readNextObject();
		if (obj)
			logger(LOG_LEVEL_INFO, obj->getId() ,  ", " ,  obj->getTypeId() );

		pxl::Relative* relP = dynamic_cast<pxl::Relative*> (obj);
		if (relP)
			logger(LOG_LEVEL_INFO, relP->getId() ,  ", " ,  relP->getTypeId());
		if (relP)
			relP->print();

		logger(LOG_LEVEL_INFO, "The user takes deletion responsibility when using readNextObject(), seekToObject or readPreviousObject!");

		delete obj;

		obj = inFile.seekToObject(20);
		if (obj)
			logger(LOG_LEVEL_INFO, obj->getId() ,  ", " ,  obj->getTypeId() );

		relP = dynamic_cast<pxl::Relative*> (obj);
		if (relP)
			relP->print();

		delete obj;

		obj = inFile.seekToObject(20);
		if (obj)
			logger(LOG_LEVEL_INFO, obj->getId() ,  ", " ,  obj->getTypeId() );

		relP = dynamic_cast<pxl::Relative*> (obj);
		if (relP)
			relP->print();

		delete obj;

		obj = inFile.readPreviousObject();
		logger(LOG_LEVEL_INFO, obj->getId() ,  ", " ,  obj->getTypeId() );

		relP = dynamic_cast<pxl::Relative*> (obj);
		if (relP)
			relP->print();

		delete obj;
		std::remove("pxlexample_2.pxlio");
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "e) Writing a file with InformationChunks.");
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "Create a new pxl::OutputFile. " );
		pxl::OutputFile outFile("pxlexample_3.pxlio");

		pxl::InformationChunk* info(new pxl::InformationChunk);

		info->setName("W+Jets");
		info->setUserRecord ("pt Cut", 70.);

		logger(LOG_LEVEL_INFO, "Write InformationChunk and finish the filesection, so it can be quickly read in." );
		outFile.streamObject(info);
		outFile.writeFileSection();

		delete info;

		// event loop
		for (int i = 0; i < 100; ++i)
		{
			pxl::Event event1;
			pxl::Event event2;
			event1.setUserRecord ("likelihood", 0.7);
			event2.setUserRecord("likelihood", 0.3);
			outFile.streamObject(&event1);
			outFile.streamObject(&event2);
		}
		outFile.close();
	}

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "f) Reading a file with InformationChunks.");
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "Create a new pxl::InputFile. " );
		pxl::InputFile inFile("pxlexample_3.pxlio");

		logger(LOG_LEVEL_INFO, "Get the information chunk and print restored UserRecord.");

		pxl::InformationChunk* info=dynamic_cast<pxl::InformationChunk*>(inFile.readNextObject());
		info->getUserRecords().print();
		delete info;

		logger(LOG_LEVEL_INFO, "Read the rest of the file." );

		inFile.close();
		std::remove("pxlexample_3.pxlio");
	}

}

void Example::showAdditionalFeatures()
{
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 7) Additional Functionality ********" );

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "a) Filters and Sorting" );
		logger(LOG_LEVEL_INFO, "");

		logger(LOG_LEVEL_INFO, "Create EventView with three particles." );
		pxl::EventView* ev = new pxl::EventView();

		pxl::Particle* gluon = ev->createIndexed<pxl::Particle> ("gluon");
		gluon->setName("gluon");
		pxl::Particle* b = ev->createIndexed<pxl::Particle> ("b");
		b->setName("b");
		b->setP4(20., 25., 0., 35.);
		pxl::Particle* bbar = ev->createIndexed<pxl::Particle> ("bbar");
		bbar->setP4(100., 100., 0., 142.);
		bbar->setName("bbar");

		logger(LOG_LEVEL_INFO, "Filter & pt-sort particles." );
		pxl::ParticleFilter filter;
		std::vector<pxl::Particle*> particles;

		logger(LOG_LEVEL_INFO, "Choose a pt, eta, name filter criterion" );
		pxl::ParticlePtEtaNameCriterion crit("gluon", 0., 0.);
		logger(LOG_LEVEL_INFO, "Apply filter. " );
		filter.apply(ev->getObjectOwner(), particles, crit);

		logger(LOG_LEVEL_INFO, "Print, sorted by pT: " );
		for (std::vector<pxl::Particle*>::const_iterator iter =
				particles.begin(); iter != particles.end(); ++iter)
		{
			pxl::weak_ptr<pxl::Particle> pa = *iter;
			logger(LOG_LEVEL_INFO, " " ,  pa->getName());
			logger(LOG_LEVEL_INFO, ", pT = " ,  pa->getPt());
			logger(LOG_LEVEL_INFO, "");
		}

		particles.clear();

		logger(LOG_LEVEL_INFO, "Take a pt filter criterion only. " );
		pxl::ParticlePtCriterion ptCrit(50.);
		filter.apply(ev->getObjectOwner(), particles, ptCrit);

		logger(LOG_LEVEL_INFO, "Print all objects with pt bigger than 50, sorted by pT: ");
		for (std::vector<pxl::Particle*>::const_iterator iter =
				particles.begin(); iter != particles.end(); ++iter)
		{
			pxl::weak_ptr<pxl::Particle> pa = *iter;
			logger(LOG_LEVEL_INFO, " " ,  pa->getName());
			logger(LOG_LEVEL_INFO, ", pT = " ,  pa->getPt());
			logger(LOG_LEVEL_INFO, "");
		}

		delete ev;

	}
}

void Example::showTechnicalDetails()
{
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 8) Technical Details ********" );

	{
		logger(LOG_LEVEL_INFO, "");
		logger(LOG_LEVEL_INFO, "a) UserRecord Copy-On-Write" );
		logger(LOG_LEVEL_INFO, "");

		pxl::EventView* ev = new pxl::EventView();
		ev->setUserRecord("run nr.", 192168);

		logger(LOG_LEVEL_INFO, "Display UserRecord entry: " ,  ev->getUserRecord (
				"run nr.") );

		logger(LOG_LEVEL_INFO, "Map of original UserRecord resides at "
				,  ev->getUserRecords().getContainer() );

		pxl::EventView* ev_copy = new pxl::EventView(ev);
		logger(LOG_LEVEL_INFO, "Map of copied event's UserRecord resides at "
				,  ev_copy->getUserRecords().getContainer() );

		ev_copy->setUserRecord("run nr.", 1921124);
		logger(LOG_LEVEL_INFO, "Display modified UserRecord entry: "
				,  ev_copy->getUserRecord("run nr.") );
		logger(LOG_LEVEL_INFO, "Map of copied event's modified UserRecord resides at "
				,  ev_copy->getUserRecords().getContainer() );

		logger(LOG_LEVEL_INFO, "Display original UserRecord entry: "
				,  ev->getUserRecord ("run nr.") );

		delete ev;
		delete ev_copy;
	}
}

void Example::showMatrix()
{
	logger(LOG_LEVEL_INFO, "");
	logger(LOG_LEVEL_INFO, "******** 9) Matrix Example ********" );
	// Set up N Matrix and N vector
	int N =9;
	BasicNVector x(N);
	BasicNVector y(N);
	BasicMatrix A(N,N);

	std::cout << "     *** Matrix multiplication" << std::endl;
	std::cout << std::endl;
	for (int i=0; i<N;i++)
	{
		x(i) =i+1;
		A(i,i) = i+1;
	}
	A(0,2) = 2;
	//math
	y = A*x;


	for (int i=0; i<N;i++)
	{
		if (i == N/2)
			std::cout << "| " << std::setw(3) << y(i) << "  | " << "= | ";
		else
			std::cout << "| "  << std::setw(3)<< y(i) << "  | " << "  | ";
		for (int j=0;j<N;j++)
		{
			std::cout  << std::setw(3)<< A(i,j) << " ";
		}
		std::cout << " | ";

		if (i == N/2)
			std::cout << "* | "  << std::setw(3)<< x(i) << "  |" << std::endl;
		else
			std::cout << "  | "  << std::setw(3)<< x(i) << "  |" << std::endl;
	}


	std::cout << std::endl;
	std::cout << "     *** Rotation Matrix" << std::endl;
	std::cout << std::endl;
	Basic3Vector v; 
	v.setZ(1);
	std::cout << " Rotation Axis v=(" ;
	for (int i=0; i<3;i++)
		std::cout << v.getElement(i) << " ";
	std::cout << " )" << std::endl;
	std::cout << "Rotation Matrix, Rotation angle 45deg" << std::endl;

	std::cout.setf(std::ios::fixed); //,std::ios::floatfield);
	std::cout.precision(3);
	std::cout << std::setiosflags( std::ios::right);

	// math
	RotationMatrix R(v,45./180*M_PI);
	Basic3Vector w;
	Basic3Vector u;
	u.setX(1);
	w = R*u;

	// output
	for (int i=0; i<3;i++)
	{
		if (i == 1)
			std::cout << "|" <<std::setw(8) <<  w.getElement(i) << " | " << "= |";
		else
			std::cout << "|"  <<std::setw(8)<< w.getElement(i) << " | " << "  |";
		std::cout << " | ";
		for (int j=0;j<3;j++)
		{
			std::cout  << std::setw(8)<< R(i,j) << " ";
		}
		std::cout << " | ";

		if (i == 1)
			std::cout << "* |" << std::setw(8) << u.getElement(i) << " |" << std::endl;
		else
			std::cout << "  |" << std::setw(8) << u.getElement(i) << " |" << std::endl;
	}

	// math
	std::cout << " once again ..." << std::endl;
	u = w;
	w = R*u;

	// Output
	for (int i=0; i<3;i++)
	{
		if (i == 1)
			std::cout << "|" <<std::setw(8) <<  w.getElement(i) << " | " << "= |";
		else
			std::cout << "|"  <<std::setw(8)<< w.getElement(i) << " | " << "  |";
		std::cout << " | ";
		for (int j=0;j<3;j++)
		{
			std::cout  << std::setw(8)<< R(i,j) << " ";
		}
		std::cout << " | ";

		if (i == 1)
			std::cout << "* |" << std::setw(8) << u.getElement(i) << " |" << std::endl;
		else
			std::cout << "  |" << std::setw(8) << u.getElement(i) << " |" << std::endl;
	}
	std::cout << std::endl;
}

} // namespace pxl

int main()
{
	pxl::Example example;

	pxl::LogDispatcher::instance().enableConsoleLogHandler(pxl::LOG_LEVEL_INFO);


	logger(pxl::LOG_LEVEL_INFO, " " );
	logger(pxl::LOG_LEVEL_INFO, "   **********************************************************");
	logger(pxl::LOG_LEVEL_INFO, "   * This is the PXL example.                               *");
	logger(pxl::LOG_LEVEL_INFO, "   **********************************************************");
	logger(pxl::LOG_LEVEL_INFO, "   *                                                        *");
	logger(pxl::LOG_LEVEL_INFO, "   *                                                        *");
	logger(pxl::LOG_LEVEL_INFO, "   * January 2010                                           *");
	logger(pxl::LOG_LEVEL_INFO, "   **********************************************************");
	logger(pxl::LOG_LEVEL_INFO, " " );

	/*
	 * Structure according to the manual:
	 * 1) Physics Objects
	 * 2) User Record
	 * 3) Relation Management
	 * 4) Object Owner
	 * 5) Pointer and Weak Pointer
	 * 6) I/O
	 * 7) Additional Functionality
	 * 8) Technical Details
	 * 9) Matrix Examples
	 */

	example.showPhysicsObjects();
	example.showUserRecords(); 
	example.showRelations();
	example.showObjectOwner();
	example.showWeakPointers();
	//example.showInputOutput();
	example.showAdditionalFeatures();
	example.showTechnicalDetails();
	example.showMatrix();

	logger(pxl::LOG_LEVEL_INFO, "");
	logger(pxl::LOG_LEVEL_INFO, "******** End of Example ********" );
	logger(pxl::LOG_LEVEL_INFO, "");

	return 0;

}
