#!/bin/bash
cp -Rd /home/amir/dev/wooden/bin/* /home/amir/dev/KallaBalla/deploy/
cp -Rd /home/amir/dev/KallaBalla/bin/* /home/amir/dev/KallaBalla/deploy/
PWD=`pwd`
cd /home/amir/dev/KallaBalla/deploy/
jar cf /tmp/kallaBalla.jar -C /home/amir/dev/KallaBalla/deploy com DisplayModeModel.class DisplayModeTest.class DisplayModeTest.java gnu javax JfcExample.class JfcExample.java junit META-INF org stylesheet.css
cp /tmp/kallaBalla.jar /var/www/kallaBalla.jar
cd $PWD
