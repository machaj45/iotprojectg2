# EPIC ULTIMATE MOTION SENSORIC BOARD
This is repo for iotprojecet under supervision of university of antwerpen


# Start clone IT!

```
git clone git@github.com:machaj45/iotproject.git
```

## This project is configured for working with klaxalk setup.


```
git clone https://github.com/klaxalk/linux-setup.git
```
Then read readme.md for how to install the setup

## Also you have to add those lines to you .i3/confige file

```
bindsym $mod2+h exec tmux send-keys -t jlink "h" ENTER
bindsym $mod2+g exec tmux send-keys -t jlink "g" ENTER
bindsym $mod2+f exec /home/machy/git/octa-stack-students/applications/iotproject/tmux/flash.sh
```
