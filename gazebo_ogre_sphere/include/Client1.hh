#ifndef CLIENT1_H
#define CLIENT1_H

#include <gazebo/gazebo.hh>
#include <gazebo/rendering/rendering.hh>
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/DynamicLines.hh"
#include "gazebo/rendering/RenderTypes.hh"

#include "gazebo/physics/physics.hh"
#include "gazebo/transport/TransportTypes.hh"
#include "gazebo/msgs/MessageTypes.hh"

#include "gazebo/common/Time.hh"
#include "gazebo/common/Plugin.hh"
#include "gazebo/common/Events.hh"

#include <gazebo/gui/GuiIface.hh>

#include <string.h>
#include <stdlib.h>     /* strtol */

#define BufferLength 25
#define SERVER "192.168.101.157"
#define SERVPORT 1234

namespace gazebo
{
/// \Class Client1 Client1.hh
    class Client1 : public VisualPlugin
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

        /// \brief Camera pointer.
        private: rendering::UserCameraPtr mUserCam;

        /// \brief Pointer to the function OnUpdate().
        private: event::ConnectionPtr updateFunction;

        /// \brief Standard Ogre scene node.
        private: Ogre::SceneNode *sceneNode;

        /// \brief Standard Ogre entity.
        private: Ogre::Entity *entity;

        /// \brief Srandard Ogre scene manager.
        private: Ogre::SceneManager *manager;

        /// \brief Float values for (x,y) point on screen.
        private: float x,y;

        /// \brief Int values for (x,y) point on screen.
        private: int xI, yI;

        /// \brief Gives information about received string.
        private: char * pch;

        /// \brief Pointer to the next number in string.
        private: char * pEnd;

        /// \brief Load.
        public: void Load(rendering::VisualPtr _parent, sdf::ElementPtr /*_sdf*/);

        /// \brief OnUpdate.
        public: void OnUpdate();

        /// \brief Connection to the server.
        private: void ConnectToServer();

        /// \brief Getting data from server in thread.
        private: void ListenThread();

    };
}
#endif // CLIENT1_H
