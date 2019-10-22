tmux has-session -t jlink || ~/git/iotproject/applications/iotproject/tmux/start.sh
tmux send-keys -t jlink "h" ENTER
tmux send-keys -t jlink "loadbin /home/machy/git/iotproject/applications/iotproject/build/iotproject.bin,0x08000000" ENTER
tmux send-keys -t jlink "r" ENTER
tmux send-keys -t jlink "g" ENTER
