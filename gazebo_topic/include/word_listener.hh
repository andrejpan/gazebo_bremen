#ifndef _WORD_LISTENER_HH
#define _WORD_LISTENER_HH

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>

namespace gazebo
{
    class WorldListener : public WorldPlugin
    {
        public: WorldListener() : WorldPlugin();

        public: void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf);
    };
}
#endif