#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "shader.h"
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

//CS168
#include "stlLoader.h"
#include <opencv2/opencv.hpp> // opencv2 headers !
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

//OPENFACE

#include <dlib/image_processing/frontal_face_detector.h>
#include "openface/core/LandmarkCoreIncludes.h"
#include <FaceAnalyser.h>
#include <GazeEstimation.h>
#include <ImageCapture.h>
#include <Visualizer.h>
#include <VisualizationUtils.h>
#include <RecorderOpenFace.h>
#include <RecorderOpenFaceParameters.h>

//ANALYZE PICTURE
void myFunc(cv::Mat my_img, std::vector<float> &some_vec, Utilities::ImageCapture &image_reader, LandmarkDetector::FaceDetectorMTCNN &face_detector_mtcnn, LandmarkDetector::CLNF &face_model, LandmarkDetector::FaceModelParameters &det_parameters);

std::vector<float> coord_vec;

pthread_t webcamThread;
void* WebCamCapture(void*);
std::vector <cv::Mat> poseframes;
std::vector <cv::Mat> vecframes;
//using namespace cv;
float vertices_Light[] = {
    //position           
    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  

    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f, -0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  

     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  

    -0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f, -0.5f,  
     0.5f, -0.5f,  0.5f,  
     0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f, -0.5f,  

    -0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f, -0.5f,  
     0.5f,  0.5f,  0.5f,  
     0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f  
};

unsigned int indices_Light[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35
    
};


// WEBCAM VISION
//vertices_Webcam[] in main!
unsigned int indices_Webcam[] = {
    0,1,2, 0,2,3
};

cv::VideoCapture cap(0); //open webcam
cv::Mat mirror;
cv::Mat pose;
cv::Mat frame;
float time_elapsed = 0.0;
float FRAMERATE = 15.0;
bool skip = false;
double fps;
double current_time_in_seconds();

//Window
unsigned int WINDOW_WIDTH;//800;
unsigned int WINDOW_HEIGHT; //600;
float Ratio;

//CALLBACKS
//Framebuffer and Input 
void framebuffer_size_callback( GLFWwindow* window, int width, int height);
void mouse_callback( GLFWwindow* window, double xpos, double ypos);
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window);
void Draw(STL_shape skull, unsigned int VAO, unsigned int LightVAO, unsigned int WebCamVAO);
//polygon mode
bool polygon = false;

//texture blending
float blend = 1.0f;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

//CAMERA -- FPS
Camera camera;
float CAMZ = 514.0f;
float lastX;
float lastY;
bool firstMouse = true;

//CAMERA -- LookAt
glm::vec3 cameraPos; //= glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float camera_Speed = 0.05f;
//Look at Manual
float cam_XZ = 0.0;
//camera mode
bool fps_cam = false;
