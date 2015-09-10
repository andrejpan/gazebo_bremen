#include "visual_listener.hh"

using namespace  gazebo;


void VisualListener::Load(rendering::VisualPtr _parent, sdf::ElementPtr /*_sdf*/)
{
    std::cout << "Load up!\n";

    // Create our node for communication
    node = transport::NodePtr(new transport::Node());

    // Initialize the node with the world name
    node->Init();

    // Listen to Gazebo world_stats topic
    commandSubscriber = node->Subscribe("/gazebo/default/world_stats", &VisualListener::cb, this);
    std::cout << "exit Load\n";
}

void VisualListener::cb(ConstWorldStatisticsPtr &_msg)
{
  //std::cout << _msg->DebugString();
  std::cout << _msg->sim_time().sec() << '.' <<_msg->sim_time().nsec() << std::endl;
}

GZ_REGISTER_VISUAL_PLUGIN(VisualListener)



