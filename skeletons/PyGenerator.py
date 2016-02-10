# -*- coding: utf-8 -*-

from pxl import core, modules


class Example(modules.PythonModule):

    def __init__(self):
        ''' Initialize private variables '''
        modules.PythonModule.__init__(self)

        self._counter = 0
        self._number = 0

    def initialize(self, module):
        ''' Initialize module options '''
        self.__module = module
        self._logger = core.Logger(self.__module.getName())

        module.addOption("number", "number of events to generate", 10)

    def beginJob(self, parameters=None):
        ''' Executed before the first object comes in '''
        self._logger.log(core.LOG_LEVEL_INFO, "Begin job")

        self._number = self.__module.getOption("number")

    def beginRun(self):
        ''' Executed before each run '''
        pass

    def generate(self):
        ''' Executed on every object '''
        if self._counter < self.__number:
            self._counter += 1
            self._logger.log(core.LOG_LEVEL_INFO, "Generating object %i / %i"
                             % (self._counter, self._number))
            return core.Event()

        # return None to stop generation
        return None

    def endRun(self):
        ''' Executed after each run '''
        pass

    def endJob(self):
        ''' Executed after the last object '''
        self._logger.log(core.LOG_LEVEL_INFO, "End job")
