tmux has-session -t jlink || ~/git/iotprojectg2/device/applications/iotproject/tmux/start.sh
tmux send-keys -t jlink "h" ENTER
tmux send-keys -t jlink "loadbin /home/machy/git/iotprojectg2/device/applications/iotproject/build/iotproject.bin,0x08000000" ENTER
tmux send-keys -t jlink "r" ENTER
tmux send-keys -t jlink "g" ENTER
