//-------------------------------------------
// Project: Physics eXtension Library (PXL) -
//      http://vispa.physik.rwth-aachen.de/ -
// Copyright (C) 2009-2012 Martin Erdmann   -
//               RWTH Aachen, Germany       -
// Licensed under a LGPL-2 or later license -
//-------------------------------------------

#include <iostream>
#include <memory>

#include "pxl/core.hh"
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


static pxl::Logger logger("pxl::GeneralTest");

namespace pxl
{

class GeneralTest
{
public:
	GeneralTest() :
		_epsilon(0.0000000001)
	{
	}

	int testPhysicsObjects();
	int testUserRecords();
	int testRelations();
	int testObjectOwner();
	int testWeakPointers();
	int testInputOutput();
	int testAdditionalFeatures();
	int testTechnicalDetails();
	int testSerializableInUserrecords();
	int testVariant();
private:
	double _epsilon;
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

int GeneralTest::testPhysicsObjects()
{
	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG, "******** 1) Test of Physics Objects ********");

	{
		logger(LOG_LEVEL_DEBUG, "a) pxl::Vertex");

		pxl::Vertex vertex1;

		vertex1.setName("my vertex 1");
		vertex1.setX(4.);
		vertex1.setY(1.);
		vertex1.setZ(-0.4);

		if (vertex1.getX() != 4. || vertex1.getY() != 1. || vertex1.getZ()
				!= -0.4)
		{
			logger(LOG_LEVEL_ERROR, "Component of vertex not correctly set");
			return 1;
		}

		// We create a second vertex
		pxl::Vertex vertex2;
		vertex2.setName("my vertex 2");
		vertex2.setX(-1.);
		vertex2.setY(3.);
		vertex2.setZ(-9.8);

		// Now we can do more complicated things.
		vertex2 += vertex1;

		if (std::abs(vertex2.getX() - 3.) > _epsilon || std::abs(vertex2.getY()
				- 4) > _epsilon || std::abs(vertex2.getZ() + 10.2) > _epsilon)
		{
			logger(LOG_LEVEL_ERROR, "Addition of vertices not working");
			return 1;
		}

		// Or, if we have pointers, we can use the addVertex-function.
		pxl::Vertex* vertex1Pointer = &vertex1;
		pxl::Vertex* vertex2Pointer = &vertex2;
		vertex1Pointer->addVertex(vertex2Pointer);

		if (std::abs(vertex1Pointer->getX() - 7.) > _epsilon || std::abs(
				vertex1Pointer->getY() - 5) > _epsilon || std::abs(
				vertex1Pointer->getZ() + 10.6) > _epsilon)
		{
			logger(LOG_LEVEL_ERROR,
					"Addition of vertices via pointers not working");
			return 1;
		}

		// More advanced arithmetics with the underlying three vector can be performed by accessing
		// the vector directly.
		logger(LOG_LEVEL_DEBUG, "Cosine theta of vector 1: ",
				vertex1.getVector().getCosTheta());
	}

	{
		logger(LOG_LEVEL_DEBUG, "b) pxl::Particle");
		pxl::Particle* ptl1 = new pxl::Particle();

		// Set some particle properties.
		// To avoid ambiguities between different representations of fourvectors,
		// you can only set all fourvector components at once.
		// The pxl::Particle uses the (px, py, pz, E)-representation.
		ptl1->setP4(5., 4., 7., 30.);
		ptl1->setCharge(1. / 3.);
		ptl1->setName("b quark");
		ptl1->setPdgNumber(5);

		ptl1->getAs<TestParticle> ()->set()->add(4.);

		if (std::abs(ptl1->getPx() - 9.) > _epsilon)
		{
			logger(LOG_LEVEL_ERROR,
					"Particle: getAs<TestParticle>()->set() not working");
			return 1;
		}

		delete ptl1; //remember to delete objects created with new
	}
	return 0;
}

int GeneralTest::testUserRecords()
{

	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG, "******** 2) Test of the UserRecord ********");

	{
		/* Each physics object in PXL has a UserRecord, consisting
		 * of (string, type)-pairs, where type is any basic C++ type,
		 * plus the std::string.
		 */
		pxl::Particle* ptl1 = new pxl::Particle();
		// set a double value
		ptl1->setName("muon");
		ptl1->setUserRecord("calorimeter isolation", 0.4);
		ptl1->setUserRecord("number of hits", 4);
		ptl1->setUserRecord("quality", "good");

		// Get UserRecord entry, providing no default item. Attention: In case
		// the entry is not present, an exception is thrown.
		logger(LOG_LEVEL_DEBUG, "Number of hits: ", ptl1->getUserRecord(
				"number of hits"));

		if (ptl1->getUserRecord("number of hits") != 4)
		{
			logger(LOG_LEVEL_ERROR,
					"UserRecord: setting or retrieving of integer entry not working");
			return 1;
		}

		if (ptl1->getUserRecord("calorimeter isolation") != 0.4)
		{
			logger(LOG_LEVEL_ERROR,
					"UserRecord: setting or retrieving of double entry not working");
			return 1;
		}

		if (ptl1->getUserRecord("quality") != "good")
		{
			logger(LOG_LEVEL_ERROR,
					"UserRecord: setting or retrieving of string entry not working");
			return 1;
		}

		// Get UserRecord entry
		logger(LOG_LEVEL_DEBUG, "Quality: ", ptl1->getUserRecord("quality"));

		// Remove a certain UserRecord entry.
		ptl1->eraseUserRecord("quality");

		if (ptl1->hasUserRecord("quality"))
		{
			logger(LOG_LEVEL_ERROR, "UserRecord: removing of entry not working");
			return 1;
		}

		if (!ptl1->hasUserRecord("number of hits"))
		{
			logger(LOG_LEVEL_ERROR,
					"UserRecord: checking/setting of entry not working");
			return 1;
		}

		ptl1->getUserRecords().change<int> ("number of hits", 5);

		if (ptl1->getUserRecord("number of hits") != 5)
		{
			logger(LOG_LEVEL_ERROR, "UserRecord: changing of entry not working");
			return 1;
		}

		ptl1->getUserRecords().clear();
		if (ptl1->getUserRecords().size() != 0)
		{
			logger(LOG_LEVEL_ERROR,
					"UserRecord: clearing of user records not working");
			return 1;
		}

		delete ptl1;
	}
	return 0;
}

int GeneralTest::testRelations()
{

	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG,
			"******** 3) Test of the Relation Management ********");
	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "a) Mother, Daughter, Flat Relations ");
		logger(LOG_LEVEL_DEBUG, "");

		pxl::Particle* p1 = new pxl::Particle();
		pxl::Particle* p2 = new pxl::Particle();
		p1->setP4(10., 10., 14.1, 20.);
		p2->setP4(-10., -12., 0., 16.);

		pxl::Particle* m1 = new pxl::Particle();
		pxl::Particle* m2 = new pxl::Particle();
		pxl::Vertex* v1 = new pxl::Vertex();
		pxl::Collision* c1 = new pxl::Collision();

		logger(LOG_LEVEL_DEBUG, "Link two daughters two one mother.");
		p1->linkMother(m1);
		p2->linkMother(m1);

		logger(LOG_LEVEL_DEBUG, "Relate to other pxl::Relative derivatives.");
		m1->linkMother(v1);
		v1->linkMother(c1);

		logger(LOG_LEVEL_DEBUG, "Unlink and print decay tree.");
		c1->unlinkDaughter(v1);

		logger(LOG_LEVEL_DEBUG, "Set kinematics from daughters.");
		m1->setP4FromDaughters();

		const pxl::Relations& daughterRel = m1->getDaughterRelations();
		logger(LOG_LEVEL_DEBUG, "Loop over daughter relations of size ",
				daughterRel.size(), ".");

		logger(LOG_LEVEL_DEBUG, "Is v1 mother of m1? ",
				v1->getDaughterRelations().has(m1));
		logger(LOG_LEVEL_DEBUG, "Is v1 mother of c1? ",
				v1->getDaughterRelations().has(c1));

		logger(LOG_LEVEL_DEBUG, "Get all related pxl::Particles.");
		std::vector<pxl::Particle*> particles;
		int nParticleDaughters = daughterRel.getObjectsOfType<pxl::Particle> (
				particles);
		logger(LOG_LEVEL_DEBUG, "There are ", nParticleDaughters,
				" daughters of type pxl::Particle.");

		logger(LOG_LEVEL_DEBUG, "Create and iterate flat relations.");
		m1->linkFlat(m2);
		for (pxl::Relations::const_iterator iter =
				m1->getFlatRelations().begin(); iter
				!= m1->getFlatRelations().end(); ++iter)
		{
		}

		delete p1;
		delete p2;
		delete m1;
		delete m2;
		delete v1;
		delete c1;
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "b) Soft Relations ");
		logger(LOG_LEVEL_DEBUG, "");

		pxl::Particle* p1 = new pxl::Particle();
		pxl::Particle* p2 = new pxl::Particle();

		pxl::Vertex* v1 = new pxl::Vertex();
		pxl::Collision* c1 = new pxl::Collision();

		logger(LOG_LEVEL_DEBUG, "Link a particle p1 to several other objects.");
		p1->linkSoft(p2, "gen-reco");
		p1->linkSoft(v1, "particle-vertex");
		p1->linkSoft(c1, "particle-collision");

		logger(LOG_LEVEL_DEBUG, "Loop soft relations and look at content.");
		for (pxl::SoftRelations::const_iterator iter =
				p1->getSoftRelations().begin(); iter
				!= p1->getSoftRelations().end(); ++iter)
		{
			logger(LOG_LEVEL_DEBUG, "Soft Relation Name/UUID: ('", iter->first,
					"', ", iter->second, "). ");
		}

		logger(LOG_LEVEL_DEBUG, "Several checks if soft relations exist.");

		if (p1->getSoftRelations().has(p2))
			logger(LOG_LEVEL_DEBUG, "Soft relation to p2 exists.");
		if (p1->getSoftRelations().hasType("particle-collision"))
			logger(LOG_LEVEL_DEBUG,
					"Soft relation named 'particle-collision' exists.");
		if (p2->getSoftRelations().has(p1, "gen-reco"))
			logger(LOG_LEVEL_DEBUG,
					"p2 has a soft relation named 'gen-reco' to p1.");
		logger(LOG_LEVEL_DEBUG, "p1 has ", p1->getSoftRelations().count(
				"gen-reco"), " soft relation(s) of type 'gen-reco'. ");

		logger(
				LOG_LEVEL_DEBUG,
				"To actually get the related objects, an object container needs to be provided. This will be detailed later.");

		delete p1;
		delete p2;
		delete v1;
		delete c1;
	}
	return 0;
}

int GeneralTest::testObjectOwner()
{

	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG,
			"******** 4) Test of the object owner: the Event and the EventView ********");

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "a) The EventView ");
		logger(LOG_LEVEL_DEBUG, "");

		pxl::EventView* ev = new pxl::EventView();
		logger(LOG_LEVEL_DEBUG,
				"Set basic properties of the EventView, e.g. name, UserRecord.");
		ev->setName("reconstructed");
		ev->setUserRecord("hypothesis nr", 4);

		logger(LOG_LEVEL_DEBUG,
				"Create Relative derivatives, e.g. particles and vertices.");
		pxl::Particle* p1 = ev->create<pxl::Particle> ();
		pxl::Particle* p2 = ev->create<pxl::Particle> ();
		pxl::Vertex* v1 = ev->create<pxl::Vertex> ();
		logger(LOG_LEVEL_DEBUG, "EventView contains (via the ObjectOwner) ",
				ev->getObjectOwner().size(), " objects. ");
		logger(LOG_LEVEL_DEBUG,
				"Does p1 belong to this EventView's ObjectOwner? ",
				ev->getObjectOwner().has(p1));
		logger(LOG_LEVEL_DEBUG,
				"These belong to the ObjectOwner (contained in the EventView).");
		logger(LOG_LEVEL_DEBUG,
				"The ObjectOwner has the deletion responsibility.");
		logger(LOG_LEVEL_DEBUG, "Relations are safe due to the ObjectOwner.");
		p1->linkMother(v1);
		p2->linkMother(v1);

		logger(LOG_LEVEL_DEBUG, "Remove an object.");
		ev->removeObject(p1);

		logger(LOG_LEVEL_DEBUG,
				"Get particles from the ObjectOwner and iterate.");
		std::vector<pxl::Particle*> particles;
		ev->getObjectsOfType<pxl::Particle> (particles);

		logger(LOG_LEVEL_DEBUG, "Alternatively use the TypeIterator.");
		for (pxl::ObjectOwnerTypeIterator<pxl::Particle> it =
				ev->getObjectOwner().begin<pxl::Particle> (); it
				!= ev->getObjectOwner().end(); ++it)
		{
			logger(LOG_LEVEL_DEBUG, "Particle with name ", (*it)->getName());
		}

		logger(LOG_LEVEL_DEBUG, "For fast access, an index can be set.");
		ev->setIndexEntry("muon", p2);
		ev->setIndexEntry("production vertex", v1);

		pxl::Particle* ele = ev->findObject<pxl::Particle> ("electron");
		if (ele)
			logger(LOG_LEVEL_DEBUG, "Found an electron.");
		pxl::Particle* muon = ev->findObject<pxl::Particle> ("muon");
		if (muon)
		{
			logger(LOG_LEVEL_DEBUG, "Found a muon.");
		}

		logger(LOG_LEVEL_DEBUG, "Relations are copied...");
		pxl::EventView* copyEv = new pxl::EventView(ev);
		copyEv->setName("Copied EventView");
		logger(LOG_LEVEL_DEBUG, "... as well as the index.");
		pxl::Vertex* prodVertex = ev->findObject<pxl::Vertex> (
				"production vertex");
		if (prodVertex)
			logger(LOG_LEVEL_DEBUG, "Production vertex found.");

		logger(LOG_LEVEL_DEBUG,
				"The CopyHistory can find the new vertex from the original vertex.");
		pxl::Vertex* copyVertex = copyEv->findCopyOf<pxl::Vertex> (v1);
		if (copyVertex)
			logger(LOG_LEVEL_DEBUG, "Copied vertex was found.");

		delete copyEv;
		delete ev;
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "b) The Event");
		logger(LOG_LEVEL_DEBUG, "");

		pxl::Event* event = new pxl::Event();

		logger(LOG_LEVEL_DEBUG, "The pxl::Event also has an object owner.");
		logger(LOG_LEVEL_DEBUG,
				"Create two EventViews, directly make an index entry.");

		pxl::EventView* ev1 =
				event->createIndexed<pxl::EventView> ("generated");
		pxl::EventView* ev2 = event->createIndexed<pxl::EventView> (
				"reconstructed");

		ev1->linkFlat(ev2);

		logger(
				LOG_LEVEL_DEBUG,
				"The event is the basic physical entity, so it has no name, workflag or lock flag.");
		logger(LOG_LEVEL_DEBUG, "Put information into the UserRecord.");
		event->setUserRecord("Name", "2 Jet 1 Muon Event");

		logger(LOG_LEVEL_DEBUG, "Create some objects within the EventViews.");
		pxl::Particle* p1 = ev1->create<pxl::Particle> ();
		pxl::Particle* p2 = ev1->create<pxl::Particle> ();
		pxl::Particle* m1 = ev1->create<pxl::Particle> ();
		p1->linkMother(m1);
		p2->linkMother(m1);
		ev2->create<pxl::Particle> ()->setName("Jet");
		ev2->create<pxl::Particle> ()->setName("Jet");
		ev2->create<pxl::Particle> ()->setName("Muon");

		delete event;
	}
	return 0;

}

int GeneralTest::testWeakPointers()
{

	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG, "******** 5) Weak pointers, Pointers ********");

	{
		logger(LOG_LEVEL_DEBUG,
				"As an additional example, use std::auto_ptr for the first object created.");
		std::auto_ptr<pxl::EventView> ev(new pxl::EventView());

		logger(LOG_LEVEL_DEBUG,
				"We can implicitly convert a returned pointer to a pxl::weak_ptr. ");
		pxl::weak_ptr<pxl::Particle> partWkPtr = ev->create<pxl::Particle> ();

		logger(LOG_LEVEL_DEBUG, "Access with the -> operator...");
		partWkPtr->setName("jet");

		logger(LOG_LEVEL_DEBUG, "... or with the ptr() method...");
		partWkPtr.ptr()->setP4(10., 10., 10., 30.);

		logger(LOG_LEVEL_DEBUG, "... or with the object() method...");

		logger(
				LOG_LEVEL_DEBUG,
				"An exception is thrown in case the object was deleted. To prevent this, check if the weak_ptr is valid.");

		logger(LOG_LEVEL_DEBUG, "Remove the object.");
		ev->removeObject(partWkPtr);

		logger(LOG_LEVEL_DEBUG, "Check if the weak pointer is still valid.");
		if (partWkPtr) // Note the implicit conversion to a pointer.
			logger(LOG_LEVEL_DEBUG, "wk_ptr still valid.");
		else
			logger(LOG_LEVEL_DEBUG, "wk_ptr not valid!");
		logger(LOG_LEVEL_DEBUG, "Alternative syntax. Valid? ",
				partWkPtr.valid());

		logger(
				LOG_LEVEL_DEBUG,
				"This is not possible for a normal pointer - this would be a hard-to-trace bug.");
	}
	return 0;
}

int GeneralTest::testInputOutput()
{

	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG, "******** 6) I/O ********");

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "a) Writing a file which contains pxl::Events");
		logger(LOG_LEVEL_DEBUG, "");

		logger(
				LOG_LEVEL_DEBUG,
				"First create a new pxl::OutputFile object with the filename as constructor argument. ");
		pxl::OutputFile outFile("pxlexample_1.pxlio");
		for (int i = 0; i < 100; ++i)
		{
			std::auto_ptr<pxl::Event> newEvent(new pxl::Event());

			pxl::weak_ptr<pxl::EventView> evtView = newEvent->create<
					pxl::EventView> ();

			pxl::weak_ptr<pxl::Particle> electron1 = evtView->create<
					pxl::Particle> ();
			electron1->setP4(40., -20., 31., 80.);
			electron1->setName("electron");
			electron1->setUserRecord("calo isolation", 0.6);

			pxl::weak_ptr<pxl::Particle> electron2 = evtView->create<
					pxl::Particle> ();
			electron2->setP4(19., 41., 10., 70.);
			electron2->setName("electron");
			electron2->setUserRecord("calo isolation", 0.3);

			pxl::weak_ptr<pxl::Particle> zboson =
					evtView->create<pxl::Particle> ();
			zboson->setName("Z");
			zboson->linkDaughter(electron1);
			zboson->linkDaughter(electron2);
			zboson->setP4FromDaughters();

			electron1->linkSoft(electron2, "we are both electrons");
			if (i == 0)
			{
				logger(LOG_LEVEL_DEBUG,
						"Meanwhile: Demonstrate how to access Soft Relations with ObjectOwner.");
				pxl::Relative
						* related =
								(pxl::Relative *) electron2->getSoftRelations().getFirst(
										evtView->getObjectOwner());
				if (related)
					logger(LOG_LEVEL_DEBUG, "Name of related relative: ",
							related->getName());
			}
			if (i == 0)
				logger(
						LOG_LEVEL_DEBUG,
						"For each event, use the streamObject method to write the complete physics event to disk.");
			outFile.streamObject(newEvent.get());
		}
		outFile.close();
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "b) Reading the file containing pxl::Events");
		logger(LOG_LEVEL_DEBUG, "");

		logger(LOG_LEVEL_DEBUG, "Book an input file.");

		pxl::InputFile inputFile("pxlexample_1.pxlio");

		logger(LOG_LEVEL_DEBUG, "Size of file: ", inputFile.getSize());
		logger(LOG_LEVEL_DEBUG, "Current position: ", inputFile.getPosition());

		int nEvents = 0;

		while (inputFile.good())
		{
			pxl::Event* event =
					dynamic_cast<pxl::Event*> (inputFile.readNextObject());
			if (!event)
				continue;
			nEvents = inputFile.objectCount();

			if (nEvents == 1)
			{
				logger(LOG_LEVEL_DEBUG,
						"Get the contained EventViews, and then the Particles contained therein.");
			}
			std::vector<pxl::EventView*> eventViews;
			event->getObjectsOfType<pxl::EventView> (eventViews);

			for (std::vector<pxl::EventView*>::const_iterator it =
					eventViews.begin(); it != eventViews.end(); ++it)
			{
				std::vector<pxl::Particle*> particles;
				pxl::EventView* evView = *it;
				evView->getObjectsOfType<pxl::Particle> (particles);
			}
			delete event;
		}
		inputFile.close();
		std::remove("pxlexample_1.pxlio");
		logger(LOG_LEVEL_DEBUG, "Processed ", nEvents, " events. ");
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG,
				"c) Writing a file which contains arbitrary objects.");
		logger(LOG_LEVEL_DEBUG, "");

		logger(LOG_LEVEL_DEBUG, "Create a new pxl::OutputFile. ");
		pxl::OutputFile outFile("pxlexample_2.pxlio");
		for (int i = 0; i < 100; ++i)
		{
			if (i == 0)
				logger(LOG_LEVEL_DEBUG,
						"For each object, use the streamObject method to add it to the output stream.");

			pxl::Particle* electron(new pxl::Particle);
			electron->setP4(50., 30., 40., 71.);

			pxl::Vertex* vertex(new pxl::Vertex);
			vertex->setX(0.3);
			vertex->setY(0.8);
			vertex->setZ(-3.8);

			pxl::EventView* ev(new pxl::EventView);
			ev->setUserRecord("likelihood", 0.7);

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
		logger(LOG_LEVEL_DEBUG,
				"d) Reading a file which contains arbitrary objects");
		pxl::InputFile inFile("pxlexample_2.pxlio");

		logger(LOG_LEVEL_DEBUG, "File good? ", inFile.good());

		pxl::Serializable* obj = inFile.readNextObject();
		if (obj)
			logger(LOG_LEVEL_DEBUG, obj->getId(), ", ", obj->getTypeId());

		pxl::Relative* relP = dynamic_cast<pxl::Relative*> (obj);
		if (relP)
			logger(LOG_LEVEL_DEBUG, relP->getId(), ", ", relP->getTypeId());

		logger(
				LOG_LEVEL_DEBUG,
				"The user takes deletion responsibility when using readNextObject(), seekToObject or readPreviousObject!");

		delete obj;

		obj = inFile.seekToObject(20);
		if (obj)
			logger(LOG_LEVEL_DEBUG, obj->getId(), ", ", obj->getTypeId());

		relP = dynamic_cast<pxl::Relative*> (obj);

		delete obj;

		obj = inFile.seekToObject(20);
		if (obj)
			logger(LOG_LEVEL_DEBUG, obj->getId(), ", ", obj->getTypeId());

		relP = dynamic_cast<pxl::Relative*> (obj);

		delete obj;

		obj = inFile.readPreviousObject();
		logger(LOG_LEVEL_DEBUG, obj->getId(), ", ", obj->getTypeId());

		relP = dynamic_cast<pxl::Relative*> (obj);

		delete obj;
		std::remove("pxlexample_2.pxlio");
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "e) Writing a file with InformationChunks.");
		logger(LOG_LEVEL_DEBUG, "");

		logger(LOG_LEVEL_DEBUG, "Create a new pxl::OutputFile. ");
		pxl::OutputFile outFile("pxlexample_3.pxlio");

		pxl::InformationChunk* info(new pxl::InformationChunk);

		info->setName("W+Jets");
		info->setUserRecord("pt Cut", 70.);

		logger(
				LOG_LEVEL_DEBUG,
				"Write InformationChunk and finish the filesection, so it can be quickly read in.");
		outFile.streamObject(info);
		outFile.writeFileSection();

		delete info;

		// event loop
		for (int i = 0; i < 100; ++i)
		{
			pxl::Event event1;
			pxl::Event event2;
			event1.setUserRecord("likelihood", 0.7);
			event2.setUserRecord("likelihood", 0.3);
			outFile.streamObject(&event1);
			outFile.streamObject(&event2);
		}
		outFile.close();
	}

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "f) Reading a file with InformationChunks.");
		logger(LOG_LEVEL_DEBUG, "");

		logger(LOG_LEVEL_DEBUG, "Create a new pxl::InputFile. ");
		pxl::InputFile inFile("pxlexample_3.pxlio");

		logger(LOG_LEVEL_DEBUG,
				"Get the information chunk and print restored UserRecord.");

		pxl::InformationChunk* info =
				dynamic_cast<pxl::InformationChunk*> (inFile.readNextObject());
		delete info;

		logger(LOG_LEVEL_DEBUG, "Read the rest of the file.");

		inFile.close();
		std::remove("pxlexample_3.pxlio");
	}
	return 0;

}

int GeneralTest::testAdditionalFeatures()
{
	logger(LOG_LEVEL_DEBUG, "");
	logger(LOG_LEVEL_DEBUG, "******** 7) Additional Functionality ********");

	{
		logger(LOG_LEVEL_DEBUG, "");
		logger(LOG_LEVEL_DEBUG, "a) Filters and Sorting");
		logger(LOG_LEVEL_DEBUG, "");

		logger(LOG_LEVEL_DEBUG, "Create EventView with three particles.");
		pxl::EventView* ev = new pxl::EventView();

		pxl::Particle* gluon = ev->createIndexed<pxl::Particle> ("gluon");
		gluon->setName("gluon");
		pxl::Particle* b = ev->createIndexed<pxl::Particle> ("b");
		b->setName("b");
		b->setP4(20., 25., 0., 35.);
		pxl::Particle* bbar = ev->createIndexed<pxl::Particle> ("bbar");
		bbar->setP4(100., 100., 0., 142.);
		bbar->setName("bbar");

		logger(LOG_LEVEL_DEBUG, "Filter & pt-sort particles.");
		pxl::ParticleFilter filter;
		std::vector<pxl::Particle*> particles;

		logger(LOG_LEVEL_DEBUG, "Choose a pt, eta, name filter criterion");
		pxl::ParticlePtEtaNameCriterion crit("gluon", 0., 0.);
		logger(LOG_LEVEL_DEBUG, "Apply filter. ");
		filter.apply(ev->getObjectOwner(), particles, crit);

		logger(LOG_LEVEL_DEBUG, "Print, sorted by pT: ");
		for (std::vector<pxl::Particle*>::const_iterator iter =
				particles.begin(); iter != particles.end(); ++iter)
		{
			pxl::weak_ptr<pxl::Particle> pa = *iter;
			logger(LOG_LEVEL_DEBUG, " ", pa->getName());
			logger(LOG_LEVEL_DEBUG, ", pT = ", pa->getPt());
			logger(LOG_LEVEL_DEBUG, "");
		}

		particles.clear();

		logger(LOG_LEVEL_DEBUG, "Take a pt filter criterion only. ");
		pxl::ParticlePtCriterion ptCrit(50.);
		filter.apply(ev->getObjectOwner(), particles, ptCrit);

		logger(LOG_LEVEL_DEBUG,
				"Print all objects with pt bigger than 50, sorted by pT: ");
		for (std::vector<pxl::Particle*>::const_iterator iter =
				particles.begin(); iter != particles.end(); ++iter)
		{
			pxl::weak_ptr<pxl::Particle> pa = *iter;
			logger(LOG_LEVEL_DEBUG, " ", pa->getName());
			logger(LOG_LEVEL_DEBUG, ", pT = ", pa->getPt());
			logger(LOG_LEVEL_DEBUG, "");
		}

		delete ev;

	}
	return 0;
}

int GeneralTest::testTechnicalDetails()
{
	logger(LOG_LEVEL_DEBUG, "******** 8) Technical Details ********");

	{
		logger(LOG_LEVEL_DEBUG, "a) UserRecord Copy-On-Write");

		pxl::EventView* ev = new pxl::EventView();
		ev->setUserRecord("run nr.", 192168);

		pxl::EventView* ev_copy = new pxl::EventView(ev);

		if (ev_copy->getUserRecords().getContainer()
				!= ev->getUserRecords().getContainer())
		{
			logger(LOG_LEVEL_ERROR,
					"Copy-On-Write of UserRecord map not working properly");
			return 1;
		}

		ev_copy->setUserRecord("run nr.", 1921124);

		if (ev_copy->getUserRecords().getContainer()
				== ev->getUserRecords().getContainer())
		{
			logger(LOG_LEVEL_ERROR,
					"Copy-On-Write (after writing) of UserRecord map not working properly");
			return 1;
		}

		delete ev;
		delete ev_copy;
	}
	return 0;
}

int GeneralTest::testSerializableInUserrecords()
{
	pxl::Particle* p = new pxl::Particle();
	p->setP4(0., 30., 40., 50.);
	p->setUserRecord("name", "top");

	pxl::Particle* p2 = new pxl::Particle();
	p2->setUserRecord("top quark", p);

	const Variant &v = p2->getUserRecord("top quark");
	const pxl::Particle *found = v.cast<pxl::Particle *> ();
	if (found == 0 || std::abs(found->getPy() - 30.) > _epsilon)
	{
		logger(LOG_LEVEL_ERROR,
				"Problem with properties of particle in UserRecord");
		return 1;
	}

	pxl::OutputFile outFile("ur_serializable_test.pxlio");
	outFile.streamObject(p2);
	outFile.close();

	delete p;
	delete p2;

	pxl::InputFile inFile("ur_serializable_test.pxlio");

	pxl::Particle* pRead =
			dynamic_cast<pxl::Particle*> (inFile.readNextObject());
	if (!pRead)
	{
		logger(LOG_LEVEL_ERROR, "Problem with restoring particle from file");
		return 1;
	}

	const pxl::Particle *foundRead = pRead->getUserRecord("top quark").cast<
			pxl::Particle *> ();
	if (foundRead == 0 || std::abs(foundRead->getPy() - 30.) > _epsilon)
	{
		logger(LOG_LEVEL_ERROR,
				"Problem with properties of restored particle in UserRecord");
		return 1;
	}

	delete pRead;

	std::remove("ur_serializable_test.pxlio");

	return 0;
}

int GeneralTest::testVariant()
{
	if (Variant::fromString("true", Variant::TYPE_BOOL).asBool() != true)
	{
		logger(LOG_LEVEL_ERROR, "Variant true test failed");
		return 1;
	}

	if (Variant::fromString("no", Variant::TYPE_BOOL).asBool() != false)
	{
		logger(LOG_LEVEL_ERROR, "Variant no test failed");
		return 1;
	}

	if (Variant::fromString("3.0", Variant::TYPE_FLOAT).asFloat() != 3.0f)
	{
		logger(LOG_LEVEL_ERROR, "Variant float test failed");
		return 1;
	}

	if (Variant::fromString("3.0", Variant::TYPE_DOUBLE).asDouble() != 3.0)
	{
		logger(LOG_LEVEL_ERROR, "Variant double test failed");
		return 1;
	}

	if (Variant::fromString("4", Variant::TYPE_CHAR).asChar() != '4')
	{
		logger(LOG_LEVEL_ERROR, "Variant char test failed");
		return 1;
	}

	if (Variant::fromString("a", Variant::TYPE_UCHAR).asUChar() != 'a')
	{
		logger(LOG_LEVEL_ERROR, "Variant uchar test failed");
		return 1;
	}

	if (Variant::fromString("-20000", Variant::TYPE_INT16).asInt16() != -20000)
	{
		logger(LOG_LEVEL_ERROR, "Variant int16 test failed");
		return 1;
	}

	if (Variant::fromString("60000", Variant::TYPE_UINT16).asUInt16() != 60000)
	{
		logger(LOG_LEVEL_ERROR, "Variant uint16 test failed");
		return 1;
	}

#ifdef MSVC
	#define LL
#endif
	if (Variant::fromString("-200000000000", Variant::TYPE_INT64).asInt64()
			!= -200000000000LL)
	{
		logger(LOG_LEVEL_ERROR, "Variant int64 test failed");
		return 1;
	}
#ifdef MSVC
	#undef LL
#endif

#ifdef MSVC
	#define ULL
#endif
	if (Variant::fromString("200000000000", Variant::TYPE_UINT64).asUInt64()
			!= 200000000000ULL)
	{
		logger(LOG_LEVEL_ERROR, "Variant uint64 test failed");
		return 1;
	}
#ifdef MSVC
	#undef ULL
#endif

	if (Variant::fromString("3.0 4.0 6.0", Variant::TYPE_BASIC3VECTOR).asBasic3Vector()
			!= Basic3Vector(3.0, 4.0, 6.0))
	{
		logger(LOG_LEVEL_ERROR, "Variant Basic3Vector test failed");
		return 1;
	}

	if (Variant::fromString("3.0 4.0 6.0 10.0", Variant::TYPE_LORENTZVECTOR).asLorentzVector()
			!= LorentzVector(3.0, 4.0, 6.0, 10.0))
	{
		logger(LOG_LEVEL_ERROR, "Variant LorentzVector test failed");
		return 1;
	}

	if (Variant::fromString("test string", Variant::TYPE_STRING).asString()
			!= "test string")
	{
		logger(LOG_LEVEL_ERROR, "Variant string test failed");
		return 1;
	}

	// types

	if (Variant::toType("float") != Variant::TYPE_FLOAT)
	{
		logger(LOG_LEVEL_ERROR, "Variant float type failed");
		return 1;
	}

	if (Variant::toType("double") != Variant::TYPE_DOUBLE)
	{
		logger(LOG_LEVEL_ERROR, "Variant double type failed");
		return 1;
	}

	if (Variant::toType("char") != Variant::TYPE_CHAR)
	{
		logger(LOG_LEVEL_ERROR, "Variant char type failed");
		return 1;
	}

	if (Variant::toType("uchar") != Variant::TYPE_UCHAR)
	{
		logger(LOG_LEVEL_ERROR, "Variant uchar type failed");
		return 1;
	}

	if (Variant::toType("int16") != Variant::TYPE_INT16)
	{
		logger(LOG_LEVEL_ERROR, "Variant int16 type failed");
		return 1;
	}

	if (Variant::toType("uint16") != Variant::TYPE_UINT16)
	{
		logger(LOG_LEVEL_ERROR, "Variant uint16 type failed");
		return 1;
	}

	if (Variant::toType("int32") != Variant::TYPE_INT32)
	{
		logger(LOG_LEVEL_ERROR, "Variant int32 type failed");
		return 1;
	}

	if (Variant::toType("uint32") != Variant::TYPE_UINT32)
	{
		logger(LOG_LEVEL_ERROR, "Variant uint32 type failed");
		return 1;
	}

	if (Variant::toType("int64") != Variant::TYPE_INT64)
	{
		logger(LOG_LEVEL_ERROR, "Variant int64 type failed");
		return 1;
	}

	if (Variant::toType("uint64") != Variant::TYPE_UINT64)
	{
		logger(LOG_LEVEL_ERROR, "Variant uint64 type failed");
		return 1;
	}

	if (Variant::toType("string") != Variant::TYPE_STRING)
	{
		logger(LOG_LEVEL_ERROR, "Variant string type failed");
		return 1;
	}

	return 0;
}

} // namespace pxl

int main()
{
	pxl::Core::initialize();
	pxl::Hep::initialize();

	pxl::GeneralTest test;


	logger(pxl::LOG_LEVEL_DEBUG, " ");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   **********************************************************");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   * This is the PXL test.                                  *");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   **********************************************************");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   *                                                        *");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   *                                                        *");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   * January 2010                                           *");
	logger(pxl::LOG_LEVEL_DEBUG,
			"   **********************************************************");
	logger(pxl::LOG_LEVEL_DEBUG, " ");

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
	 */

	int result = 0;
	int globalResult = 0;

	result = test.testPhysicsObjects();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testPhysicsObjects: ", result == 0 ? "success"
			: "fail", ", ", result);

	result = test.testUserRecords();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testUserRecords: ", result);

	result = test.testRelations();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testRelations: ", result);

	result = test.testObjectOwner();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testObjectOwner: ", result);

	result = test.testWeakPointers();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testWeakPointers: ", result);

	result = test.testInputOutput();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testInputOutput: ", result);

	result = test.testAdditionalFeatures();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testAdditionalFeatures: ", result);

	result = test.testTechnicalDetails();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testTechnicalDetails: ", result);

	result = test.testSerializableInUserrecords();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testSerializableInUserrecords: ", result);

	result = test.testVariant();
	globalResult += result;
	logger(pxl::LOG_LEVEL_INFO, "testVariant: ", result);

	logger(pxl::LOG_LEVEL_DEBUG, "");
	logger(pxl::LOG_LEVEL_DEBUG, "******** End of Test ********");
	logger(pxl::LOG_LEVEL_DEBUG, "");

	return globalResult;
}
