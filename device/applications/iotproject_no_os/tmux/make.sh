tmux has-session -t jlink || ~/git/iotprojectg2/device/applications/iotproject/tmux/start.sh
tmux send-keys -t jlink:jlink.left "make" ENTER

