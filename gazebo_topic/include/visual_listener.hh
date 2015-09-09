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

namespace gazebo
{
    class VisualListener : public VisualPlugin
    {
        public: physics::WorldPtr world;
        public: rendering::VisualPtr _parent;
        public: transport::NodePtr node;
        public: transport::SubscriberPtr commandSubscriber;

        public: void Load(rendering::VisualPtr _parent, sdf::ElementPtr _sdf);

        public: void cb(ConstWorldStatisticsPtr &_msg);
    };

}
#endif
