import serial
import logging
import re
from datetime import datetime

ser = serial.Serial('/dev/ttyACM0',9600)

#TODO: log to different files each month, right now setup for logging each day

#TODO: allow for user to send commands to Arduino

formatter = logging.Formatter('%(message)s')
currentDay = datetime.now().day

def setup_logger(name, log_file, level=logging.INFO):

    handler = logging.FileHandler(log_file)        
    handler.setFormatter(formatter)

    logger = logging.getLogger(name)
    logger.setLevel(level)
    logger.addHandler(handler)
    logger.info('date time data')

    return logger

logger = setup_logger(datetime.now().strftime('%Y-%m-%d'), datetime.now().strftime('%Y-%m-%d')+'.csv')


while 1:

    if(ser.in_waiting > 0):
        line = ser.readline()
        #read user input if there is any... (do not block on read)
        #send command via serial
        
        #check time to see if you need to write to a different file
        if(currentDay < datetime.now().day or (currentDay==30 and datetime.now().day == 1)):
            logger = setup_logger(datetime.now().strftime('%Y-%m-%d'), datetime.now().strftime('%Y-%m-%d')+'.csv')
            currentDay = datetime.now().day
            
        stringLine = re.split(" |\r",str(line, 'utf-8')) #convert serial data from bytes to utf-8 & split string
        print(stringLine)
        if(stringLine[1] == "data:"): #only log if it is data and not a command response
            logger.info(datetime.now().strftime('%Y-%m-%d %H:%M:%S ') + stringLine[2] +' '+ stringLine[3])
            print(datetime.now().strftime('%Y-%m-%d %H:%M:%S ') + stringLine[2] +' '+ stringLine[3]);
            #print(line)
 
