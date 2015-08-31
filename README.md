# gazebo_bremen

## Before runing this code you should run set up [Tobii EyeX Traceker](https://github.com/andrejpan/toobi_eyex_tracker_server)

## Project: [gazebo_ogre_sphere](https://github.com/andrejpan/gazebo_bremen/tree/master/gazebo_ogre_sphere)

Program draws red sphere inside XY plane. The sphere is drawn at same position where we are looking a screen.

## Usage
```
$ mkdir build && cd build
$ cmake ..
$ make
```
## Run
```
$ gazebo -u --verbose worlds/special_kamera_view.world
```
![alt tag](https://github.com/andrejpan/gazebo_bremen/blob/master/pic/red_sphere.png)
