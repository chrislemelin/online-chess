#!/bin/bash



tmux new -d -A -s my-session
tmux split-window -h 
tmux split-window -v -t my-session:0.1
tmux send -t my-session:0.0 "./server/server 5000" Enter
tmux send -t my-session:0.1 "./client/client localhost 5000" Enter "chris" Enter
tmux send -t my-session:0.2 "./client/client localhost 5000" Enter "bob" Enter
tmux attach -t my-session

