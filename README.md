# gazebo_bremen

## Before runing projects below you must run set up [Tobii EyeX Traceker](https://github.com/andrejpan/toobi_eyex_tracker_server)

## Project: [gazebo_eye_tracker](https://github.com/andrejpan/gazebo_bremen/tree/master/gazebo_eye_tracker)

Program detects which objects are you looking inside Gazebo simulator and logs it.

## Prerequisites
For logging purpose we use Mongodb tehnology. On Ubuntu you can install server through package manager. For client side we use [mongodb-cxx-client](https://github.com/mongodb/mongo-cxx-driver), 26compat branch. Our installed instructure looks like this: 
```
$ scons --prefix=$HOME/mongo-client-install-26compat --sharedclient --full --use-system-boost install-mongoclient
```
Update path to mongodb-cxx-client inside [CMakeLists.txt](https://github.com/andrejpan/gazebo_bremen/blob/master/gazebo_eye_tracker/CMakeLists.txt#L6).

If you want to run [kitchen.world](https://github.com/andrejpan/gazebo_bremen/blob/master/gazebo_eye_tracker/worlds/kitchen.world) you must download [models](https://bitbucket.org/dobracristian/gz_models) and update GAZEBO_MODEL_PATH.

Inside [EyeTracker.hh](https://github.com/andrejpan/gazebo_bremen/blob/master/gazebo_eye_tracker/include/EyeTracker.hh#L29) you must set up server ip and port number. 

## Usage
```
$ mkdir build && cd build
$ cmake ..
$ make
```
## Run
```
$ gazebo -u --verbose worlds/kitchen.world
```
After you start simulation, you will see small spheres which detects objects that you currently looking. 
![alt tag](https://github.com/andrejpan/gazebo_bremen/blob/master/pic/objects.png)

Data is also logged when simulation is running.
![alt tag](https://github.com/andrejpan/gazebo_bremen/blob/master/pic/mongodb-entry.png)

## Project [eye_tracker_legacy](https://github.com/andrejpan/gazebo_bremen/tree/master/eye_tracker_legacy)
Is the same project as one above, but it is ussing Mongodb legacy driver.

Compiling instruction:
```
$ scons --prefix=$HOME/mongo-client-install --sharedclient --c++11=on install 
```

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

## Project: [ see_first_object](https://github.com/andrejpan/gazebo_bremen/tree/master/see_first_object)

Program draws red sphere at the object which it is seen first.

## Usage
```
$ mkdir build && cd build
$ cmake ..
$ make
```
## Run
```
$ gazebo -u --verbose worlds/client1.world
```
![alt tag](https://github.com/andrejpan/gazebo_bremen/blob/master/pic/first_object.png)
