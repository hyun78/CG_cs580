// Include standard headers
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include common libraries
#include <common/geometry.hpp>
#include <common/model.hpp>

using namespace std;
using namespace glm;

// Global variable
GLFWwindow* g_window;
float g_window_width = 1024.f;
float g_window_height = 768.f;
int g_framebuffer_width = 1024;
int g_framebuffer_height = 768;
float g_fov = 45.f; // 45-degree of fov
float g_fovy = g_fov; // y-angle of fov
//current node
int curr_node_idx =0;
int max_node_counts = 0;

float tab_amount = 1.5f;
float floor_height = 1.7f;
vec3 root_position = vec3(0.0f, 1.0, 0.0f);
std::vector<Model> g_models;
std::vector<mat4> g_model_rbts;

// Transformations
mat4 g_projection;
// Camera model property
mat4 g_eye_rbt;
// World model property
mat4 g_world_rbt = mat4(1.0f);

// Event functions
static void WindowSizeCallback(GLFWwindow*, int, int);
static void MouseButtonCallback(GLFWwindow*, int, int, int);
static void CursorPosCallback(GLFWwindow*, double, double);
static void KeyboardCallback(GLFWwindow*, int, int, int, int);
void UpdateFovy(void);
void CleanUp(void);

//data struct : 
class ModelTree {
private:
    ModelTree* parent; // if parent is null , then it is root
    ModelTree* left_child;
    ModelTree* mid_child;
    ModelTree* right_child;
    Model model_obj;
    mat4 local_rbt;
    mat4 init_rbt;
    vec3 r_vec;
    float rotation_speed;
    float rotation_status;
public:
    void SetLeftChild(ModelTree* child);
    void SetRightChild(ModelTree* child);
    void Rotate(void);
    void RotateChild(void);
    float GetRotationSpeed(void);
    void SetRotationSpeed(float speed);
    void ResetRotation(void);
    void SetRotationVector(vec3 rvec);
    void InitDataModel(Model* model,int type,vec3 color);
    void SetMidChild(ModelTree* child);
    Model* GetModel(void);
    ModelTree(int model_type, vec3 model_color,vec3 dir_light, ModelTree* parent_ptr) {
        model_obj = Model();
        InitDataModel(&model_obj,model_type,model_color);
        model_obj.InitializeGLSL(DRAW_TYPE::ARRAY, "VertexShader.glsl", "FragmentShader.glsl");
        model_obj.SetProjection(&g_projection);
        model_obj.SetEyeRbt(&g_eye_rbt);
        
        if (parent_ptr==NULL){
            init_rbt = translate(g_world_rbt, root_position); // roots position
            local_rbt = init_rbt;
        }
        else{
            init_rbt = mat4(1.0);
            local_rbt = init_rbt;
        }
        //g_model_rbts.push_back(cube_rbt);
        
        model_obj.SetModelRbt(&local_rbt);
        model_obj.SetDirectionalLight(dir_light);
        //g_models.push_back(cube);
        parent = parent_ptr;
        //model_obj = Model();
        rotation_speed = 0.0f;
        rotation_status= 0.0f;
        //init_rbt = *(model_obj->GetRBT());
        
        left_child = NULL;
        right_child = NULL;
        mid_child = NULL;
        r_vec = vec3(0.0f,1.0f,0.0f);
    }
    
};
void ModelTree::InitDataModel(Model* model, int type, vec3 color){
    if (type==MODEL_TYPE::CUBE){
        InitDataCube(*model, color);
        //printf("model init\n");
    }
    else if (type==MODEL_TYPE::SPHERE){
        //InitDataCube(*model, color);
        InitDataSphere(*model, color,0.5f); // sphere
    }
    else if (type==MODEL_TYPE::PYRAMID){
        //InitDataCube(*model, color);
    }
}

void ModelTree::SetLeftChild(ModelTree* child) {
    left_child = child;
    //left_child->init_rbt = translate(init_rbt,vec3(-tab_amount,-floor_height,0.0));
    child->init_rbt = translate(mat4(1.0),vec3(-tab_amount,-floor_height,0.0f));
    child->local_rbt = (init_rbt)*(child->init_rbt);
    child->GetModel()->SetModelRbt(&child->local_rbt);
}
void ModelTree::SetRightChild(ModelTree* child) {
    right_child = child;
    child->init_rbt = translate(mat4(1.0),vec3(tab_amount,-floor_height,0.0f));
    child->local_rbt = (init_rbt)*(child->init_rbt);
    child->GetModel()->SetModelRbt(&child->local_rbt);
}
void ModelTree::SetMidChild(ModelTree* child) {
    mid_child = child;
    child->init_rbt = translate(mat4(1.0),vec3(0.0f,-floor_height,0.0f));
    child->local_rbt = (init_rbt)*(child->init_rbt);
    child->GetModel()->SetModelRbt(&child->local_rbt);
}
void ModelTree::ResetRotation(void){
    rotation_status=0.0f;
    rotation_speed = 0.0f;
}
void ModelTree::Rotate(void) {
    mat4 L,T;
    mat4 m_rbt = (local_rbt);
    //L = get_linear(init_rbt);
    //T = get_translation(init_rbt);
    //local_rbt = inverse(L)*inverse(T)*local_rbt;
    //init_rbt = init_rbt*rotate(mat4(1.0f), rotation_speed, r_vec);
    //printf("rotation_speed : %f \n",rotation_speed);
    //local_rbt = init_rbt*local_rbt;
    rotation_status += rotation_speed;
    local_rbt = rotate(local_rbt,rotation_speed,r_vec);
    init_rbt = rotate(init_rbt,rotation_speed,r_vec);
    (model_obj).SetModelRbt(&local_rbt);
    //printf("or here?");
    RotateChild();
}
void ModelTree::RotateChild(void) {
    mat4 m_rbt;
    if (left_child != NULL) {
        //R = local_rbt
        (left_child->local_rbt) = local_rbt*(left_child->init_rbt);
        ((left_child->model_obj)).SetModelRbt(&(left_child->local_rbt));
        left_child->RotateChild();
    }
    
    if (right_child != NULL) {
        //printf("here too");
        (right_child->local_rbt) = local_rbt*(right_child->init_rbt);
        ((right_child->model_obj)).SetModelRbt(&(right_child->local_rbt));
        right_child->RotateChild();
    }
    if (mid_child!=NULL){
        (mid_child->local_rbt) = local_rbt*(mid_child->init_rbt);
        ((mid_child->model_obj)).SetModelRbt(&(mid_child->local_rbt));
        mid_child->RotateChild();
    }
}

void ModelTree::SetRotationSpeed(float speed) {
    rotation_speed = speed;
    
}
float ModelTree::GetRotationSpeed(void){
    return rotation_speed;
}
Model* ModelTree::GetModel(void){
    return &model_obj;
}

std::vector<ModelTree*> g_model_trees;

// Helper function: Update the vertical field-of-view(float fovy in global)
void UpdateFovy()
{
	if (g_framebuffer_width >= g_framebuffer_height)
	{
		g_fovy = g_fov;
	}
	else {
		const float RAD_PER_DEG = 0.5f * glm::pi<float>() / 180.0f;
		g_fovy = (float)atan2(sin(g_fov * RAD_PER_DEG) * ((float)g_framebuffer_height / (float)g_framebuffer_width), cos(g_fov * RAD_PER_DEG)) / RAD_PER_DEG;
	}
}

void CleanUp()
{
	for (int i = 0; i < g_models.size(); i++)
	{
		g_models[i].CleanUp();
	}
	g_models.clear();
	g_models.shrink_to_fit();

	g_model_rbts.clear();
	g_model_rbts.shrink_to_fit();
}

// TODO: Modify GLFW window resized callback function
static void WindowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
	// Get resized size and set to current window
	g_window_width = (float)a_width;
	g_window_height = (float)a_height;

	// glViewport accept pixel size, please use glfwGetFramebufferSize rather than window size.
	// window size != framebuffer size
	glfwGetFramebufferSize(a_window, &g_framebuffer_width, &g_framebuffer_height);
	glViewport(0, 0, (GLsizei)g_framebuffer_width, (GLsizei)g_framebuffer_height);
	
	// Update projection matrix
	g_projection = perspective(g_fov, ((float)g_framebuffer_width / (float)g_framebuffer_height), 0.1f, 100.0f);
}

// TODO: Fill up GLFW mouse button callback function
static void MouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
	//example code for get x position and y position of mouse click
	if (a_button == GLFW_MOUSE_BUTTON_LEFT && a_action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(a_window, &xpos, &ypos);
		xpos = xpos / ((double)g_window_width) * ((double)g_framebuffer_width);
		ypos = ypos / ((double)g_window_height) * ((double)g_framebuffer_height);
		printf("mouse click! %f %f",xpos,ypos);
	}
}

// TODO: Fill up GLFW cursor position callback function
static void CursorPosCallback(GLFWwindow* a_window, double a_xpos, double a_ypos)
{
	// 받아서 뭐 하는데?
}

static void KeyboardCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
	if (a_action == GLFW_PRESS)
	{
		switch (a_key)
		{
		case GLFW_KEY_H:
			cout << "CS580 Homework Assignment 1" << endl;
			cout << "keymaps:" << endl;
			cout << "h\t\t Help command" << endl;
			cout << "p\t\t Enable/Disable picking" << endl;
			break;
        case GLFW_KEY_M:
            cout << "you just pressed +10 degree accel key!" << endl;
                
                (*g_model_trees[curr_node_idx]).SetRotationSpeed((*g_model_trees[curr_node_idx]).GetRotationSpeed()+10.0f);
                
            break;
        case GLFW_KEY_N:
            cout << "you just pressed -10 degree accel key!" << endl;
                (*g_model_trees[curr_node_idx]).SetRotationSpeed((*g_model_trees[curr_node_idx]).GetRotationSpeed()-10.0f);
                //printf("now it is %f \n",g_model_trees[curr_node_idx].GetRotationSpeed()-10.0f);
            break;
        case GLFW_KEY_R:
            cout << "you just pressed rotation stop key!" << endl;
                (*g_model_trees[curr_node_idx]).ResetRotation();
            break;
        case GLFW_KEY_O:
            cout << "you just pressed selection key!" << endl;
            curr_node_idx+=1;
            curr_node_idx = curr_node_idx % max_node_counts;
                printf("curr node idx : %d \n",curr_node_idx);
            break;
		default:
			break;
		}
	}
}


int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	g_window = glfwCreateWindow( 1024, 768, "Homework 1: Your Student ID - Your Name", NULL, NULL);

	if( g_window == NULL ){
		fprintf( stderr, "Failed to open GLFW window." );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(g_window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(g_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetWindowSizeCallback(g_window, WindowSizeCallback);
	glfwSetMouseButtonCallback(g_window, MouseButtonCallback);
	glfwSetCursorPosCallback(g_window, CursorPosCallback);
	glfwSetKeyCallback(g_window, KeyboardCallback);

	// Clear with black backround
	glClearColor((GLclampf)0.0f, (GLclampf)0.0f, (GLclampf)0.0f, (GLclampf) 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Enable culling
	glEnable(GL_CULL_FACE);
	// Backface culling
	glCullFace(GL_BACK);

	// Setting viewport
	// glViewport accept pixel size, please use glfwGetFramebufferSize rather than window size.
	// window size != framebuffer size
	glfwGetFramebufferSize(g_window, &g_framebuffer_width, &g_framebuffer_height);
	glViewport(0, 0, (GLsizei)g_framebuffer_width, (GLsizei)g_framebuffer_height);

	g_projection = perspective(g_fov, ((float)g_framebuffer_width / (float)g_framebuffer_height), 0.1f, 100.0f);

	g_eye_rbt = translate(mat4(1.0f), vec3(0.0, -1.0f, 10.0));

	// initialize models
	g_models = std::vector<Model>();
	g_model_rbts = std::vector<mat4>();

	// initialize directional light
	vec3 dir_light = vec3(0.0f, -1.0f, 1.0f);
    float top_y = 1.0;
	float second_y = -1.4f; // height of one model

    //setting global variables
    
    
    // cube
    vec3 cube_color =vec3(1.0f, 0.0f, 0.0f);
//    Model cube = Model();
//    InitDataCube(cube, cube_color);
//    cube.InitializeGLSL(DRAW_TYPE::ARRAY, "VertexShader.glsl", "FragmentShader.glsl");
//    cube.SetProjection(&g_projection);
//    cube.SetEyeRbt(&g_eye_rbt);
//
//    mat4 cube_rbt = translate(g_world_rbt, vec3(0.0f, top_y, 0.0f));
//    //g_model_rbts.push_back(cube_rbt);
//    cube.SetModelRbt(&cube_rbt);
//    cube.SetDirectionalLight(dir_light);
//    //g_models.push_back(cube);
	

//    Model pyramid = Model();
//    //InitDataCube(cube, vec3(0.0f, 1.0f, 0.0f)); // real cube
//    //InitDataSphere(cube, vec3(0.0f, 1.0f, 0.0f),0.5f); // sphere
//    InitDataPyramid(pyramid, vec3(0.0f, 0.0f, 1.0f));
//    pyramid.InitializeGLSL(DRAW_TYPE::ARRAY, "VertexShader.glsl", "FragmentShader.glsl");
//    pyramid.SetProjection(&g_projection);
//    pyramid.SetEyeRbt(&g_eye_rbt);
//    mat4 pyramid_rbt = translate(g_world_rbt, vec3(-1.0f, top_y+second_y-1.0, 0.0f));
//    //mat4 rotated_rbt = pyramid_rbt;
//    pyramid.SetModelRbt(&pyramid_rbt);
//    pyramid.SetDirectionalLight(dir_light);
//

//    Model sphere = Model();
//    //InitDataSphere(my_sphere, vec3(1.0f, 0.0f, 0.0f),1.0f);
//    InitDataSphere(sphere, vec3(0.0f, 0.0f, 1.0f),0.8f);
//    sphere.InitializeGLSL(DRAW_TYPE::ARRAY, "VertexShader.glsl", "FragmentShader.glsl");
//    sphere.SetProjection(&g_projection);
//    sphere.SetEyeRbt(&g_eye_rbt);
//
//    mat4 sphere_rbt = translate(g_world_rbt, vec3(1.0f, top_y+second_y-1.0f, 0.0f));
//    //g_model_rbts.push_back(my_sphere_rbt);
//    sphere.SetModelRbt(&sphere_rbt);
//    sphere.SetDirectionalLight(dir_light);
//    //g_models.push_back(my_sphere);
//
//    Model pyramid2 = Model();
//    //InitDataCube(cube, vec3(0.0f, 1.0f, 0.0f)); // real cube
//    //InitDataSphere(cube, vec3(0.0f, 1.0f, 0.0f),0.5f); // sphere
//    InitDataPyramid(pyramid2, vec3(0.0f, 0.0f, 1.0f));
//    pyramid2.InitializeGLSL(DRAW_TYPE::ARRAY, "VertexShader.glsl", "FragmentShader.glsl");
//    pyramid2.SetProjection(&g_projection);
//    pyramid2.SetEyeRbt(&g_eye_rbt);
//    mat4 pyramid_rbt2 = translate(g_world_rbt, vec3(-1.0f, top_y+2*second_y-1.0, 0.0f));
//    //mat4 rotated_rbt = pyramid_rbt;
//    pyramid2.SetModelRbt(&pyramid_rbt2);
//    pyramid2.SetDirectionalLight(dir_light);
//
//    Model Line = Model();
//    //InitDataLine(&Line, vec3(0.0f, 0.0f, 0.0f),vec3(1.0f, 1.0f, 1.0f),vec3(0.0f, 0.0f, 1.0f));
//    InitDataLine(Line, vec3(-10.0f,-10.0f,-10.0f), vec3(10.0f,10.0f,10.0f), vec3(1.0f,0.0f,0.0f));
//    //void InitDataLine(Model &a_model, vec3 start, vec3 end, vec3 a_color)
//    Line.InitializeGLSL(DRAW_TYPE::LINE, "VertexShader.glsl", "FragmentShader.glsl");
//    Line.SetProjection(&g_projection);
//    Line.SetEyeRbt(&g_eye_rbt);
//    mat4 Line_rbt = translate(g_world_rbt, vec3(0.0f, top_y+2*second_y-1.0, 0.0f));
//    //mat4 rotated_rbt = pyramid_rbt;
//    Line.SetModelRbt(&Line_rbt);
//    Line.SetDirectionalLight(dir_light);
    
    
    ModelTree roots = ModelTree(MODEL_TYPE::CUBE, cube_color, dir_light, NULL);
    g_model_trees.push_back(&roots);
    max_node_counts++;

    vec3 sphere_color = vec3(0.0f,1.0f,0.0f);
    ModelTree sphere = ModelTree(MODEL_TYPE::SPHERE,sphere_color,dir_light,&roots);
    roots.SetLeftChild(&sphere);
    g_model_trees.push_back(&sphere);
    max_node_counts++;
    vec3 cube_color2 = vec3(0.0f,0.0f,1.0f);
    
    ModelTree cube2 = ModelTree(MODEL_TYPE::CUBE,cube_color2,dir_light,&roots);
    roots.SetRightChild(&cube2);
    g_model_trees.push_back(&cube2);
    max_node_counts++;
    
    ModelTree cube3 = ModelTree(MODEL_TYPE::CUBE,cube_color2,dir_light,&roots);
    roots.SetMidChild(&cube3);
    g_model_trees.push_back(&cube3);
    max_node_counts++;
//    ModelTree leftchild = ModelTree(MODEL_TYPE::CUBE, &roots,&g_world_rbt);
//    roots.SetLeftChild(&leftchild);
//    ModelTree rightchild = ModelTree(&sphere, &roots,&g_world_rbt);
//    roots.SetRightChild(&rightchild);
//    ModelTree pyramid2_tree = ModelTree(&pyramid2, &leftchild,&g_world_rbt);
//    leftchild.SetLeftChild(&pyramid2_tree);
//
    //roots.SetRotationSpeed(2.0f);
	//rightchild.SetRotationSpeed(0.5f);

    
    //global model push
    
//    g_model_trees.push_back(&rightchild);
//    max_node_counts++;
//    g_model_trees.push_back(&pyramid2_tree);
//    max_node_counts++;
	//ModelTree roots = ModelTree(&another_cube, NULL, &g_model_rbts, &g_models);
    
    
    

	do{
		// Clear the screen	
		// Fill the background
		glClearColor((GLclampf)0.0f, (GLclampf)0.0f, (GLclampf)0.0f, (GLclampf)0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*
         
		rotated_rbt = rotate(rotated_rbt, 0.1f, vec3(1.0f, 1.0f, 1.0f));

		g_model_rbts.push_back(rotated_rbt);

		cube.SetModelRbt(&rotated_rbt);
		cube.SetDirectionalLight(dir_light);

		g_models.push_back(cube);
*/
        
        
        //leftchild.Rotate();
        //Line.Draw();
        for (int idx=0; idx<max_node_counts;idx++){
            g_model_trees[idx]->Rotate();
            if (curr_node_idx==idx){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                g_model_trees[idx]->GetModel()->Draw();
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else{
                g_model_trees[idx]->GetModel()->Draw();
            }
        }
        //g_model_trees[2]->Rotate();
        //g_model_trees[1]->Rotate();
        
        
        //roots.Rotate();
		//rightchild.Rotate();
		//child1.Rotate();
        //leftchild.Rotate();
        //rightchild.Rotate();
		//cube.Draw();
		//pyramid.Draw();
		//sphere.Draw();
		
		//my_line.Draw();
		// Swap buffers
		
		glfwSwapBuffers(g_window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(g_window) == 0);

	// Cleanup all resources
	CleanUp();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
