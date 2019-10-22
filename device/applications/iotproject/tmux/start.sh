#!/bin/bash
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`
cd $MY_PATH
rm .tmuxinator.yml
ln session.yml .tmuxinator.yml
tmuxinator
