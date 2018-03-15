#ifndef MODEL_HPP
#define MODEL_HPP

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

enum DRAW_TYPE {
	ARRAY,
	INDEX,
	LINE
};

class Model {
	vector<vec3> m_positions;
	vector<unsigned int> m_indices;
	vector<vec3> m_normals;
	vector<vec3> m_colors;

	mat4* m_projection;
	mat4* m_eye_rbt;
	mat4* m_model_rbt;

	DRAW_TYPE m_draw_type;

	GLuint m_vertex_array_id;
	GLuint m_position_buffer_id;
	GLuint m_normal_buffer_id;
	GLuint m_color_buffer_id;
	GLuint m_index_buffer_id;
	
	GLint m_light_loc;
public:
	GLuint m_glsl_program_id;

	Model();
	// Add vertex position in (x,y,z) coordinate in model space
	void AddPosition(float a_px, float a_py, float a_pz);
	// Add vertex position in (x,y,z) glm vector in model space
	void AddPosition(vec3 a_position);
	// Add vertex normal in (x,y,z) coordinate in model space
	void AddNormal(float a_nx, float a_ny, float a_nz);
	// Add vertex normal in (x,y,z) glm vector in model space
	void AddNormal(vec3 a_normal);
	// Add vertex color in (r,g,b) components
	void AddColor(float a_r, float a_g, float a_b);
	// Add vertex color in (r,g,b) glm vector
	void AddColor(vec3 a_color);
	// Add index for triangle
	void AddIndex(unsigned int a_index);
	// Set pointer of projection matrix for this model
	void SetProjection(mat4* p_projection);
	// Set pointer of camera rigid-body matrix for this model
	void SetEyeRbt(mat4* p_eye_rbt);
	// Set pointer of model matrix for this model
	void SetModelRbt(mat4* p_model_rbt);
	// Set directional light vector for model
	void SetDirectionalLight(vec3 a_dir_light);
	// Set a shader program for this model and transfer data which stored in m_vertices, m_normals, m_colors, (m_indices are optional)
	void InitializeGLSL(DRAW_TYPE a_draw_type, const char * a_vertex_shader_path, const char * a_fragment_shader_path);
	// Draw function
	void Draw(void);
	// Clean up all resources
	void CleanUp(void);
	//get model's current RBT
	mat4* GetRBT(void);
};
class ModelTree {
private:
	ModelTree* parent; // if parent is null , then it is root
	ModelTree* left_child;
	ModelTree* right_child;
	Model* model_obj;
	mat4 local_rbt;
	mat4 init_rbt;
	std::vector<mat4>* local_g_model_rbts;
	std::vector<Model>* local_g_models;
	float rotation_speed;
public:
	void ModelTree::SetLeftChild(ModelTree* child);
	void ModelTree::SetRightChild(ModelTree* child);
	void ModelTree::Rotate(void);
	void ModelTree::RotateChild(void);
	void ModelTree::SetRotationSpeed(float speed);
	ModelTree(Model* a_model, ModelTree* parent_ptr) {
		parent = parent_ptr;
		model_obj = a_model;
		rotation_speed = 0.1f;
		init_rbt = *(a_model->GetRBT());
		left_child = NULL;
		right_child = NULL;
	}

};
#endif
