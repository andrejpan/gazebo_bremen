#include "visual_listener.hh"

using namespace  gazebo;


void VisualListener::Load(rendering::VisualPtr _parent, sdf::ElementPtr /*_sdf*/)
{
    std::cout << "Load up!\n";

    this->ConnectToServer();

    this->listeningThread = new boost::thread(boost::bind(&VisualListener::ListenThread, this));

    this->updateFunction = event::Events::ConnectPreRender(boost::bind(&VisualListener::OnUpdate, this));

    this->manager = gui::get_active_camera()->GetScene()->GetManager();
    this->sceneNode = this->manager->getRootSceneNode()->createChildSceneNode("MySphere");
    this->entity = this->manager->createEntity("MySphere", Ogre::SceneManager::PT_SPHERE);
    this->entity->setMaterialName("Gazebo/Red");
    this->sceneNode->setVisible(true);
    this->entity->setVisible(true);
    this->sceneNode->setScale(0.001, 0.001, 0.001);
    this->sceneNode->setPosition(0, 0, 0);
    this->sceneNode->attachObject(entity);
    this->xI = 0;
    this->yI = 0;

    // tmp sphere for testing
    sceneNodeClosest = this->manager->getRootSceneNode()->createChildSceneNode("Closest");
    entityClosest = this->manager->createEntity("Closest", Ogre::SceneManager::PT_SPHERE);
    entityClosest->setMaterialName("Gazebo/Blue");
    sceneNodeClosest->setVisible(true);
    entityClosest->setVisible(true);
    sceneNodeClosest->setScale(0.001, 0.001, 0.001);
    sceneNodeClosest->setPosition(0, 0, 0);
    sceneNodeClosest->attachObject(entityClosest);

    // Create our node for communication
    node = transport::NodePtr(new transport::Node());
    // Initialize the node with the world name
    node->Init();
    // Listen to Gazebo world_stats topic
    commandSubscriber = node->Subscribe("/gazebo/default/world_stats", &VisualListener::cb, this);

    // TODO no idea how put this variable to haeader without errors
    mongo::Status status = mongo::client::initialize();
    if (!status.isOK())
       ::abort();
    try
    {
       c.connect("localhost");
       std::cout << "connected to mongodb server!" << std::endl;
    }
    catch( const mongo::DBException &e )
    {
       std::cout << "caught " << e.what() << std::endl;
    }

    std::cout << "Load done\n";
}

//////////////////////////////////////////////////
void VisualListener::cb(ConstWorldStatisticsPtr &_msg)
{
    //std::cout << _msg->DebugString();
    //std::cout << _msg->sim_time().sec() << '.' <<_msg->sim_time().nsec() << std::endl;
    this->sec = _msg->sim_time().sec();
    this->nsec = _msg->sim_time().nsec();
}

//////////////////////////////////////////////////
void VisualListener::getClosestObject()
{
    int len = 50;
    int points = 6;
    int angle = 0;
    int closest = -1;
    unsigned int closestDist = (unsigned int)-1;
    unsigned int tmpDist;
    for (int i = 0; i < points; i++ )
    {
        angle = ((i*360/points)*PI/180);
        this->coords2iTmp.x = this->coords2i.x + len * cos(angle);
        this->coords2iTmp.y = this->coords2i.y + len * sin(angle);
        if (this->mUserCam->GetScene()->GetFirstContact(this->mUserCam, this->coords2iTmp, this->contactPointTmp))
        {
            tmpDist = this->mUserCam->GetWorldPose().pos.Distance(this->contactPointTmp.x,this->contactPointTmp.y, this->contactPointTmp.z);
            //std::cout << tmpDist <<"\n";
            if (tmpDist < closestDist)
            {
                closestDist = tmpDist;
                closest = i;
            }
        }
    }
    angle = ((closest*360/points)*PI/180);
    this->coords2iTmp.x = this->coords2i.x + len * cos(angle);
    this->coords2iTmp.y = this->coords2i.y + len * sin(angle);
    this->mUserCam->GetScene()->GetFirstContact(this->mUserCam, this->coords2iTmp, this->contactPointTmp);
    this->sceneNodeClosest->setPosition(this->contactPointTmp.x, this->contactPointTmp.y, this->contactPointTmp.z);
}

//////////////////////////////////////////////////
void VisualListener::getClosestObjects()
{
    int len = 50;
    int points = 6;
    int angle = 0;
    double tmpDist;
    bool addAtEnd;
    std::vector< std::pair<double, std::string> > allwords_vp;
    for (int i = 0; i < points; i++ )
    {
        addAtEnd = true;
        angle = ((i*360/points)*PI/180);
        this->coords2iTmp.x = this->coords2i.x + len * cos(angle);
        this->coords2iTmp.y = this->coords2i.y + len * sin(angle);
        if (this->mUserCam->GetScene()->GetFirstContact(this->mUserCam, this->coords2iTmp, this->contactPointTmp))
        {
            tmpDist = this->mUserCam->GetWorldPose().pos.Distance(this->contactPointTmp.x,this->contactPointTmp.y, this->contactPointTmp.z);
            try // TODO we should fix this somehow :/
            {
                this->VPtr = this->mUserCam->GetScene()->GetVisualAt(this->mUserCam, this->coords2iTmp);
            }
            catch(...)
            {
            }
            if (this->VPtr != NULL && allwords_vp.empty())
            {
                allwords_vp.push_back(std::make_pair(tmpDist, this->VPtr->GetName()));
                continue;
            }
            if(this->VPtr != NULL)
            {
                for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
                {
                    if ( pos->second == this->VPtr->GetName() )
                    {
                        if (pos->first < tmpDist)
                            pos->first = tmpDist;

                        addAtEnd = false;
                    }
                }
                if (addAtEnd)
                {
                    allwords_vp.push_back(std::make_pair(tmpDist, this->VPtr->GetName()));
                }
            }
        }
    }

    std::sort(allwords_vp.begin(), allwords_vp.end(), sort_pred);


    if (allwords_vp.size() > 1)
    {
        for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
            std::cout << pos->first << " " << pos->second << "\n";
        std::cout << "\n\n";
    }
}

//////////////////////////////////////////////////
bool VisualListener::sort_pred(const std::pair<double, std::string> &left, const std::pair<double, std::string> &right)
    {
    return left.first < right.first;
    }

//////////////////////////////////////////////////
void VisualListener::OnUpdate()
{
    this->mUserCam = gui::get_active_camera();

    this->coords2i.x = this->xI;
    this->coords2i.y = this->yI;

    getClosestObjects();


    if (this->mUserCam->GetScene()->GetFirstContact(this->mUserCam, this->coords2i, this->contactPoint))
    {
        this->sceneNode->setPosition(this->contactPoint.x, this->contactPoint.y, this->contactPoint.z);
        /*
        mongo::BSONObjBuilder b;
        mongo::BSONObj p;
        b.append("cord", BSON_ARRAY(this->contactPoint.x << this->contactPoint.y << this->contactPoint.z));
        b.append("time", BSON_ARRAY(this->sec << this->nsec));
        p = b.obj();
        c.insert("tutorial.persons", p);
        */ //std::cout << p << std::endl;

        try // TODO we should fix this somehow :/
        {
            suc_counter++ ;
            failed = true;
            this->VPtr = this->mUserCam->GetScene()->GetVisualAt(this->mUserCam, this->coords2i);
            if (this->VPtr != NULL)
            {
                //std::cout << this->VPtr->GetName() << "\n";
                VPtr != NULL;
            }
        }
        catch(...)
        {

        }
    }
    if (!failed)
        unsuc_counter++;
    failed = false;
    //std::cout << "Succes/Failed: " << suc_counter / (double)unsuc_counter << "\n";

}

//////////////////////////////////////////////////
void VisualListener::ListenThread()
{
    boost::posix_time::millisec workTime(1);
    while (1)
    {
        memset(buffer, 0, BufferLength);
        this->rc = read(this->sd, &buffer[0], BufferLength);
        if (this->rc < 0)
        {
            perror("Client-read() error");
            close(sd);
            exit(-1);
        }
        else if (this->rc == 0)
        {
            printf("Server program has issued a close()\n");
        }
        else
        {
            // lets be sure, that we recieve right data
            pch = strchr(this->buffer,' ');
            if(pch!=NULL)
            {
                xI = strtol(this->buffer, &pEnd, 10);
                yI = strtol(pEnd, NULL, 10);
                //printf("%d, %d\n", xI, yI);
            }
        }
        boost::this_thread::sleep(workTime);
    }
}

//////////////////////////////////////////////////
void VisualListener::ConnectToServer()
{

    if( (this->he=gethostbyname(SERVER)) == NULL )
    {
        perror("gethostbyname()");
        exit(1);
    }
    else
        printf("Client-The remote host is: %s\n", SERVER);

    if ((this->sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client-socket() error");
        exit(-1);
    }
    else
        printf("Client-socket() OK\n");

    this->serveraddr.sin_family = AF_INET;
    this->serveraddr.sin_port = htons(SERVPORT);
    this->serveraddr.sin_addr = *((struct in_addr *) this->he->h_addr);

    // zero the rest of the struct
    memset(&(this->serveraddr.sin_zero), '\0', 8);

    if(connect(this->sd, (struct sockaddr *)&this->serveraddr, sizeof(this->serveraddr)) == -1)
    {
        perror("connect()");
        close(this->sd);
        exit(1);
    }
    else
    {
        printf("Client is connected to Win server\n");
    }
}

GZ_REGISTER_VISUAL_PLUGIN(VisualListener)



