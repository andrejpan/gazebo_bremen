#ifndef _EYE_TRACKER_HH
#define _EYE_TRACKER_HH

#include <gazebo/gazebo.hh>
#include <gazebo/rendering/rendering.hh>
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/DynamicLines.hh"
#include "gazebo/rendering/RenderTypes.hh"
#include <gazebo/physics/physics.hh>

#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>

#include <mongo/client/dbclient.h>
#include "mongo/client/init.h"

#include <gazebo/gui/GuiIface.hh>

#include <iostream>
#include <vector>
#include <algorithm>

// max size of receving buffer
#define BUFFERLENGTH 25

// server properties
#define SERVER "192.168.101.217"
#define SERVPORT 1234

#define PI 3.14159265

//spheres scale
#define SPHERE_SCALE_FACTOR 0.0001

// size of circe which are we looking, it depends where is camera position
#define LEN 40

// how many times at circle we want to look, equal distnaces and angles
#define NUMBER_OF_POINTS 6

//how many other elements we want to see from the clossest object
#define PROCENTS 10

//screen resolution, importing X11 crash mongodb_26copmat
static const int SCREEN_RESOLUTION [] = {1920, 1080};

//colors for spheres
const char *colorNames[] = {"Gazebo/Grey", "Gazebo/Red", "Gazebo/Green", "Gazebo/Blue", "Gazebo/Yellow", "Gazebo/Purple",
                            "Gazebo/Orange", "Gazebo/Turquoise", "Gazebo/DarkGrey", "Gazebo/White", "material Gazebo/FlatBlack",
                           "Gazebo/SkyBlue", "Gazebo/ZincYellow", "Gazebo/Indigo"};

namespace gazebo
{
    class EyeTracker : public VisualPlugin
    {
        /// \brief Socket descriptor for client.
        private: int sd;

        /// \brief Number of received bytes from server.
        private: int rc;

        /// \brief Received bytes from server.
        private: char buffer[];

        /// \brief Data for server.
        private: struct sockaddr_in serveraddr;

        /// \brief Structure of type hostent for connecting to server.
        private: struct hostent *he;

        /// \brief Thread recive data from server.
        private: boost::thread *listeningThread;

        /// \brief Pointer to the function OnUpdate().
        private: event::ConnectionPtr updateFunction;

        /// \brief Pointer to the function OnUpdate().
        private: event::ConnectionPtr updateFunction1;

        /// \brief Camera pointer.
        private: rendering::UserCameraPtr mUserCam;

        /// \brief Values for (x,y) point on screen.
        private: int xI, yI;

        /// \brief Gives information about received string.
        private: char * pch;

        /// \brief Pointer to the next number in string.
        private: char * pEnd;

        /// \brief  //apparently I am not ussing it anymore
        private: rendering::VisualPtr VPtr;

        /// \brief Srandard Ogre scene manager.
        private: Ogre::SceneManager *manager;

        /// \brief Standard Ogre scene node.
        //private: Ogre::SceneNode *sceneNode;
        private: Ogre::SceneNode *sceneNodeClosest;

        /// \brief Standard Ogre entity.
        //private: Ogre::Entity *entity;
        private: Ogre::Entity *entityClosest;

        /// \brief Minimal fixation data from sensor.
        private: math::Vector2i coords2i;
        private: math::Vector2i coords2iTmp;

        /// \brief Point in space where we look.
        private: math::Vector3 contactPoint;

        /// \brief Resizing the view.
        private: math::Vector3 contactPointTmp;

        /// \brief Simulation time.
        private: int32_t sec, nsec;

        /// \brief Angle of the circle where we look.
        private: int angle;

        /// \brief Distance between camera and objects.
        private: double tmpDist;

        /// \brief Using for adding uniq object to vector.
        private: bool addAtEnd;

        /// \brief Ogre sphere names.
        private: std::string tmpString;

        /// \brief We must not have spheres with same name.
        private: bool sphereAttached;

        /// \brief Pointer to the visual
        public: rendering::VisualPtr _parent;

        /// \brief Node for communication.
        public: transport::NodePtr transportNode;

        /// \brief Listening topic where we get simulation time.
        public: transport::SubscriberPtr commandSubscriber;

        /// \brief Connection to the mongodb server.
        mongo::DBClientConnection c;

        /// \brief Load.
        public: void Load(rendering::VisualPtr _parent, sdf::ElementPtr _sdf);

        /// \brief Callback function for listening topic.
        public: void cb(ConstTimePtr &_msg);

        /// \brief Logi data 2 times slower then we show it
        private: bool logData;

        //// \brief TODO
        private: void getClosestObjects();

        //// \brief Propeties of single object.
        struct myObject {
            double distance;
            std::string objectName;
            math::Vector3 contactPoint;
            int colorNumber;
            bool seen;

            // for sorting purpose
            bool operator<(const myObject& a) const
            {
                return distance < a.distance;
            }
        };

        //// \brief Vector of objects that we detect.
        std::vector< myObject > allwords_vp;

        //// \brief Adding uniq color to objects when calling getClosestObjects()
        int colorCounter = 0;

        //// \brief Calling getClosestObjects() when we get message from topic
        private:bool callClossest = false;

        /// \brief Getting data from server in thread.
        private: void ListenThread();

        /// \brief TODO.
        public: void OnUpdate();

        /// \brief Connection to the server where we get eye tracker data.
        private: void ConnectToServer();

    };
}
#endif
