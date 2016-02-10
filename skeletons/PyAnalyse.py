# -*- coding: utf-8 -*-

from pxl import core, modules


class Example(modules.PythonModule):

    def __init__(self):
        ''' Initialize private variables '''
        modules.PythonModule.__init__(self)
        # self._exampleVariable = startValue

    def initialize(self, module):
        ''' Initialize module options '''
        self.__module = module
        self._logger = core.Logger(self.__module.getName())
        # module.addOption("example option", "example description", "default value")

    def beginJob(self, parameters=None):
        ''' Executed before the first object comes in '''
        self._logger.log(core.LOG_LEVEL_INFO, "Begin job")

    def beginRun(self):
        ''' Executed before each run '''
        pass

    def analyse(self, object):
        ''' Executed on every object '''
        # event = core.toEvent(object)

    def endRun(self):
        ''' Executed after each run '''
        pass

    def endJob(self):
        ''' Executed after the last object '''
        self._logger.log(core.LOG_LEVEL_INFO, "End job")
