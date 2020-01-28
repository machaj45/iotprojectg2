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

## Also you have to add those lines to you .i3/confige file

```
bindsym $mod2+h exec tmux send-keys -t jlink "h" ENTER
bindsym $mod2+g exec tmux send-keys -t jlink "g" ENTER
bindsym $mod2+f exec ~/iotprojectg2/device/applications/iotproject/tmux/flash.sh
```

# Backend
This is our server side logic of our application

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
