#include "main.h"

int main(int argc, char *argv[])
{
  std::cout << "OpenCV version : " << CV_VERSION << std::endl;
  std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
  std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
  std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
    
    //OPENCV WEBCAM
    if (! cap.isOpened())     // you have to CHECK this !
    {
        std::cout << "Could not open the VideoCapture !" << std::endl;
        return -1;
    }
    pthread_create(&webcamThread, NULL, WebCamCapture, NULL);

    std::cout << "Loading OpenFace" << std::endl;
    std::cout << "Creating Image Capture" << std::endl;
    Utilities::ImageCapture image_reader;
    image_reader.Create(); //MY VERSION
    // Load the models if images found
    std::cout << "Loading the model" << std::endl;
    LandmarkDetector::FaceModelParameters det_parameters; //DEFAULT CONSTRUCTOR
    // The modules that are being used for tracking
    std::cout << "MODEL LOC = " << det_parameters.model_location << std::endl;
    LandmarkDetector::CLNF face_model(det_parameters.model_location);
    if (!face_model.loaded_successfully)
    {
        cout << "ERROR: Could not load the landmark detector" << endl;
        return 1;
    }
    std::cout << "Model loaded" << std::endl;
    // If bounding boxes not provided, use a face detector
    cv::CascadeClassifier classifier(det_parameters.haar_face_detector_location);
    dlib::frontal_face_detector face_detector_hog = dlib::get_frontal_face_detector();
    LandmarkDetector::FaceDetectorMTCNN face_detector_mtcnn(det_parameters.mtcnn_face_detector_location);
    // If can't find MTCNN face detector, default to HOG one
    if (det_parameters.curr_face_detector == LandmarkDetector::FaceModelParameters::MTCNN_DETECTOR && face_detector_mtcnn.empty())
    {
        cout << "INFO: defaulting to HOG-SVM face detector" << endl;
        det_parameters.curr_face_detector = LandmarkDetector::FaceModelParameters::HOG_SVM_DETECTOR;
    }
    std::cout << "Face Detector Loaded" << std::endl;
    std::cout << "OpenFace Loaded" << std::endl; 


    //CS 168 Read the dfs file into SILT::DFSurface surface object.
    STL_shape skull = STL_shape("skull.stl");
    std::cout << "MID ( " << skull.midx << " ," << skull.midy << " ,"<< skull.midz << ")" << std::endl;
    std::cout << "DONE INIT" << std::endl;

    STL_shape tumor = STL_shape("tumor.stl");


    //GLFW WINDOW SETUP
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //for OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    WINDOW_WIDTH = (unsigned int) cap.get(CV_CAP_PROP_FRAME_WIDTH);
    WINDOW_HEIGHT = (unsigned int) cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    std::cout << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << std::endl;
    
    //VIEW AND CAMERA
    Ratio = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT;
    lastX = WINDOW_WIDTH / 2.0f;
    lastY = WINDOW_HEIGHT / 2.0f;
    
    
    float vertices_Webcam[] = {
        //Position           //texture
        -lastX, -lastY, 0.0f,   0.0f, 0.0f,
        lastX, -lastY, 0.0f,    1.0f, 0.0f,
        lastX, lastY, 0.0f,     1.0f, 1.0f,
        -lastX, lastY, 0.0f,    0.0f, 1.0f 
    };

    GLFWwindow* window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "My First Window", NULL, NULL);
                                                                                        //glfwGetPrimaryMonitor()   --for fullscreen app automatically

    if (window == NULL)
    {
        std::cout << "Failed to create the GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //setup GLAD, gives pointers to OS specific OpenGL function pointers
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    //Buffers

    //BUILD VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);  //BIND VAO FIRST BEFORE VBO

    //Vertex Buffer Object    
    unsigned int VBO;
    glGenBuffers(1, &VBO); //create VBOs

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind it
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skull.num_vertices * 6, skull.vertices, GL_STATIC_DRAW); //send data to GPU    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0); //setup Vertex Attributes 0 - POS
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3*sizeof(float))); //setup Vertex Attributes 1 - Normal
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); //UNBIND VAO FIRST
    glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO


    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);  //BIND VAO FIRST BEFORE VBO

    //Vertex Buffer Object    
    unsigned int VBO2;
    glGenBuffers(1, &VBO2); //create VBOs

    glBindBuffer(GL_ARRAY_BUFFER, VBO2); //Bind it
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tumor.num_vertices * 6, tumor.vertices, GL_STATIC_DRAW); //send data to GPU    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0); //setup Vertex Attributes 0 - POS
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3*sizeof(float))); //setup Vertex Attributes 1 - Normal
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); //UNBIND VAO FIRST
    glBindBuffer(GL_ARRAY_BUFFER, 0); //VBO


    //LIGHT
    unsigned int LightVAO;
    glGenVertexArrays(1, &LightVAO);
    glBindVertexArray(LightVAO);

    unsigned int LightVBO;
    glGenBuffers(1, &LightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, LightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Light), vertices_Light, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    unsigned int LightEBO;
    glGenBuffers(1, &LightEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_Light), indices_Light, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    //WEBCAM SCREEN
    unsigned int WebCamVAO;
    glGenVertexArrays(1, &WebCamVAO);
    glBindVertexArray(WebCamVAO);

    unsigned int WebCamVBO;
    glGenBuffers(1, &WebCamVBO);
    glBindBuffer(GL_ARRAY_BUFFER, WebCamVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_Webcam), vertices_Webcam, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int WebCamEBO;
    glGenBuffers(1, &WebCamEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WebCamEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_Webcam), indices_Webcam, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //TEXTURE
    unsigned int webcam_texture;
    glGenTextures(1, &webcam_texture);
    glBindTexture(GL_TEXTURE_2D, webcam_texture);
    //texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //texture shader
    Shader textureShader("./shaders/webcamShader.vs", "./shaders/webcamShader.fs");
    textureShader.use();
    textureShader.setInt("ourTexture", 0);
    textureShader.setMat4("model_transform", glm::translate(glm::mat4(1.0f), glm::vec3(skull.midx, skull.midy, skull.midz)));


    //SHADER SETUP
    glm::vec3 Light = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lightSize = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 skullColor = glm::vec3(0.90f, 0.90f, 0.90f);
    glm::vec3 tumorColor = glm::vec3(0.917f, 0.043f, 0.050f);
    
    Shader lightShader("./shaders/lightShader.vs", "./shaders/lightShader.fs");
    lightShader.use();
    lightShader.setVec3("lightColor", Light);

    Shader lightningShader("./shaders/lightningShader.vs", "./shaders/lightningShader.fs");
    lightningShader.use();
    lightningShader.setVec3("objectColor", skullColor);
    lightningShader.setVec3("lightColor", Light);
    lightningShader.setVec3("lightPos", lightPos);
    lightningShader.setFloat("transparency", blend);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // COORD SPACES 
    // Position FPS
    //camera = Camera(glm::vec3(skull.midx - 150.0f , skull.midy - 20.0f, skull.midz - 50.0f));

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));

    // -- VIEWING -- LookAt
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-74.0f, -85.0f, -220.0f));
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    lightningShader.setMat4("viewing", view);

    // -- PROJECTION -- LookAt
    glm::mat4 proj = glm::perspective(glm::radians(50.0f), Ratio, 0.1f, 1000.0f);
    lightningShader.setMat4("projection", proj);

    //cap.read(mirror);

    //RENDER LOOP
    while(!glfwWindowShouldClose(window))
    {
        //timing
        float current_frame = glfwGetTime();
        deltaTime = current_frame - lastFrame;
        lastFrame = current_frame;
        //Input Processing
        process_input(window);

        if(polygon)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //Time
        float Time = glfwGetTime(); 

        //Render
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Webcam and Texture
        
        if(!vecframes.empty() && !poseframes.empty())//frame.data)
        {   
            cv::Mat display_frame = vecframes.back().clone();
            cv::Mat pose_frame = poseframes.back().clone();

            vecframes.clear();
            poseframes.clear();
            coord_vec.clear();

            myFunc(pose_frame, coord_vec, image_reader, face_detector_mtcnn, face_model, det_parameters);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, display_frame.cols, display_frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, display_frame.data);
            
        }

        //DISPLAY SCREEN AND WEBCAM FRAMWE
        textureShader.use();
        textureShader.setMat4("viewing", view);
        textureShader.setMat4("projection", proj);
        glBindTexture(GL_TEXTURE_2D, webcam_texture);
        glBindVertexArray(WebCamVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glClear(GL_DEPTH_BUFFER_BIT); //clear z buffer
        

        float dist = 514.0f; //distance from camera

        //UNTRANSFORMED SKULL
        float X_eyeSpacing = abs(-29.7466 - (35.0669)); //L - R
        float Y_noseEyeSpacing = abs(-41.0725f - (-65.6835)); //MID EYES - NOSE

        //COMPUTE DIRECTION VECTOR - Left Eye
        // POS = -29.7466, -41.0725, 64.2449
        float lEx = coord_vec[0] - lastX;  //X - WINDOW WIDTH /2;
        float lEy = -coord_vec[1] + lastY; //-Y - WINDOW HEIGHT/2;

        glm::vec3 lE = glm::vec3(lEx, lEy, -CAMZ); 
        glm::vec3 nlE = glm::normalize(lE);
        glm::vec3 lEPos = glm::vec3(dist * nlE.x, dist * nlE.y, CAMZ + dist * nlE.z);

        //COMPUTE DIRECTION VECTOR - Right Eye
        //POS = 35.0669, -40.2896, 64.1509
        float rEx = coord_vec[2] - lastX;//50.0f;
        float rEy = -coord_vec[3] + lastY;//50.0f;
        glm::vec3 rE = glm::vec3(rEx, rEy, -CAMZ); 
        glm::vec3 nrE = glm::normalize(rE);

        //ROTATED PLANE
        glm::vec3 u = glm::vec3(1.0f, 0.0f, 0.0f);
        u = glm::rotate(u, coord_vec[6], glm::vec3(1.0f, 0.0f, 0.0f)); //ROTATE X
        u = glm::rotate(u, coord_vec[7], glm::vec3(0.0f, -1.0f, 0.0f)); //ROTATE Y
        u = glm::rotate(u, coord_vec[8], glm::vec3(0.0f, 0.0f, -1.0f)); //ROTATE Z
        glm::vec3 v = glm::vec3(0.0f, 1.0f, 0.0f);
        v = glm::rotate(v, coord_vec[6], glm::vec3(1.0f, 0.0f, 0.0f)); //ROTATE X
        v = glm::rotate(v, coord_vec[7], glm::vec3(0.0f, -1.0f, 0.0f)); //ROTATE Y
        v = glm::rotate(v, coord_vec[8], glm::vec3(0.0f, 0.0f, -1.0f)); //ROTATE Z
        glm::vec3 plane_n = glm::cross(u,v);
        plane_n = glm::normalize(plane_n);
        
        //DISTANCE RIGHT EYE PLANE
        float dist_rE = glm::dot(glm::vec3(lEPos.x, lEPos.y, lEPos.z - CAMZ), plane_n) / glm::dot(nrE, plane_n);
        glm::vec3 rEPos = glm::vec3(dist_rE * nrE.x, dist_rE * nrE.y, CAMZ + dist_rE * nrE.z);        
    

        //MID EYES POINT
        glm::vec3 midEyes = glm::vec3((lEPos.x + rEPos.x) / 2.0, (lEPos.y + rEPos.y) / 2.0, (lEPos.z + rEPos.z) / 2.0 );


        //COMPUTE DIRECTION VECTOR - Nose 
        //POS = 2.43943, -65.6835, 69.8924
        float Nx = coord_vec[4] - lastX;//0.0f;
        float Ny = -coord_vec[5] + lastY;//0.0f;
        glm::vec3 Nose = glm::vec3(Nx, Ny, -CAMZ);
        glm::vec3 nNose = glm::normalize(Nose);

        //DISTANCE NOSE AND PLANE
        float dist_nose = glm::dot(glm::vec3(lEPos.x, lEPos.y, lEPos.z - CAMZ), plane_n) / glm::dot(nNose, plane_n);
        glm::vec3 NosePos = glm::vec3(dist_nose * nNose.x, dist_nose * nNose.y, CAMZ + dist_nose * nNose.z); 


        //SKULL SCALING 
        glm::vec3 scalingEyes = glm::vec3( 1.1f *  glm::distance(rEPos, lEPos) / X_eyeSpacing, glm::distance(midEyes, NosePos) / Y_noseEyeSpacing, 1.2f);        

        //COMPUTE FINAL TRANSLATION
        glm::vec3 initPoslE = glm::vec3((-29.7466f)* scalingEyes.x, (-41.0725f)* scalingEyes.y, (64.2449f)* scalingEyes.z ); //SCALE
        initPoslE = glm::rotate(initPoslE, coord_vec[6], glm::vec3(1.0f, 0.0f, 0.0f)); //ROTATE X
        initPoslE = glm::rotate(initPoslE, coord_vec[7], glm::vec3(0.0f, -1.0f, 0.0f)); //ROTATE Y
        initPoslE = glm::rotate(initPoslE, coord_vec[8], glm::vec3(0.0f, 0.0f, -1.0f)); //ROTATE Z
        glm::vec3 translationlE = glm::vec3(lEPos.x - initPoslE.x, lEPos.y - initPoslE.y, lEPos.z - initPoslE.z); //TRANSLATE

        

        //Light
        float Lrad = 200.0f;
        int Ldrag = 10;
        float LX = sin(cam_XZ - Ldrag * camera_Speed) * Lrad;
        float LZ = cos(cam_XZ - Ldrag * camera_Speed) * Lrad;
        lightPos = glm::vec3(LX, 0.0f, LZ);

        lightningShader.use();
        lightningShader.setVec3("lightPos", lightPos);

        if (!fps_cam) //CAMERA LOOK AT
        {
            float radius = CAMZ; //CAM Z
            float camX = sin(cam_XZ) * radius;
            float camZ = cos(cam_XZ) * radius;

            view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));  
            lightningShader.setMat4("viewing", view);

            glm::mat4 proj = glm::perspective(glm::radians(50.0f), Ratio, 0.1f, 800.0f);
            lightningShader.setMat4("projection", proj);            
        }
        else //CAMERA FPS
        {
            std::cout << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
            view = camera.GetViewMatrix();  
            lightningShader.setMat4("viewing", view);

            proj = glm::perspective(glm::radians(camera.Zoom), Ratio, 0.1f, 800.0f);
            lightningShader.setMat4("projection", proj);
        }


        //SKULL -- Transparent object second
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, translationlE);
        transform = glm::rotate(transform, coord_vec[6], glm::vec3(1.0f, 0.0f, 0.0f)); //X
        transform = glm::rotate(transform, coord_vec[7], glm::vec3(0.0f, -1.0f, 0.0f)); //Y
        transform = glm::rotate(transform, coord_vec[8], glm::vec3(0.0f, 0.0f, -1.0f)); //Z
        transform = glm::scale(transform, scalingEyes);
        transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
        
        glm::mat4 tumorTransform = glm::translate(transform, glm::vec3(20.0f, -10.0f, 15.0f));
        lightningShader.setMat4("model_transform", tumorTransform);
        lightningShader.setVec3("objectColor", tumorColor);
        lightningShader.setFloat("transparency", 1.0f);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, tumor.num_vertices);
        glBindVertexArray(0);
        
        lightningShader.setMat4("model_transform", transform);
        lightningShader.setVec3("objectColor", skullColor);
        lightningShader.setFloat("transparency", blend);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, skull.num_vertices);
        glBindVertexArray(0);

        //LIGHT
        lightShader.use();
        lightShader.setMat4("viewing", view);
        lightShader.setMat4("projection", proj);
        glm::mat4 lightTransform = glm::mat4(1.0f);
        lightTransform = glm::translate(lightTransform, translationlE);
        lightTransform = glm::translate(lightTransform, glm::vec3(LX, 0.0f, LZ));
        lightTransform = glm::translate(lightTransform, lightPos);
        lightTransform = glm::scale(lightTransform, lightSize);
        lightShader.setMat4("model_transform", lightTransform);
        
        glBindVertexArray(LightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        //Swap Buffers, Poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    //Deallocate buffers allocated
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteVertexArrays(1, &LightVAO);
    glDeleteBuffers(1, &LightVBO);
    glDeleteBuffers(1, &LightEBO);
    
    cap.release(); // release webcam 

    //terminate GLFW v 
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback( GLFWwindow* window, int width, int height)
{
   // glfwSetWindowSizeCallback(window, &width, &height);
   glViewport(0, 0, width, height); 
}

void mouse_callback( GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovements(xoffset, yoffset);
}

void scroll_callback( GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void process_input(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true); 

    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        polygon = !polygon; 

    if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        fps_cam = !fps_cam; 

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && blend < 1.0f)
        blend += 0.05f;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && blend > 0.0f)
        blend -= 0.05f;

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cam_XZ += camera_Speed;

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cam_XZ -= camera_Speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARDS, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}



double current_time_in_seconds()
{
  timeval timer;
  gettimeofday(&timer,NULL);
  double seconds = 1e-6 * timer.tv_usec + timer.tv_sec;
  return seconds;
}


void* WebCamCapture(void*)
{
    while(1)
    {    
        if(!cap.read(mirror))
            break;
        cv::flip(mirror, frame, -1);   
        vecframes.push_back(frame);
        cv::flip(mirror, pose, +1);
        poseframes.push_back(pose);
    }
}




void myFunc(cv::Mat my_img, std::vector<float> &some_vec, 
        Utilities::ImageCapture &image_reader, LandmarkDetector::FaceDetectorMTCNN &face_detector_mtcnn,
        LandmarkDetector::CLNF &face_model, LandmarkDetector::FaceModelParameters &det_parameters)
{
    //Start func
    image_reader.LoadFrame(my_img);  //ADDED BY ME
         
    //cv::Mat rgb_image = my_img;
    cv::Mat rgb_image = image_reader.GetRGBFrame(); //MY VERSION


    //if (!face_model.eye_model)
    //{
    //    std::cout << "WARNING: no eye model found" << std::endl;
    //}

    //std::cout << "Starting tracking" << std::endl;
    // Making sure the image is in uchar grayscale (some face detectors use RGB, landmark detector uses grayscale)
    
    //cv::Mat_<uchar> grayscale_image = image_reader.GetGrayFrame();
    cv::Mat_<uchar> grayscale_image = image_reader.GetGRAYFrame(); //MY VERSION
    //std::cout << "Got GreyScale " << std::endl;
    // Detect faces in an image
    vector<cv::Rect_<float> > face_detections;

    if (image_reader.has_bounding_boxes)
    {
        face_detections = image_reader.GetBoundingBoxes();
    }
    else
    {
      //  std::cout << "No Bounding Box" << std::endl;
        vector<float> confidences;
        LandmarkDetector::DetectFacesMTCNN(face_detections, rgb_image, face_detector_mtcnn, confidences);        
        //std::cout << "Detected Face" << std::endl;
        // Detect landmarks around detected faces
        int face_det = 0;

        
        for (size_t face = 0; face <face_detections.size(); ++face)
        {

            // if there are multiple detections go through them
            bool success = LandmarkDetector::DetectLandmarksInImage(rgb_image, face_detections[face], face_model, det_parameters, grayscale_image);

            // Estimate head pose and eye gaze              
            cv::Vec6d pose_estimate = LandmarkDetector::GetPose(face_model, image_reader.fx, image_reader.fy, image_reader.cx, image_reader.cy);

            //This is where my code starts
            int iterator = 36;
            float counter = 0;
            float counter2 = 0;
            float left_eye_avg_x;
            float left_eye_avg_y;
            for(; iterator<42; iterator++){
                counter += *face_model.detected_landmarks[iterator];
                counter2 += *face_model.detected_landmarks[iterator + 68];
            }
            left_eye_avg_x = counter / 6.0;
            counter = 0;
            left_eye_avg_y = counter2 / 6.0;
            counter2 = 0;
            float right_eye_avg_x = 0;
            float right_eye_avg_y = 0;
            for(; iterator<48;iterator++){
                counter += *face_model.detected_landmarks[iterator];
                counter2 += *face_model.detected_landmarks[iterator + 68];

            }
            right_eye_avg_x = counter / 6;
            right_eye_avg_y = counter2 / 6;

            float nose_x = *face_model.detected_landmarks[29];
            float nose_y = *face_model.detected_landmarks[29+68];

            some_vec.push_back(left_eye_avg_x); //average of left eye x coord
            some_vec.push_back(left_eye_avg_y); //average of left eye y coord
            some_vec.push_back(right_eye_avg_x); //Average of right eye x coord
            some_vec.push_back(right_eye_avg_y); //average of right eye y coord
            some_vec.push_back(nose_x); //Average of nose x coord
            some_vec.push_back(nose_y); //Average of nose y coord
            some_vec.push_back(pose_estimate[3]);//pose_Rx
            some_vec.push_back(pose_estimate[4]);//pose_Ry
            some_vec.push_back(pose_estimate[5]);//pose_Rz
            some_vec.push_back(pose_estimate[0]);//pose_Tx
            some_vec.push_back(pose_estimate[1]);//pose_Tz
            some_vec.push_back(pose_estimate[2]);//post_Tz

        }

    }
    return;
}

