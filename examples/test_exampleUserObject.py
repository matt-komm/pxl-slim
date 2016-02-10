
from pxl import core
import ExampleUserObject

core.PluginManager_instance().loadPluginsFromDirectory('.')
factory = core.ObjectFactory_instance()

myObject = factory.create(core.Id('074fd3f9-fbf3-418e-9b33-1459bbd65e77'))
print myObject 

myObject = ExampleUserObject.toExampleUserObject(myObject)
myObject.testMethod()

b = core.BasicContainer()
b.insertObject(myObject)

# This does not work, as  the ExampleUserObject is not know to pxl
#b.getObjectsOfType(ExampleUserObject)

o = b.getObjects()
l = [ExampleUserObject.toExampleUserObject(f) for f in o if f.getTypeId() == ExampleUserObject.ExampleUserObject_getStaticTypeId()]

print len(l)
