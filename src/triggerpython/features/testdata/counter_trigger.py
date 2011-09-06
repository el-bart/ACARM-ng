##
# test class that performs counting of calls
class CounterPythonTrigger(triggerapi.BaseWrapper):
  ##
  # c-tor setting default values for start
  def __init__(self):
    triggerapi.BaseWrapper.__init__(self)
    ## calls counter
    self.counter=0
  ##
  # trigger action - increment counter
  def triggerImpl(self, ma):
    ++self.counter

# return instance to the C++ code
derived=CounterPythonTrigger()
