#!/bin/bash


tmux kill-session -t my-session

tmux new -d -A -s my-session
tmux split-window -h
tmux split-window -v -t my-session:0.1
tmux send -t my-session:0.0 "./server/server 5001" Enter
sleep 1
tmux send -t my-session:0.1 "./client/client localhost 5001" Enter
tmux send -t my-session:0.1 "chris" Enter
tmux send -t my-session:0.2 "./client/client localhost 5001" Enter 
tmux send -t my-session:0.2 "bob" Enter
tmux attach -t my-session
