#include "word_listener.hh"

namespace gazebo
{
    class WorldListener : public WorldPlugin
    {
        WorldListener() : WorldPlugin()
        {
            std::cout << "Constructor up!\n");
        }

        void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
        {
            std::cout << "Load up!\n");
        }
    };
    GZ_REGISTER_WORLD_PLUGIN(WorldListener)
}
