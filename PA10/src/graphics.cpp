
#include "graphics.h"
#include <imgui.h>
#include <stb_image.h>
#include <SDL2/SDL.h>

float cubemap_points[] = {
  -10.0f,  10.0f, -10.0f,
  -10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  
  -10.0f, -10.0f,  10.0f,
  -10.0f, -10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f, -10.0f,
  -10.0f,  10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f,
  
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
   
  -10.0f, -10.0f,  10.0f,
  -10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f, -10.0f,  10.0f,
  -10.0f, -10.0f,  10.0f,
  
  -10.0f,  10.0f, -10.0f,
   10.0f,  10.0f, -10.0f,
   10.0f,  10.0f,  10.0f,
   10.0f,  10.0f,  10.0f,
  -10.0f,  10.0f,  10.0f,
  -10.0f,  10.0f, -10.0f,
  
  -10.0f, -10.0f, -10.0f,
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f, -10.0f,
   10.0f, -10.0f, -10.0f,
  -10.0f, -10.0f,  10.0f,
   10.0f, -10.0f,  10.0f
};

Graphics::Graphics() {

	m_v_light_shader = nullptr;
	m_f_light_shader = nullptr;
	m_cubemap_shader = nullptr;
}

Graphics::~Graphics() {

	if(m_v_light_shader) {

		delete m_v_light_shader;
		m_v_light_shader = nullptr;
	}
	if(m_f_light_shader) {

		delete m_f_light_shader;
		m_f_light_shader = nullptr;
	}	
	if(m_cubemap_shader) {

		delete m_cubemap_shader;
		m_cubemap_shader = nullptr;
	}

	glDeleteBuffers(1, &cubemap_vbo);
	glDeleteTextures(1, &cubemap_tex);
	glDeleteVertexArrays(1, &cubemap_vao);
	glDeleteVertexArrays(1, &object_VAO);
}

bool Graphics::ReloadShaders() {

	if(m_v_light_shader) delete m_v_light_shader;
	if(m_f_light_shader) delete m_f_light_shader;
	if(m_cubemap_shader) delete m_cubemap_shader;

	{
		m_v_light_shader = new Shader();
		if(!m_v_light_shader->Initialize()) {

			std::cerr << "Shader Failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the vertex shader
		if(!m_v_light_shader->AddShader(GL_VERTEX_SHADER, "../data/shaders/v_light.v")) {
		
			std::cerr << "Vertex Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the fragment shader
		if(!m_v_light_shader->AddShader(GL_FRAGMENT_SHADER, "../data/shaders/v_light.f")) {
		
			std::cerr << "Fragment Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Connect the program
		if(!m_v_light_shader->Finalize()) {

			std::cerr << "Program to Finalize" << std::endl;
			return false;
		}
	}
	{
		m_f_light_shader = new Shader();
		if(!m_f_light_shader->Initialize()) {

			std::cerr << "Shader Failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the vertex shader
		if(!m_f_light_shader->AddShader(GL_VERTEX_SHADER, "../data/shaders/f_light.v")) {
		
			std::cerr << "Vertex Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the fragment shader
		if(!m_f_light_shader->AddShader(GL_FRAGMENT_SHADER, "../data/shaders/f_light.f")) {
		
			std::cerr << "Fragment Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Connect the program
		if(!m_f_light_shader->Finalize()) {

			std::cerr << "Program to Finalize" << std::endl;
			return false;
		}
	}
	{
		m_cubemap_shader = new Shader();
		if(!m_cubemap_shader->Initialize()) {

			std::cerr << "Shader Failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the vertex shader
		if(!m_cubemap_shader->AddShader(GL_VERTEX_SHADER, "../data/shaders/cubemap.v")) {
		
			std::cerr << "Vertex Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Add the fragment shader
		if(!m_cubemap_shader->AddShader(GL_FRAGMENT_SHADER, "../data/shaders/cubemap.f")) {
		
			std::cerr << "Fragment Shader failed to Initialize" << std::endl;
			return false;
		}
		
		// Connect the program
		if(!m_cubemap_shader->Finalize()) {

			std::cerr << "Program to Finalize" << std::endl;
			return false;
		}
	}

	return true;
}

bool Graphics::Initialize(int width, int height) {
  
	glewExperimental = GL_TRUE;

	w = width; h = height;
	auto status = glewInit();

	// This is here to grab the error that comes from glew init.
	// This error is an GL_INVALID_ENUM that has no effects on the performance
	glGetError();

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debug_proc, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

	std::cout << "GL_VERSION: " << (const char*)glGetString(GL_VERSION) << std::endl;
	std::cout << "GL_RENDERER: " << (const char*)glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VENDOR: " << (const char*)glGetString(GL_VENDOR) << std::endl;

	//Check for error
	if (status != GLEW_OK) {
	  
		std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
		return false;
	}

	// For OpenGL 3
	glGenVertexArrays(1, &object_VAO);

	// Init Camera
	camera_type = CameraType::orbit;

	// Set up the shaders
	if(!ReloadShaders()) {
		return false;
	}

	// load cube map texture
	if(!CreateCubeMap("../data/textures/skybox.png")) {
		return false;
	}

	orbit_camera.lock = true;

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	
	return true;
}

void Graphics::Update(unsigned int dt) {

	static const unsigned char* keys = SDL_GetKeyboardState(NULL);
	
	free_camera.last_update += dt;

	if(camera_type == CameraType::free) {
		float dT = dt / 1000.0f;

		if (keys[SDL_SCANCODE_W]) {
			free_camera.pos += free_camera.front * free_camera.speed * dT;
		}
		if (keys[SDL_SCANCODE_S]) {
			free_camera.pos -= free_camera.front * free_camera.speed * dT;
		}
		if (keys[SDL_SCANCODE_A]) {
			free_camera.pos -= free_camera.right * free_camera.speed * dT;
		}
		if (keys[SDL_SCANCODE_D]) {
			free_camera.pos += free_camera.right * free_camera.speed * dT;
		}

		free_camera.update();
	}
}

bool Graphics::CreateCubeMap(std::string file) {

	glGenBuffers(1, &cubemap_vbo);
	glGenVertexArrays(1, &cubemap_vao);
	glGenTextures(1, &cubemap_tex);

	glBindVertexArray(cubemap_vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubemap_vbo);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
	glActiveTexture(GL_TEXTURE0);

	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &cubemap_points, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	int x, y, n;
	unsigned char* image = stbi_load(file.c_str(), &x, &y, &n, 4);
	if (!image) {
		std::cerr << "Error loading cubemap texture " << file << std::endl;
		return false;
	}

	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindVertexArray(0);

	return true;
}

Camera* Graphics::GetCamera() {
	switch(camera_type) {
	case CameraType::orbit: 	return &orbit_camera;
	case CameraType::free: 		return &free_camera;
	default: return nullptr;
	}
}

void Graphics::MoveCamera(int dx, int dy) {
	
	Camera* c = GetCamera();
	c->move(dx, dy);
}

void Graphics::SetCameraDistance(float d) {
	
	Camera* c = GetCamera();
	c->setDistance(d);
}

void Graphics::Clear() {
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 Graphics::GetView() {

	Camera* c = GetCamera();
	return c->GetView();
}

glm::mat4 Graphics::GetProj() {

	Camera* c = GetCamera();
	return c->GetProjection(w,h);
}

void Graphics::UpdateWH(int _w, int _h) {
	w = _w;
	h = _h;
}

void Graphics::RenderSkybox() {

	Camera* c = GetCamera();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glBindVertexArray(cubemap_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
	m_cubemap_shader->Enable();
	glUniformMatrix4fv(m_cubemap_shader->GetUniformLocation("proj"), 1, GL_FALSE, glm::value_ptr(c->GetProjection(w, h))); 
	glUniformMatrix4fv(m_cubemap_shader->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(c->GetViewWithoutTranslate())); 
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

ShaderInfo Graphics::BeginObjectRender() {

	Camera* c = GetCamera();

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShaderInfo m;
	m.default_ambient = default_ambient;
	
	glBindVertexArray(object_VAO);

	if(light_type == LightingType::vertex) {
		m_v_light_shader->Enable();
		m.shader = m_v_light_shader;

		glUniformMatrix4fv(m_v_light_shader->GetUniformLocation("proj"), 1, GL_FALSE, glm::value_ptr(c->GetProjection(w, h))); 
		glUniformMatrix4fv(m_v_light_shader->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(c->GetView())); 

	} else {
		m_f_light_shader->Enable();
		m.shader = m_f_light_shader;

		glUniformMatrix4fv(m_f_light_shader->GetUniformLocation("proj"), 1, GL_FALSE, glm::value_ptr(c->GetProjection(w, h))); 
		glUniformMatrix4fv(m_f_light_shader->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(c->GetView())); 
	}
	
	return m;
}

void Graphics::UI() {

	ImGui::Separator();
	Camera* c = GetCamera();
	ImGui::Text("Camera");
	ImGui::SliderFloat("FOV", &c->fov, 10.0f, 150.0f); 
 	static const char* cameras[] = {"Orbit Camera", "Free Camera"};
	ImGui::Combo("Camera Type", (int*)&camera_type, cameras, 2);
	ImGui::Checkbox("Lock", &c->lock);
	if(c == &orbit_camera) {
		if(ImGui::SliderFloat("Radius", &orbit_camera.radius, 10.0f, 30.0f)) {
			orbit_camera.update();
		}
	}
	if(c == &free_camera) {
		ImGui::SliderFloat("Speed", &free_camera.speed, 5.0f, 100.0f);
	}

	ImGui::Separator();
	ImGui::Text("Lighting");
	static const char* lightings[] = {"Per-Vertex", "Per-Fragment"};
	ImGui::Combo("Lighting Type", (int*)&light_type, lightings, 2);

	if(ImGui::CollapsingHeader("Ambient Light")) {
		ImGui::Indent();	
		ImGui::SliderFloat("R", &default_ambient.x, 0.0f, 1.0f);
		ImGui::SliderFloat("G", &default_ambient.y, 0.0f, 1.0f);
		ImGui::SliderFloat("B", &default_ambient.z, 0.0f, 1.0f);
		ImGui::Unindent();
	}
}

void Graphics::EndRender() {

	ImGui::Render();
}

void debug_proc(GLenum glsource, GLenum gltype, GLuint id, GLenum severity, GLsizei length, const GLchar* glmessage, const void* up) {

	std::string message((char*)glmessage);
	std::string source, type;

	switch(glsource) {
	case GL_DEBUG_SOURCE_API:
		source = "OpenGL API";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		source = "Window System";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		source = "Shader Compiler";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		source = "Third Party";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		source = "Application";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		source = "Other";
		break;
	}

	switch(gltype) {
	case GL_DEBUG_TYPE_ERROR:
		type = "Error";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		type = "Deprecated";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		type = "Undefined Behavior";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		type = "Portability";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		type = "Performance";
		break;
	case GL_DEBUG_TYPE_MARKER:
		type = "Marker";
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		type = "Push Group";
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		type = "Pop Group";
		break;
	case GL_DEBUG_TYPE_OTHER:
		type = "Other";
		break;
	}

	switch(severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MED OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW OpenGL: " << message << " SOURCE: " << source << " TYPE: " << type << std::endl;
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		break;
	}
}
