Mp = float(input("Mass of Platform: ")) #User input of platform mass
Mo = float(input("Mass of Object: ")) #User input of Object

Dp = float(input("Distance of Platform to pivot: ")) #User input of distance from platform to pivot
Do = float(input("Distance of Object to pivot: ")) #User input of distance from object to pivot

Op = float(input("Amount of Oscillations for Platform only test: ")) #User input of Osillations
Oo = float(input("Amount of Oscillations for Platform and Object test: ")) #User input of Osillations

Tp = float(input("Time taken for Platform only test (Average): ")) #Time taken for Platform only test
To = float(input("Time taken for Platform and Object test (Average): ")) #Time taken for Platform and Object test

TimePeriod_p = Tp/Op #Time Period of Platform only test
TimePeriod_o = To/Oo #Time Period of Platform and Object test

g = 9.81 #Gravity
pi = 3.14 #Pi

Jp = (g*((Mp*Dp)*TimePeriod_p**2))/(4*(pi**2)) #Formula for Platform Intertia
Jo = (g*((Mo*Do)+(Mp*Dp))*TimePeriod_o**2)/((4*pi**2) - (Mo*Do**2) - Jp) #Formula for Object Intertia

print("Object Inertia: " + str(Jo) + " kg*m^2")
