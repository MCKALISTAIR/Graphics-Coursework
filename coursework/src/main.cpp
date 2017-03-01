#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map < string, mesh> meshes;
map < string, texture> textures;
geometry geom;
effect eff;
effect skybox;
mesh sky_mesh;
free_camera cam;
target_camera camera2;
double cursor_x = 1.0f;
double cursor_y = 1.0f;
cubemap cube_map;
bool cameras = true;


bool initialise() {
	// *********************************
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position
	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	// *********************************
	return true;
}

bool load_content() {
	meshes["plane"] = mesh(geometry_builder::create_plane());
	textures["plane"] = texture("textures/lava.jpg"); 
	
	meshes["sofa"] = mesh(geometry("models/sofa.obj"));
	meshes["sofa"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f);
	//meshes["sofa"].get_transform().translate(vec3(6.0f, 0.0f, 0.0f));
	meshes["sofa"].get_transform().position = vec3(40.0f, 0.0f, 47.0f);
	textures["sofa"] = texture("textures/sofa.jpg");

	meshes["table"] = mesh(geometry("models/table.obj"));
	meshes["table"].get_transform().scale = vec3(0.5f, 0.5f, 0.5f);
	meshes["table"].get_transform().translate(vec3(5.0f, 0.82f, 0.0f));
	textures["table"] = texture("textures/table.bmp");
	/*
	meshes["lamp"] = mesh(geometry("models/lamp.obj"));
	meshes["lamp"].get_transform().scale = vec3(0.001f, 0.001f, 0.001f);
	meshes["lamp"].get_transform().translate(vec3(3.4f, 1.98f, 0.0f));
	textures["lamp"] = texture("textures/lamp.jpg");
	*/
	meshes["TV"] = mesh(geometry("models/TV.obj"));
	meshes["TV"].get_transform().scale = vec3(0.01f, 0.01f, 0.01f);
	meshes["TV"].get_transform().translate(vec3(10.5f, 2.5f, 7.0f));
	textures["TV"] = texture("textures/sofa.jpg");
	
	// Load the skybox mesh
	sky_mesh = mesh(geometry_builder::create_box(vec3(1.0f, 1.0f, 1.0f)));
	sky_mesh.get_transform().scale = vec3(100.0f, 100.0f, 100.0f);
	array<string, 6> filenames = { "textures/sahara_ft.jpg", "textures/sahara_bk.jpg", "textures/sahara_up.jpg",
		"textures/sahara_dn.jpg", "textures/sahara_rt.jpg", "textures/sahara_lf.jpg" };
	cube_map = cubemap(filenames);
	skybox.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	skybox.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	skybox.build();

	// Load in shaders
	eff.add_shader("shaders/simple_texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("shaders/simple_texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set camera properties
	camera2.set_position(vec3(0.0f, 10.0f, 0.0f));
	camera2.set_target(vec3(10.0f, 10.0f, 10.0f));
	camera2.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	

	cam.set_position(vec3(0.0f, 10.0f, 0.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}


bool update(float delta_time) {
	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());
	double current_x;
	double current_y;
	// *********************************
	// Get the current cursor position
	current_x = 0;
	current_y = 0;
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);
	// Calculate delta of cursor positions from last frame
	double delta_x = current_x - cursor_x;
	double delta_y = cursor_y - current_y;
	// Multiply deltas by ratios - gets actual change in orientation
	delta_x = delta_x * ratio_width;
	delta_y = delta_y * ratio_height;
	// Rotate cameras by delta
	// delta_y - x-axis rotation
	// delta_x - y-axis rotation
	cam.rotate(delta_x, delta_y);
	// Use keyboard to move the camera - WSAD
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		cam.move(vec3(0.0f, 0.0f, 0.1f));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		cam.move(vec3(-0.1f, 0.0f, 0.0f));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		cam.move(vec3(0.0f, 0.0f, -0.1));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		cam.move(vec3(0.1f, 0.0f, 0.0f));
		cameras = true;
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		camera2.set_position(vec3(0.0f, 50.0f, 0.0f));
		cameras = false;
		
		//if () {							///set bool to true if using a certain camera, update render code with camera2.get projection etc
			//bool usingcamera2 = true
		//}
	}
	// Update the camera
	cam.update(delta_time);
	camera2.update(delta_time);
	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;
	// *********************************
	return true;
}

bool render() {
	// Render the skybox

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	renderer::bind(skybox);
	
	mat4 M = sky_mesh.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	
	glUniformMatrix4fv(skybox.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	
	renderer::bind(cube_map, 0);
	glUniform1i(skybox.get_uniform_location("cubemap"), 0);
	
	renderer::render(sky_mesh);
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix
		M = m.get_transform().get_transform_matrix();
		if (cameras == true)
		{
			V = cam.get_view();
			P = cam.get_projection();
		}
		else
		{
			V = camera2.get_view();
			P = camera2.get_projection();
		}
		MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Render geometry
		// Bind texture to renderer
		renderer::bind(textures[e.first], 0);
		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), 0);
		// *********************************
		// Render the mesh
		renderer::render(m);
	}
	return true;
}

void main() {
  // Create application
  app application("Graphics Coursework");
  // Set load content, update and render methods
  application.set_load_content(load_content);
  application.set_update(update);
  application.set_render(render);
  application.set_initialise(initialise);
  // Run application
  application.run();
}