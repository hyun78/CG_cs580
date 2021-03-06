#include <iostream>
#include <vector>

#include "model.hpp"
#include "shader.hpp"
#include "affine.hpp"
//#include <common/geometry.hpp>
using namespace std;
using namespace glm;

//
//class ModelTree{
//	private:
//		ModelTree* parent; // if parent is null , then it is root
//		ModelTree* left_child;
//		ModelTree* right_child;
//		Model* model_obj;
//		float rotation_speed;
//	public:
//		void ModelTree::SetLeftChild( ModelTree &child);
//		void ModelTree::SetRightChild(ModelTree &child);
//		void ModelTree::Rotate(void);
//		ModelTree(Model &a_model, ModelTree &parent_ptr) {
//			parent = &parent_ptr;
//			model_obj = &a_model;
//			rotation_speed = 0.0f;
//	}
//		
//};


Model::Model()
{
	// Initialize model information
	m_positions = vector<vec3>();
	m_indices = vector<unsigned int>();
	m_normals = vector<vec3>();
	m_colors = vector<vec3>();
}

void Model::AddPosition(float a_px, float a_py, float a_pz)
{
	m_positions.push_back(vec3(a_px, a_py, a_pz));
}

void Model::AddPosition(vec3 a_position)
{
	m_positions.push_back(a_position);
}

void Model::AddNormal(float a_nx, float a_ny, float a_nz)
{
	m_normals.push_back(vec3(a_nx, a_ny, a_nz));
}

void Model::AddNormal(vec3 a_normal)
{
	m_normals.push_back(a_normal);
}

void Model::AddColor(float a_r, float a_g, float a_b)
{
	m_colors.push_back(vec3(a_r, a_g, a_b));
}

void Model::AddColor(vec3 a_color)
{
	m_colors.push_back(a_color);
}

void Model::AddIndex(unsigned int a_index)
{
	m_indices.push_back(a_index);
}

void Model::SetProjection(mat4* p_projection)
{
	m_projection = p_projection;
}

void Model::SetEyeRbt(mat4* p_eye_rbt)
{
	m_eye_rbt = p_eye_rbt;
}

void Model::SetModelRbt(mat4* p_model_rbt)
{
	m_model_rbt = p_model_rbt;
}

void Model::SetDirectionalLight(vec3 a_dir_light)
{
	m_light_loc = glGetUniformLocation(m_glsl_program_id, "uLight");
	glUniform3f(m_light_loc, a_dir_light.x, a_dir_light.y, a_dir_light.z);
}

void Model::InitializeGLSL(DRAW_TYPE a_draw_type, const char * a_vertex_shader_path, const char * a_fragment_shader_path)
{
	m_glsl_program_id = LoadShaders(a_vertex_shader_path, a_fragment_shader_path);
	m_draw_type = a_draw_type;

	glGenVertexArrays(1, &m_vertex_array_id);
	glBindVertexArray(m_vertex_array_id);

	glGenBuffers(1, &m_position_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_position_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*m_positions.size(), &m_positions[0], GL_STATIC_DRAW);

	if (m_draw_type == DRAW_TYPE::INDEX)
	{
		glGenBuffers(1, &m_index_buffer_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*m_indices.size(), &m_indices[0], GL_STATIC_DRAW);
	}

	glGenBuffers(1, &m_normal_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*m_normals.size(), &m_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_color_buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*m_colors.size(), &m_colors[0], GL_STATIC_DRAW);
}

void Model::Draw()
{
	
	glUseProgram(m_glsl_program_id);
	
	GLint projection_id = glGetUniformLocation(m_glsl_program_id, "Projection");
	GLint eye_id = glGetUniformLocation(m_glsl_program_id, "Eye");
	GLint model_id = glGetUniformLocation(m_glsl_program_id, "ModelTransform");

	glUniformMatrix4fv(projection_id, 1, GL_FALSE, &(*(m_projection))[0][0]);
	glUniformMatrix4fv(eye_id, 1, GL_FALSE, &(*(m_eye_rbt))[0][0]);
	glUniformMatrix4fv(model_id, 1, GL_FALSE, &(*(m_model_rbt))[0][0]);
    
    if (m_draw_type==DRAW_TYPE::ARRAY){
	glBindVertexArray(m_vertex_array_id);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_position_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));
	

    
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer_id);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));

    glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer_id);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));
        
    }
    else if (m_draw_type==DRAW_TYPE::LINE){
        glBindVertexArray(m_vertex_array_id);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_position_buffer_id);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));
        
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer_id);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer_id);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), ((GLvoid*)(0)));
    }
	if (m_draw_type == DRAW_TYPE::ARRAY)
	{
		
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei) m_positions.size());
        

	}
	else if (m_draw_type== DRAW_TYPE::LINE){
		
        //glEnable(GL_LINE_SMOOTH);
        //glDrawArrays(GL_LINES, 0, (GLsizei) m_positions.size());
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES,0,(GLsizei)m_positions.size());
        //printf("am I called? %d \n",(int)m_positions.size());
	}
	else {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id);
		glDrawElements(GL_TRIANGLES, (GLsizei) m_indices.size(), GL_UNSIGNED_INT, ((GLvoid *)0));
	}
}

void Model::CleanUp()
{
	// Clean up data structures
	m_positions.clear();
	m_positions.shrink_to_fit();

	m_indices.clear();
	m_indices.shrink_to_fit();

	m_normals.clear();
	m_normals.shrink_to_fit();

	m_colors.clear();
	m_colors.shrink_to_fit();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &m_vertex_array_id);
	glDeleteBuffers(1, &m_normal_buffer_id);
	glDeleteBuffers(1, &m_color_buffer_id);
	if (m_draw_type == DRAW_TYPE::INDEX) glDeleteBuffers(1, &m_index_buffer_id);
	glDeleteProgram(m_glsl_program_id);
	glDeleteVertexArrays(1, &m_vertex_array_id);
}
mat4* Model::GetRBT(void) {
	return m_model_rbt;
}
