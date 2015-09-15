#include "word_listener.hh"

using namespace  gazebo;


void WorldListener::Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
{
    std::cout << "Load up!\n";

    // Create our node for communication
    node = transport::NodePtr(new transport::Node());

    // Initialize the node with the world name
     node->Init(_world->GetName());

    // Listen to Gazebo world_stats topic
    commandSubscriber = node->Subscribe("~/world_stats", &WorldListener::cb, this);
}

void WorldListener::cb(ConstWorldStatisticsPtr &_msg)
{
  //std::cout << _msg->DebugString();
  std::cout << _msg->sim_time().sec() << '.' <<_msg->sim_time().nsec() << std::endl;
}

GZ_REGISTER_WORLD_PLUGIN(WorldListener)



