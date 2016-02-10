#
# Purpose: count the total number of processed events
#         
# Author   : Martin Erdmann
# Date     : 5-Jul-2008
#
# Modified : Andreas Hinzmann
# Date     : 30-Jun-2009
#
# Modified : Robert Fischer
# Date     : 23-Jun-2010

import time
from pxl import modules, core
logger = core.Logger("Count Script")
LOG_LEVEL_DEBUG = core.LOG_LEVEL_DEBUG

logger(LOG_LEVEL_DEBUG, "Loading File")
class Counter(modules.PythonModule):
    #----------------------------------------------------------------------
    def initialize(self, module):
        logger(LOG_LEVEL_DEBUG, "initialize")
        pass

    #----------------------------------------------------------------------
    def beginJob(self, parameters=None):
        logger(LOG_LEVEL_DEBUG, "beginJob")
        self._count = 0
        self._next_print_count = 1
        self._next_print_time = 0
        self._begin_time = 0

        print ("*** start event counting ***")
        self._begin_time=time.time()

    #----------------------------------------------------------------------
    def analyse(self, event):
        logger(LOG_LEVEL_DEBUG, "analyse")
        # increment by one for each event
        self._count += 1

        # print counter
        if self._count > self._next_print_count or time.time() > self._next_print_time:
            if self._count < 5:
                self._next_print_count = self._count
            elif str(self._count)[0] == "1":
                self._next_print_count = self._count * 5 - 1
            elif str(self._count)[0] == "5":
                self._next_print_count = self._count * 2 - 1
            self._next_print_time = time.time() + 60
            print ("processing event", self._count, "...")

    #----------------------------------------------------------------------
    def endJob(self):
        logger(LOG_LEVEL_DEBUG, "endJob")
        total_time=time.time()-self._begin_time
        if total_time < 0.0000001:
            total_time = 0.0000001
        print ("*** event counting finished.", self._count, "events processed in",int(total_time/60/60),"h,",int(total_time/60%60),"m,",int(total_time%60),"s. (",int(self._count/total_time),"events/s )")

logger(LOG_LEVEL_DEBUG, "loading finished")
