#! /usr/bin/env python
#-------------------------------------------
# Project: Physics eXtension Library (PXL) -
#          http:#pxl.sourceforge.net      -
# Copyright (C) 2006-2008                  -
#               RWTH Aachen, Germany       -
# Contact: pxl-users@lists.sourceforge.net -
#-------------------------------------------
import sys
import os
topsourcedir = os.path.join('..','build/lib/python2.5/site-packages/')
sys.path.append(topsourcedir)

from pxl.hep import *
from pxl.base import *
from pxl.io import *

#----------------------------------------------------------------------
#/ This program provides code examples for how to use the PXL library.
#/ Open the source code to see an illustration of how the basic classes are used.
#/ The example demonstrates how to access data and 
#/ establish mother/daughter relations with other objects. The event container
#/ pxl::EventView with physics objects such as pxl::Particle, pxl::Vertex
#/ and pxl::Collision is introduced and various code examples are given.

if __name__ == "__main__":

    print " " 
    print "   **********************************************************"
    print "   * This is the PXL example.                               *"
    print "   **********************************************************"
    print "   *                                                        *"
    print "   *                                                        *"
    print "   * June 2008                                              *"
    print "   **********************************************************"
    print " " 

    #
    # Structure according to the manual:
    # 1) Physics Objects
    # 2) User Record
    # 3) Relation Management
    # 4) Object Owner
    # 5) Pointer and Weak Pointer
    # 6) Copy-On-Write
    # 7) I/O
    # 8) Additional Functionality
    #

    print "\n"
    print "******** 1) Demonstration of Physics Objects ********"
            

    if True:
        print "\na) pxl::Vertex \n" 
        # A simple physics object is the vertex:
        vertex1 = Vertex()
        # We can print the standard information with the print method.
        print vertex1
        # This is boring, so we set the name and the components of the vertex.
        vertex1.setName("my vertex 1")
        vertex1.setX(4.)
        vertex1.setY(1.)
        vertex1.setZ(0.4)
        # Now the printout is more exciting:
        print vertex1
        # We create a second vertex
        vertex2 = Vertex()
        vertex2.setName("my vertex 2")
        vertex2.setX(- 1.)
        vertex2.setY(3.)
        vertex2.setZ(- 9.8)
        print vertex2
        # Now we can do more complicated things.
        vertex2+=vertex1
        print vertex2
        # Or, we can use the addVertex-function.
        vertex1.addVertex(vertex2)
        print vertex1
        # More advanced arithmetics with the underlying three vector can be performed by accessing
        # the vector directly.
        print "Cosine theta of vector 1:" , vertex1.getVector().getCosTheta() 
    

    if True:
        print "\nb) pxl::Particle \n" 
        ptl1 = Particle()
        print ptl1
        # Set some particle properties.
        # To avoid ambiguities between different representations of fourvectors,
        # you can only set all fourvector components at once.
        # The pxl::Particle uses the (px, py, pz, E)-representation.
        ptl1.setP4(5., 4., 7., 30.)
        ptl1.setCharge(1. / 3.)
        ptl1.setName("b quark")
        ptl1.setPdgNumber(5)
        print ptl1
        print "charge" , ptl1.getCharge()\
                , ", ID" , ptl1.getPdgNumber() 
    

    print "\n"
    print "******** 2) Demonstration of the UserRecord ******** \n"
            

    if True:
        # Each physics object in PXL has a UserRecord, consisting
        # of (string, type)-pairs, where type is any basic C++ type,
        # plus the std::string.
        #
        ptl1 = Particle()
        # set a double value
        ptl1.setName("muon")
        ptl1.setUserRecord("calorimeter isolation", 0.4)
        ptl1.setUserRecord("number of hits", 4)
        ptl1.setUserRecord("quality", "good")

        # Print all UserRecord entries.
        print ptl1
        # Or print them directly.
        print ptl1.getUserRecords()

        # Get UserRecord entry, providing no default item. Attention: In case
        # the entry is not present, an exception is thrown.
        print "Number of hits: "\
                , ptl1.getUserRecord("number of hits") 

        # Get UserRecord entry.
        print "Quality:" , ptl1.getUserRecord("quality"
                ) 
        # Remove a certain UserRecord entry.
        ptl1.eraseUserRecord("quality")
        print "Quality:" , ptl1.getUserRecord("quality"
                ) 

        # To check if a UserRecord is present, use one of the following methods. The
        # second one directly fills the according value into the passed variable if present.
        print "Is an entry named 'quality' present? "\
                , ptl1.hasUserRecord("quality") 

        print "Is an entry named 'number of hits' present? "\
                , ptl1.hasUserRecord("number of hits") 
# not supported in pypxl
#        isolation = 0.
#        if (ptl1.hasUserRecord("calorimeter isolation", isolation)):
#            print "Isolation variable present, value" , isolation
#                    
#        else:
#            print "Isolation variable not present." 

        # Looping over the UserRecord is also possible. However, subtleties
        # due to the underlying pxl::Variant are involved.
        for key in ptl1.getUserRecords().getContainer().keys():
            item=ptl1.getUserRecord(key)
            print "Key:" , key , ", item has type "\
                    , type(item) 
            print "A UserRecord entry is present, ("\
                        , key , "," , item\
                        , ")." 

        # Finally, the UserRecord can also be cleared.
        print "Size of UserRecord:" , ptl1.getUserRecords().size() 
        ptl1.getUserRecords().clear()
        print "Size of UserRecord:" , ptl1.getUserRecords().size() 

    

    print "\n"
    print "******** 3) Demonstration of the Relation Management ********"
            
    if True:
        print ""
        print "a) Mother, Daughter, Flat Relations " 
        print ""

        p1 = Particle()
        p2 = Particle()
        p1.setP4(10., 10., 14.1, 20.)
        p2.setP4(- 10., - 12., 0., 16.)

        m1 = Particle()
        m2 = Particle()
        v1 = Vertex()
        c1 = Collision()

        print "Link two daughters two one mother." 
        p1.linkMother(m1)
        p2.linkMother(m1)
        m1.printDecayTree()
        print "Relate to other pxl::Relative derivatives." 
        m1.linkMother(v1)
        v1.linkMother(c1)
        c1.printDecayTree()
        print "Unlink and print decay tree." 
        c1.unlinkDaughter(v1)
        c1.printDecayTree()

        print "Set kinematics from daughters." 
        m1.setP4FromDaughters()
        m1.printDecayTree()

        daughterRel = m1.getDaughterRelations()
        print "Loop over daughter relations of size "\
                , daughterRel.size() , "." 

        for daughter in daughterRel.getContainer():
            print daughter
        

        print "Is v1 mother of m1?" , v1.getDaughterRelations().has(m1) 
        print "Is v1 mother of c1?" , v1.getDaughterRelations().has(c1) 

        print "Get all related pxl::Particles." 
        nParticleDaughters = len(daughterRel.getObjectsOfType(Particle))
        print "There are" , nParticleDaughters\
                , " daughters of type pxl::Particle." 

        print "Create and iterate flat relations." 
        m1.linkFlat(m2)
        for iter in m1.getFlatRelations().getContainer():
            print iter
        

    

    if True:
        print ""
        print "b) Soft Relations " 
        print ""

        p1 = Particle()
        p2 = Particle()

        v1 = Vertex()
        c1 = Collision()

        print "Link a particle p1 to several other objects."
                
        p1.linkSoft(p2, "gen-reco")
        p1.linkSoft(v1, "particle-vertex")
        p1.linkSoft(c1, "particle-collision")

        print "Loop soft relations and look at content." 
        for key, item in p1.getSoftRelations().getContainer().items():
        
            print "Soft Relation Name/UUID: ('" , key , "', "\
                    , item, "). " 
        

        print "Several checks if soft relations exist." 

        if (p1.getSoftRelations().has(p2)):
            print "Soft relation to p2 exists." 
        if (p1.getSoftRelations().hasType("particle-collision")):
            print "Soft relation named 'particle-collision' exists."
                    
        if (p2.getSoftRelations().has(p1, "gen-reco")):
            print "p2 has a soft relation named 'gen-reco' to p1."
                    
        print "p1 has" , p1.getSoftRelations().count("gen-reco") , " soft relation(s) of type 'gen-reco'. "
                

        print "To actually get the related objects, an object container needs to be provided. This will be detailed later."
                

    

    print "\n"
    print "******** 4) Demonstration of the object owner: the Event and the EventView ********"
            

    if True:
        print ""
        print "a) The EventView " 
        print ""

        ev = EventView()
        print "Set basic properties of the EventView, e.g. name, UserRecord."
                
        ev.setName("reconstructed")
        ev.setUserRecord("hypothesis nr", 4)

        print "Create Relative derivatives, e.g. particles and vertices."
                
        p1 = ev.createParticle()
        p2 = ev.createParticle()
        v1 = ev.createVertex()
        print "EventView contains (via the ObjectOwner) "\
                , ev.getObjectOwner().size() , " objects. " 
        print "Does p1 belong to this EventView's ObjectOwner? "\
                , ev.getObjectOwner().has(p1) 
        print ev
        print "These belong to the ObjectOwner (contained in the EventView)."
                
        print "The ObjectOwner has the deletion responsibility."
                
        print "Relations are safe due to the ObjectOwner." 
        p1.linkMother(v1)
        p2.linkMother(v1)
        print ev

        print "\nRemove an object." 
        ev.removeObject(p1)
        print ev

        print "\nLoop over the contained objects via the ObjectOwner."
                
        for iter in ev.getObjectOwner().getObjects():
            print iter
        

        print "\nGet particles from the ObjectOwner and iterate."
                
        for pIt in ev.getObjectsOfType(Particle):
            print pIt
        

        print "For fast access, an index can be set." 
        ev.setIndexEntry("muon", p2)
        ev.setIndexEntry("production vertex", v1)

        ele = ev.findObject("electron")
        if (ele):
            print "Found an electron." 
        muon = ev.findObject("muon")
        if (muon):
        
            print "Found a muon." 
            print muon
        

        print "Relations are copied..." 
        copyEv = EventView(ev)
        copyEv.setName("Copied EventView")
        print copyEv
        print "... as well as the index." 
        prodVertex = \
                ev.findObject("production vertex")
        if (prodVertex):
            print "Production vertex found." 

# not supported in pypxl
#        print "The CopyHistory can find the new vertex from the original vertex."
#                
#        copyVertex = copyEv.findCopyOf(v1)
#        if (copyVertex):
#            print "Copied vertex was found." 

    

    if True:
        print ""
        print "b) The Event" 
        print ""

        event = Event()

        print "The pxl::Event also has an object owner." 
        print "Create two EventViews, directly make an index entry."
                

        ev1 = event.createIndexedEventView("generated")
        ev2 = event.createIndexedEventView("reconstructed")

        ev1.linkFlat(ev2)
        print event

        print "The event is the basic physical entity, so it has no name, workflag or lock flag."
                
        print "Put information into the UserRecord." 
        event.setUserRecord("Name", "2 Jet 1 Muon Event")

        print "Create some objects within the EventViews." 
        p1 = ev1.createParticle()
        p2 = ev1.createParticle()
        m1 = ev1.createParticle()
        p1.linkMother(m1)
        p2.linkMother(m1)
        ev2.createParticle().setName("Jet")
        ev2.createParticle().setName("Jet")
        ev2.createParticle().setName("Muon")

        print "** Print complete event. **" 
        print event
        print "** End print complete event. **" 

    

    print "\n"
    print "******** 6) I/O ********" 

    if True:
        print ""
        print "a) Writing a file which contains pxl::Events"
                
        print ""

        print "First create a OutputFile object with the filename as constructor argument. "
                
        outFile=OutputFile("pxlexample_1.pxlio")
        for i in range(100):
        
            newEvent=Event()

            evtView = newEvent.createEventView()

            electron1 = evtView.createParticle()
            electron1.setP4(40., - 20., 31., 80.)
            electron1.setName("electron")
            electron1.setUserRecord("calo isolation", 0.6)
            if (i == 0):
                print electron1
            electron2 = evtView.createParticle()
            electron2.setP4(19., 41., 10., 70.)
            electron2.setName("electron")
            electron2.setUserRecord("calo isolation", 0.3)
            if (i == 0):
                print electron2
            zboson = evtView.createParticle()
            zboson.setName("Z")
            zboson.linkDaughter(electron1)
            zboson.linkDaughter(electron2)
            zboson.setP4FromDaughters()
            if (i == 0):
                print zboson
            electron1.linkSoft(electron2, "we are both electrons")
            if (i == 0):
            
                print "Meanwhile: Demonstrate how to access Soft Relations with ObjectOwner."
                related = toParticle(electron2.getSoftRelations().getFirst(evtView.getObjectOwner()))
                if (related):
                    print "Name of related relative: "\
                            , related.getName() 
            
            if (i == 0):
                print "For each event, use the streamObject method to write the complete physics event to disk."
                        
            outFile.streamObject(newEvent)
        
        outFile.close()
    

    if True:
        print ""
        print "b) Reading the file containing pxl::Events" 
        print ""

        nEvents = 0

        inputFile=InputFile("pxlexample_1.pxlio")
        while (inputFile.good()):
        
            nEvents = inputFile.objectCount()

            event=inputFile.readNextObject()
            if not event:
                continue
            
            if (nEvents == 1):
                
                print event
                print "Get the contained EventViews, and then the Particles contained therein."
                
            eventViews = event.getObjectsOfType(EventView)

            for evView in eventViews:
                particles = evView.getObjectsOfType(Particle)
                for zboson in particles:
                    if (zboson.getMotherRelations().size() == 0 and nEvents == 1):
                        print zboson
                    
        inputFile.close()
                
        
        print "Processed" , nEvents , " events. " 
    

    if True:
        print ""
        print "c) Writing a file which contains arbitrary objects."
                
        print ""

        print "Create a OutputFile. " 
        outFile=OutputFile("pxlexample_2.pxlio")
        
        for i in range(100):

            if (i == 0):
                print "For each object, use the streamObject method to add it to the output stream."
                        

            electron=Particle()
            electron.setP4(50., 30., 40., 71.)

            vertex=Vertex()
            vertex.setX(0.3)
            vertex.setY(0.8)
            vertex.setZ(- 3.8)

            ev=EventView()
            ev.setUserRecord("likelihood", 0.7)

            outFile.streamObject(electron)
            outFile.streamObject(vertex)
            outFile.streamObject(ev)


        outFile.close()
        

    if True:
        print ""
        print "d) Reading a file which contains arbitrary objects."
                
        inFile=InputFile("pxlexample_2.pxlio")

        print "File good? ", inFile.good()

        obj = inFile.readNextObject()
        if (obj):
            print type(obj)
            print obj
        
        obj = inFile.seekToObject(20)
        if (obj):
            print type(obj)
            print obj

        obj = inFile.seekToObject(20)
        if (obj):
            print type(obj)
            print obj

        obj = inFile.readPreviousObject()
        if (obj):
            print type(obj)
            print obj
    

    if True:
        print ""
        print "e) Writing a file with InformationChunks."
                
        print ""

        print "Create a OutputFile. " 
        outFile=OutputFile("pxlexample_3.pxlio")

        info=InformationChunk()

        info.setName("W+Jets")
        info.setUserRecord("pt Cut", 70.)

        print "Write InformationChunk and finish the filesection, so it can be quickly read in."
        outFile.streamObject(info)
        outFile.writeFileSection()

        # event loop
        for i in range(100):
        
            ev1=EventView()
            ev2=EventView()
            ev1.setUserRecord("likelihood", 0.7)
            ev2.setUserRecord("likelihood", 0.3)

            outFile.streamObject(ev1)
            outFile.streamObject(ev2)
        outFile.close()
    

    if True:
        print ""
        print "f) Reading a file with InformationChunks."
                
        print ""

        print "Create a InputFile. " 
        inFile=InputFile("pxlexample_3.pxlio")

        print "Get the information chunk and print restored UserRecord."

        info=inFile.readNextObject()
        print info.getUserRecords()

        print "Read the rest of the file."

        inFile.close()
    

    print "\n"
    print "******** 7) Additional Functionality ********" 

    if True:
        print ""
        print "a) Filters and Sorting" 
        print ""

        print "Create EventView with three particles." 
        ev = EventView()

        gluon = ev.createIndexedParticle("gluon")
        gluon.setName("gluon")
        b = ev.createIndexedParticle("b")
        b.setName("b")
        b.setP4(20., 25., 0., 35.)
        bbar = ev.createIndexedParticle("bbar")
        bbar.setP4(100., 100., 0., 142.)
        bbar.setName("bbar")

        print "Filter & pt-sort particles." 
        filter=ParticleFilter()
        particles=ParticleVector()

        print "Choose a pt, eta, name filter criterion" 
        crit=ParticlePtEtaNameCriterion("gluon", 0., 0.)
        print "Apply filter. " 
        filter.apply(ev.getObjectOwner(), particles, crit)

        print "Print, sorted by pT: "
        for pa in particles:
            print "" , pa.getName(), "pT =" , pa.getPt()
        

        particles.clear()

        print "Take a pt filter criterion only. " 
        ptCrit=ParticlePtCriterion(50.)
        filter.apply(ev.getObjectOwner(), particles, ptCrit)

        print "Print all objects with pt bigger than 50, sorted by pT: "
                
        for pa in particles:
            print "" , pa.getName() , ", pT =" , pa.getPt()
        


    

    print "\n"
    print "******** 8) Technical Details ********" 

    if True:
        print ""
        print "a) UserRecord Copy-On-Write" 
        print ""

        ev = EventView()
        ev.setUserRecord("run nr.", 192168)

        print "Display UserRecord entry: "\
                , ev.getUserRecord("run nr.") 

        print "Map of original UserRecord resides at "\
                , ev.getUserRecords().getContainer() 

        ev_copy = EventView(ev)
        print "Map of copied event's UserRecord resides at "\
                , ev_copy.getUserRecords().getContainer() 

        ev_copy.setUserRecord("run nr.", 1921124)
        print "Display modified UserRecord entry: "\
                , ev_copy.getUserRecord("run nr.") 
        print "Map of copied event's modified UserRecord resides at "\
                , ev_copy.getUserRecords().getContainer() 

        print "Display original UserRecord entry: "\
                , ev.getUserRecord("run nr.") 

    

    print "\n"
    print "******** End of Example ********" 
    print "\n"
