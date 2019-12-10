tmux has-session -t jlink || ~/git/iotprojectg2/device/applications/iotproject/tmux/start.sh
tmux send-keys -t jlink:jlink.right "h" ENTER
tmux send-keys -t jlink:jlink.right "loadbin /home/machy/git/iotprojectg2/device/applications/no-scheduler-example/build/no-scheduler-example.bin,0x08000000" ENTER
#tmux send-keys -t jlink:jlink.left "loadbin /home/machy/git/iotprojectg2/device/applications/murata-dualstack-example-ns/build/murata-dualstack-example-ns.bin,0x08000000" ENTER
tmux send-keys -t jlink:jlink.right "r" ENTER
tmux send-keys -t jlink:jlink.right "g" ENTER
