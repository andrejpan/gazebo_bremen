#ifndef _WORD_LISTENER_HH
#define _WORD_LISTENER_HH

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo_client.hh>

namespace gazebo
{
    class WorldListener : public WorldPlugin
    {
        public: transport::NodePtr node;
        public: transport::SubscriberPtr commandSubscriber;

        public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf);

        public: void cb(ConstWorldStatisticsPtr &_msg);
    };

}
#endif
