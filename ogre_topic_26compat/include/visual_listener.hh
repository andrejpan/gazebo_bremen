#ifndef _VISUAL_LISTENER_HH
#define _VISUAL_LISTENER_HH

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

#define BufferLength 25
#define SERVER "192.168.101.210"
#define SERVPORT 1234

#define PI 3.14159265

namespace gazebo
{
    class VisualListener : public VisualPlugin
    {
        /// \brief Socket descriptor for client.
        private: int sd;

        /// \brief Number of received bytes from server.
        private: int rc;

        /// \brief Received bytes from server.
        private: char buffer[];

        /// \brief Data for server.
        private: struct sockaddr_in serveraddr;

        /// \brief Server ip address.
        private: struct hostent *he;

        /// \brief Thread recive data from server.
        private: boost::thread *listeningThread;

        /// \brief Pointer to the function OnUpdate().
        private: event::ConnectionPtr updateFunction;

        /// \brief Pointer to the function OnUpdate().
        private: event::ConnectionPtr updateFunction1;

        /// \brief Camera pointer.
        private: rendering::UserCameraPtr mUserCam;

        /// \brief Int values for (x,y) point on screen.
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
        private: Ogre::SceneNode *sceneNode;

        /// \brief Standard Ogre entity.
        private: Ogre::Entity *entity;

        // tmp sphere for testing
        /// \brief Standard Ogre scene node.
        private: Ogre::SceneNode *sceneNodeClosest;

        /// \brief Standard Ogre entity.
        private: Ogre::Entity *entityClosest;

        /// \brief Minimal fixation data from sensor.
        private: math::Vector2i coords2i;
        private: math::Vector2i coords2iTmp;

        /// \brief Point in space where we look.
        private: math::Vector3 contactPoint;

        /// \brief Resizing the view.
        private: math::Vector3 contactPointTmp;

        /// \brief Simulation time.
        private: int sec, nsec;

        public: rendering::VisualPtr _parent;
        public: transport::NodePtr node;
        public: transport::SubscriberPtr commandSubscriber;

        /// \brief Connection to the mongodb server.
        mongo::DBClientConnection c;

        /// \brief Load.
        public: void Load(rendering::VisualPtr _parent, sdf::ElementPtr _sdf);

        /// \brief Callback function for listening
        public: void cb(ConstWorldStatisticsPtr &_msg);

        //// \brief TODO not sure what it is for
        private: void getClosestObject();

        //// \brief TODO not sure what it is for
        private: void getClosestObjects();

        /// \brief Connection to the server.
        private: void ConnectToServer();

        /// \brief Getting data from server in thread.
        private: void ListenThread();

        /// \brief TODO.
        public: void OnUpdate();

    private: bool sort_pred(const std::pair<double, std::string> &left, const std::pair<double, std::string> &right);

        /// \brief Calculating if we get an object back or we get an error
        int suc_counter = 1;
        int unsuc_counter = 1;
        bool failed = false;
    };

}
#endif
