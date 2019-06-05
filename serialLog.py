import serial
import signal
import logging
import re
from datetime import datetime

ser = serial.Serial('/dev/ttyACM0',9600)

#TODO: log to different files each month, right now setup for logging each day

#TODO: allow for user to send commands to Arduino
class InputTimedOut(Exception):
    pass

def inputTimeOutHandler(signum, frame):
    raise InputTimedOut

signal.signal(signal.SIGALRM, inputTimeOutHandler)

formatter = logging.Formatter('%(message)s')
currentDay = datetime.now().day

def checkInput(timeout=0.1):
    cmd = ""
    try:
            signal.setitimer(signal.ITIMER_REAL, timeout) #set timer to throw exception after timeout
            cmd = input()
            signal.setitimer(signal.ITIMER_REAL, 0)
    except InputTimedOut:
            pass
    return cmd

def setup_logger(name, log_file, level=logging.INFO):

    handler = logging.FileHandler(log_file)        
    handler.setFormatter(formatter)

    logger = logging.getLogger(name)
    logger.setLevel(level)
    logger.addHandler(handler)
    logger.info('date time data')

    return logger

logger = setup_logger(datetime.now().strftime('%Y-%m-%d'), datetime.now().strftime('%Y-%m-%d')+'.csv')

#TODO: display command to see all available commands
while 1:

    command = checkInput(timeout=0.1)
    if (command !=""):
        print ("sent: "+command) #send serially
        string_encode = command.encode()
        ser.write(string_encode)

    if(ser.in_waiting > 0):
        line = ser.readline()
        #read user input if there is any... (do not block on read)
        #send command via serial

            
        #check time to see if you need to write to a different file
        if(currentDay < datetime.now().day or (currentDay==30 and datetime.now().day == 1)):
            logger = setup_logger(datetime.now().strftime('%Y-%m-%d'), datetime.now().strftime('%Y-%m-%d')+'.csv')
            currentDay = datetime.now().day
            
        stringLine = re.split(" |\r",str(line,'utf-8')) #convert serial data from bytes to utf-8 & split string
        #stringLine = re.split(" |\r",str(line.encode('utf-8'))) #python2 equivalent
        print(stringLine)
        if(stringLine[1] == "data:"): #only log if it is data and not a command response
            logger.info(datetime.now().strftime('%Y-%m-%d %H:%M:%S ') + stringLine[2] +' '+ stringLine[3])
            #print(datetime.now().strftime('%Y-%m-%d %H:%M:%S ') + stringLine[2] +' '+ stringLine[3]);
            #print(line)
        else:
            print(line.decode("utf-8"))
                
        
            
 
