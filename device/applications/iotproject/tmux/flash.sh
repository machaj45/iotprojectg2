tmux has-session -t jlink || ~/git/iotprojectg2/device/applications/iotproject/tmux/start.sh
tmux send-keys -t jlink:jlink.left "h" ENTER
#tmux send-keys -t jlink:jlink.left "loadbin /home/machy/git/iotprojectg2/device/applications/iotproject/build/iotproject.bin,0x08000000" ENTER
tmux send-keys -t jlink:jlink.left "loadbin /home/machy/git/iotprojectg2/device/applications/murata-dualstack-example-ns/build/murata-dualstack-example-ns.bin,0x08000000" ENTER
tmux send-keys -t jlink:jlink.left "r" ENTER
tmux send-keys -t jlink:jlink.left "g" ENTER
