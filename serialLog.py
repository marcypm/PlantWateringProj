import serial

ser = serial.Serial('/dev/ttyACM0',9600)

#TODO: parse serial readline to get sensor reading
#      then get timestamp and log both to spreadsheet in sambaShare

#TODO: allow for user to send commands to Arduino

while 1:

    if(ser.in_waiting > 0):
        line = ser.readline()
        print(line)
 
