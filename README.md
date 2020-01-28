# Enviroment Warner
This is semestral work for IOT. That can save your live. In this repository you can found code for device it self and code for server side and also code for android app that helps with seting up the device.


# How to clone this repository?

```
git clone git@github.com:machaj45/iotprojectg2.git
```

## This project is configured for working with klaxalk setup.

```
git clone https://github.com/klaxalk/linux-setup.git
```
Then read readme.md for how to install the setup

## You can also use Visula studio
Setup for Visual studio is in repository it self.

## How to compile code 
Run:
```
cd {git_repo}/device/application/no-schedguler-example
make
```
## How to programm the device
```
jlinkexe -device STM32L496ZG -if SWD -speed 4000 -autoconnect 1
h
loadbin {git_repo}/device/applications/no-scheduler-example/build/no-scheduler-example.bin,0x08000000" 
r 
g 
```
# Backend
This is server side logic of our application

## How to run it?

Run with python:
```
python {git_repo}/backend/main_service/main_service.py
```

## Measurements

You can see last measurements done with the main_service script
```
cat {git_repo}/backend/main_service/measurements.json
```

# More information

## Final presentation
Go to:
```
{git_repo}/presentation/final_presentation.pdf
```

## Power measurements
Go to:
```
{git_repo}/documentation/power measurements
```
