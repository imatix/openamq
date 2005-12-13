"""
Bridges between Python and Java, to provide a simpler and more Pythonesque environment.
Certified to be free of dead parrots.
@author Robert Greig
Copyright (c) 2005 JP Morgan Chase
"""
# Imports
from java.lang import *
import org.openamq.management.jmx.AMQConsole as AMQConsole
import org.openamq.stac.jmx.MBeanServerConnectionContext as MBeanServerConnectionContext

# Globals
commandPrompt = ""
proxied = 0
connected = 0
amqConsole = None
connectionContext = None

# Functions
def connect(url="", username="", password=""):
  """
  Connects to an OpenAMQ broker. The URL must be in the form amq://host:port/context
  """
  try:
    global connected
    global connectionContext
    if connected==1:
      print "Already Connected!"
      return
    parsedURL = parseURL(url)
    amqConsole = AMQConsole(parsedURL['host'], parsedURL['port'], username, password, parsedURL['context'])
    amqConsole.initialise()
    amqConsole.registerAllMBeans()
    connectionContext = MBeanServerConnectionContext()
    connectionContext.connect()
    connected = 1
  except Exception, e:
    updateGlobals()
    msg = "Error: " + e.getMessage()
    print msg
  else:
    updateGlobals();

def ls():
    """
    Lists the current mbean
    """
    global connected
    if connected == 0:
        print "Not connected!"
        return

    connectionContext.ls()

def cd(beanName):
    """
    Changes the current mbean
    """
    global connected
    if connected == 0:
        print "Not connected!"
        return

    try:
        connectionContext.cd(beanName)
    except Exception, e:
        updateGlobals()
        msg = "Error: " + e.getMessage()
        print msg
    else:
        updateGlobals();
    
def parseURL(url):
    """
    Parses an AMQ URL into host, port and context components returning them in a dictionary
    """
    return {'host':'localhost','port':7654,'context':'test'}

def updateGlobals():
    global commandPrompt
    global connectionContext
    if connected == 1:
        commandPrompt = "AMQ:connected#" + connectionContext.getCurrentMBean().getMBeanInfo().getClassName() + "> "
    else:
        commandPrompt = "AMQ:disconnected> "
# Classes


# Global execution

# Update all the global variables - this is called to sync everything at the start
updateGlobals()