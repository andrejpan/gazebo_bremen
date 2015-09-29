#include "EyeTracker.hh"

using namespace  gazebo;

void EyeTracker::Load(rendering::VisualPtr _parent, sdf::ElementPtr /*_sdf*/)
{
    std::cout << "Load up!\n";

    // connect to the server
    this->ConnectToServer();

    //grab data from eye tracker server
    this->listeningThread = new boost::thread(boost::bind(&EyeTracker::ListenThread, this));

    //get objects after every REFTIME(defined in WorldTimePublisher.hh)
    this->updateFunction = event::Events::ConnectPreRender(boost::bind(&EyeTracker::OnUpdate, this));

    this->mUserCam = gui::get_active_camera();

    this->manager = gui::get_active_camera()->GetScene()->GetManager();

    //initial postion of view
    this->xI = 0;
    this->yI = 0;

    // Create our node for communication
    this->transportNode = transport::NodePtr(new transport::Node());
    // Initialize the node with the world name
    this->transportNode->Init();
    // Listen to Gazebo world_stats topic
    commandSubscriber = this->transportNode->Subscribe("/gazebo/EmptyWorld/WorldTime_topic", &EyeTracker::cb, this);

    // TODO no idea how put variable status to haeader without errors
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
void EyeTracker::cb(ConstTimePtr &_msg)
{
    //std::cout << _msg->DebugString();
    this->sec = _msg->sec();
    this->nsec = _msg->nsec();
    //std::cout << "GOT: " << this->sec << ".";
    //std::cout << std::setfill('0') << std::setw(9) << this->nsec << "\n";
    callClossest = true;
}

//////////////////////////////////////////////////
void EyeTracker::getClosestObjects()
{
    myObject tmpObj;
    colorCounter = 0;


    //before we detect new objects we hide spheres which represent object until now
    for( auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++ )
    {
        pos->seen = false;
        pos->distance = DBL_MAX;
        pos->colorNumber = -1;
        //TODO hide spheres
    }

    for ( int i = 0; i < NUMBER_OF_POINTS; i++ )
    {
        this->angle = ((i*360/NUMBER_OF_POINTS)*PI/180);
        this->coords2iTmp.x = this->coords2i.x + LEN * cos(this->angle);
        this->coords2iTmp.y = this->coords2i.y + LEN * sin(this->angle);

        // trying to get 3d point of object that we look
        if (this->mUserCam->GetScene()->GetFirstContact(this->mUserCam, this->coords2iTmp, this->contactPointTmp))
        {
            // calculate distance between camera and object
            this->tmpDist = this->mUserCam->GetWorldPose().pos.Distance(this->contactPointTmp.x,this->contactPointTmp.y, this->contactPointTmp.z);

            // trying to get object name
            try // TODO we should fix this somehow
            {
                this->VPtr = this->mUserCam->GetScene()->GetVisualAt(this->mUserCam, this->coords2iTmp);
            }
            catch(...)
            {
                std::cout << "Error: Got point in space (" << this->contactPointTmp.x << ", " << this->contactPointTmp.y << ", " << this->contactPointTmp.z
                          << ") but no object name, (x,y)=(" << this->coords2iTmp.x << ", " << this->coords2iTmp.y << "), time: "
                          << static_cast<double>(this->sec) + static_cast<double>(this->nsec)*1e-9 << "\n";

                //we can log invalid data
                /*
                mongo::BSONObjBuilder b;
                mongo::BSONObj p;
                b.append( "world_point", BSON ("x" << this->contactPointTmp.x
                                         << "y" << this->contactPointTmp.y
                                         << "z" << this->contactPointTmp.z) );
                b.append("view_point", BSON ("x" << this->coords2iTmp.x
                                        << "y" << this->coords2iTmp.y) );
                b.append("timestamp",  static_cast<double>(this->sec) + static_cast<double>(this->nsec)*1e-9);
                p = b.obj();
                c.insert("invalid.data", p);
                */
            }

            // if we got the objext name, we can update its data
            if(this->VPtr != NULL)
            {
                this->addAtEnd = true;
                // if we have seen object before we just update location, distance and that we are see it right now
                for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
                {
                    if ( pos->objectName == this->VPtr->GetName() )
                    {
                        if (this->tmpDist < pos->distance)
                        {
                            pos->distance = this->tmpDist;
                            pos->contactPoint = this->contactPointTmp;                            
                        }                        
                        this->addAtEnd = false;
                        pos->seen = true;
                    }
                }
                // add new object to vector
                if (this->addAtEnd)
                {
                    tmpObj.objectName = this->VPtr->GetName();
                    tmpObj.contactPoint = this->contactPointTmp;
                    tmpObj.distance = this->tmpDist;
                    tmpObj.colorNumber = -1;
                    tmpObj.seen = true;
                    allwords_vp.push_back(tmpObj);
                }
            }
        }
    }

    // we are not detecting any objects
    if (allwords_vp.size() == 0)
        return;

    // sorting objects in vector by distance, the clossest will come first
    sort(allwords_vp.begin(), allwords_vp.end());    

    //sort objects by distance
    double refDistance = allwords_vp.begin()->distance + PROCENTS * allwords_vp.begin()->distance / 100;

    // object which are to far behind we will not show
    for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
    {
        if(pos->distance > refDistance)
        {
            pos->seen = false;
        }
    }
/*
    // print out list of all objects
    for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
    {
        std::cout << pos->objectName << " " <<  pos->distance << " " <<  pos->seen << "\n";
    }
    std::cout << "\n\n";
*/

    // visualisation
    for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
    {
        sceneNodeClosest = NULL;
        entityClosest = NULL;

        // spheres can not have same name as gazebo models!!!
        this->tmpString = pos->objectName + "_";

        // when object is detected, we show it
        if (pos->seen)
        {
            this->sphereAttached = false;

            // check if objects' associated sphere entity already exist
            if(this->manager->hasEntity(this->tmpString))
            {
                sceneNodeClosest = this->manager->getSceneNode(this->tmpString);
                entityClosest = this->manager->getEntity(this->tmpString);
                this->sphereAttached = true;
            }
            // or make new entity
            else
            {
                sceneNodeClosest = this->manager->getRootSceneNode()->createChildSceneNode(this->tmpString);
                entityClosest = this->manager->createEntity(this->tmpString, Ogre::SceneManager::PT_SPHERE);
                sceneNodeClosest->setScale(SPHERE_SCALE_FACTOR, SPHERE_SCALE_FACTOR, SPHERE_SCALE_FACTOR);
                entityClosest->setVisible(true);
            }

            //set scene and entity settings
            sceneNodeClosest->setVisible(true);
            //entityClosest->setVisible(true);
            entityClosest->setMaterialName(colorNames[colorCounter++]);
            sceneNodeClosest->setPosition(pos->contactPoint.x, pos->contactPoint.y, pos->contactPoint.z);

            // add entity to scene node
            if (!this->sphereAttached)
            {
                sceneNodeClosest->attachObject(entityClosest);
            }
        }
        // hide spheres of objects which are not seen at the moment
        else
        {

            if(this->manager->hasEntity(this->tmpString))
            {
                sceneNodeClosest = this->manager->getSceneNode(this->tmpString);
                //entityClosest = this->manager->getEntity(this->tmpString);
            }
            else
            {
                sceneNodeClosest = this->manager->getRootSceneNode()->createChildSceneNode(this->tmpString);
                entityClosest = this->manager->createEntity(this->tmpString, Ogre::SceneManager::PT_SPHERE);
                sceneNodeClosest->setScale(SPHERE_SCALE_FACTOR, SPHERE_SCALE_FACTOR, SPHERE_SCALE_FACTOR);
                entityClosest->setVisible(true);
            }
            sceneNodeClosest->setVisible(false);
            //entityClosest->setVisible(false);
        }
    }
}


//////////////////////////////////////////////////
void EyeTracker::OnUpdate()
{
    //this->mUserCam = gui::get_active_camera(); TODO remove at some point

    if (callClossest)
    {

        // eye position
        this->coords2i.x = this->xI;
        this->coords2i.y = this->yI;
        if( ((this->coords2i.x < 0) || (this->coords2i.x > SCREEN_RESOLUTION[0]) || (this->coords2i.y < 0) || (this->coords2i.y > SCREEN_RESOLUTION[1])) && this->logData )
        {
            //std::cout << "Eyes are not located inside a screen (" << this->coords2i.y << ", " << this->coords2i.x <<  ").\n";
            mongo::BSONObjBuilder b;
            mongo::BSONObj p;
            b.append("models", "eyes located out of screen");
            b.append("timestamp",  static_cast<double>(this->sec) + static_cast<double>(this->nsec)*1e-9);
            p = b.obj();
            c.insert("valid.data", p);
            this->logData = false;
            return;
        }

        // get objects
        getClosestObjects();

        callClossest = false;

        if(this->logData)
        {
            // sending data to mongodb server
            mongo::BSONObjBuilder b;
            mongo::BSONObj p;

            if (allwords_vp.empty())
            {
                // append empty list if do not see any elements
                b.append("models", mongo::BSONArray());
            }
            else
            {
                mongo::BSONArrayBuilder _contacts_arr_builder;
                for(auto pos = allwords_vp.begin(); pos != allwords_vp.end(); pos++)
                {
                    if (pos->seen)
                    {
                        int first_pos = pos->objectName.find("::");
                        int last_pos = pos->objectName.rfind("::");
                        if ( (first_pos > -1) && (last_pos > first_pos))
                        {
                            mongo::BSONObj _contact = BSON("model_name" << pos->objectName.substr(0, first_pos)
                                                        << "link_name" << pos->objectName.substr(first_pos + 2, last_pos - (first_pos + 2))
                                                        << "visual_name" << pos->objectName.substr(last_pos + 2)
                                                        << "pos" << BSON ("x" << pos->contactPoint.x
                                                                        << "y" << pos->contactPoint.y
                                                                        << "z" << pos->contactPoint.z)
                                                        << "distance_from_camera" << pos->distance);
                            // append object to array
                            _contacts_arr_builder.append(_contact);
                        }
                        else
                        {
                            std::cout << "Error: Bad model name: " << pos->objectName << "\n";
                            continue;
                        }
                    }
                }
                // create the bson contacts array and append it to BSONObjBuilder
                b.append("models", _contacts_arr_builder.arr());
            }
            b.append("timestamp",  static_cast<double>(this->sec) + static_cast<double>(this->nsec)*1e-9);
            p = b.obj();
            c.insert("valid.data", p);
            //std::cout << p << "\n\n";
            this->logData = false;
        }
        else
            this->logData = true;
    }
}

//////////////////////////////////////////////////
void EyeTracker::ListenThread()
{
    boost::posix_time::millisec workTime(1);
    while (1)
    {
        memset(buffer, 0, BUFFERLENGTH);
        this->rc = read(this->sd, &buffer[0], BUFFERLENGTH);
        if (this->rc < 0)
        {
            perror("Client: read() error, closing gazebo ...");
            close(sd);
            exit(-1);
        }
        else if (this->rc == 0)
        {
            printf("Client: Server program has issued a close()\n");
            //TODO what to do here?
        }
        else
        {
            // "checking" if we have valid data >>int int<<
            pch = strchr(this->buffer,' ');
            if(pch != NULL)
            {
                xI = strtol(this->buffer, &pEnd, 10);
                yI = strtol(pEnd, NULL, 10);
            }            
        }
        boost::this_thread::sleep(workTime);
    }
}

//////////////////////////////////////////////////
void EyeTracker::ConnectToServer()
{
    //TODO obsolete http://linux.die.net/man/3/gethostbyname
    if( (this->he=gethostbyname(SERVER)) == NULL )
    {
        perror("Client: gethostbyname()");
        exit(1);
    }
    else
        printf("Client: The remote host is: %s\n", SERVER);

    if ((this->sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client: socket()");
        exit(-1);
    }
    else
        printf("Client: Socket is OK\n");

    this->serveraddr.sin_family = AF_INET;
    this->serveraddr.sin_port = htons(SERVPORT);
    this->serveraddr.sin_addr = *((struct in_addr *) this->he->h_addr);

    // zero the rest of the struct
    memset(&(this->serveraddr.sin_zero), '\0', 8);

    if(connect(this->sd, (struct sockaddr *)&this->serveraddr, sizeof(this->serveraddr)) == -1)
    {
        perror("Client: connect() failed, is server running?");
        close(this->sd);
        exit(1);
    }
    else
    {
        printf("Client: Connected to Eye tracker server\n");
    }
}

GZ_REGISTER_VISUAL_PLUGIN(EyeTracker)



