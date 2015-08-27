#include <Client1.hh>

using namespace std;
using namespace gazebo;

//////////////////////////////////////////////////
void Client1::Load(rendering::VisualPtr _parent, sdf::ElementPtr /*_sdf*/)
{
    cout << "Load function has been called" << endl;
    this->ConnectToServer();

    this->listeningThread = new boost::thread(boost::bind(&Client1::ListenThread, this));

    // Listen to the update event. This event is broadcast every
    // simulation iteration.
    this->updateFunction = event::Events::ConnectRender(
        boost::bind(&Client1::OnUpdate, this));

    /*
    this->manager = gui::get_active_camera()->GetScene()->GetManager();
    //this->sceneNode = this->manager->getRootSceneNode()->createChildSceneNode("MySphere");
    this->sceneNode = this->manager->getRootSceneNode()->createChildSceneNode();
    this->entity = this->manager->createEntity("MySphere", Ogre::SceneManager::PT_SPHERE);
    this->entity->setMaterialName("Gazebo/Red");
    this->sceneNode->setVisible(true);
    this->entity->setVisible(true);
    this->sceneNode->setScale(0.001, 0.001, 0.001);
    this->sceneNode->setPosition(2, 0, 2);
    this->sceneNode->attachObject(entity);
    this->xI = 0;
    this->yI = 0;
    */
}

//////////////////////////////////////////////////
void Client1::OnUpdate()
{
    //cout << "SystemSubscriber: Entering OnUpdate() " << endl;
    this->mUserCam = gui::get_active_camera();
    this->coords2i.x = this->xI;
    this->coords2i.y = this->yI;
    //printf("%d, %d\n", this->coords2i.x, this->coords2i.y); //this is working
    this->VPtr = this->mUserCam->GetScene()->GetVisualAt(this->mUserCam, this->coords2i);
    if (this->VPtr != NULL)
    {
        this->contactPoint = this->VPtr->GetPosition();
        cout << "Visual : " << this->VPtr->GetName() << endl;

        if (this->contactPoint.x>0 || this->contactPoint.y>0 || this->contactPoint.z>0)
        {
            printf("%f, %f, %f\n", this->contactPoint.x, this->contactPoint.y, this->contactPoint.z);

        }
    }
    else
        cout << "No data \n";


    //this->sceneNode->setPosition(xI/100.0, yI/100.0, 0);
}

//////////////////////////////////////////////////
void Client1::ConnectToServer()
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
        printf("Client-The connect() is OK...\n");
    }
}

//////////////////////////////////////////////////
void Client1::ListenThread()
{
    boost::posix_time::millisec workTime(1);
    while (1) {
        memset(buffer, 0, BufferLength);
        this->rc = read(this->sd, &buffer[0], BufferLength);
        if (this->rc < 0)
        {
            perror("Client-read() error");
            close(sd);
            exit(-1);
        } else if (this->rc == 0)
        {
            printf("Server program has issued a close()\n");
        }
        else
        {
            // lets be sure, that we recieve right data
            pch=strchr(this->buffer,' ');
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

GZ_REGISTER_VISUAL_PLUGIN(Client1)
