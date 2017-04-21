#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;
map < string, mesh> meshes;
map < string, texture> textures;
map<string, int> effects;
geometry shape;
geometry shape2;
geometry screen_quad;
effect eff;
effect gray_eff;
effect l_eff; 
effect skybox; 
effect mask_eff;    
effect tv_eff;
effect additional_eff;
texture mask;
texture tex;
mesh sky_mesh;
mesh rm;
free_camera cam;
target_camera camera2;
double cursor_x = 1.0f;
double cursor_y = 1.0f;
cubemap cube_map;
bool cameras = true;
bool goingup;
bool shapegoingup;
bool shapegoingup2;
bool wire;
point_light light;
spot_light S_light;
float theta = 0.0f;
float rho = 0.0f;
vector<point_light> points(4);
vector<spot_light> spots(5);
frame_buffer frame;
frame_buffer frame2;
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
	//main frame buffer
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	//frame buffer for the additional effect
	frame2 = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());
	// Create screen quad
	vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	// *********************************
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);
	//Point light 0
	points[0].set_position(vec3(-3.0f, 1.0f, 3.0f));
	points[0].set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	points[0].set_range(20.0f);
	// Spot 0
	spots[0].set_position(vec3(5.4f, 1.98f, 0.0f));
	spots[0].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	spots[0].set_direction(normalize(vec3(-3.0f, 1.0f, -1.0f)));
	spots[0].set_range(20.0f);
	spots[0].set_power(1.5f);
	mask = texture("textures/mask.jpg");
	// Load in shaders
	gray_eff.add_shader("shaders/simple_texture.vert", GL_VERTEX_SHADER);
	gray_eff.add_shader("shaders/greyscale.frag", GL_FRAGMENT_SHADER);
	tv_eff.add_shader("shaders/simple_texture.vert", GL_VERTEX_SHADER);
	tv_eff.add_shader("shaders/simple_texture.frag", GL_FRAGMENT_SHADER);
	mask_eff.add_shader("shaders/postprocess.vert", GL_VERTEX_SHADER);
	mask_eff.add_shader("shaders/mask.frag", GL_FRAGMENT_SHADER);
	l_eff.add_shader("shaders/multi-light.vert", GL_VERTEX_SHADER);
	l_eff.add_shader("shaders/multi-light.frag", GL_FRAGMENT_SHADER);
	//build effects
	l_eff.build();
	mask_eff.build();
	gray_eff.build();
	tv_eff.build();
	//load and texture the plane
	meshes["plane"] = mesh(geometry_builder::create_plane());
	meshes["plane"].get_transform().scale = vec3(0.5f, 0.5f, 0.5f);
	textures["plane"] = texture("textures/lava.jpg");
	//creates a plane to go infront of the tv(shelved for now)
	meshes["tvplane"] = mesh(geometry_builder::create_plane());
	meshes["tvplane"].get_transform().scale = vec3(0.1, 0.1f, 0.1f);
	meshes["tvplane"].get_transform().translate(vec3(10.5f, 15.5f, 7.0f));
	meshes["tvplane"].get_transform().rotate(vec3(1.6f, 3.05f, 0.0f));
	textures["tvplane"] = texture("textures/sofa.jpg");
	//load the ceiling lamp, change its position and texture it
	meshes["clamp"] = mesh(geometry("models/ceilinglamp.obj"));
	meshes["clamp"].get_transform().scale = vec3(0.05f, 0.05f, 0.05f);
	meshes["clamp"].get_transform().translate(vec3(4.0f, 7.5f, 0.0f));
	textures["clamp"] = texture("textures/sofa.jpg");
	//load the new sofa, change its position and texture it
	meshes["sofa_replacement"] = mesh(geometry("models/sofa_replacement.obj"));
	meshes["sofa_replacement"].get_transform().scale = vec3(0.03f, 0.03f, 0.03f);
	meshes["sofa_replacement"].get_transform().position = vec3(10.0f, 0.0f, 1.0f);
	textures["sofa_replacement"] = texture("textures/sofa.jpg");
	//texture the rectangle
	textures["rectangle"] = texture("textures/red.jpg");
	textures["rectangle2"] = texture("textures/red.jpg");
	//load the table, change its position and texture it
	meshes["table"] = mesh(geometry("models/table.obj"));
	meshes["table"].get_transform().scale = vec3(0.5f, 0.5f, 0.5f);
	meshes["table"].get_transform().translate(vec3(5.0f, 0.82f, 0.0f));
	textures["table"] = texture("textures/table.bmp");
	//load the window, change its position and texture it
	meshes["window"] = mesh(geometry("models/window.obj"));
	meshes["window"].get_transform().scale = vec3(0.003f, 0.003f, 0.003f);
	meshes["window"].get_transform().position = vec3(-4.5f, 2.5f, -3.0f);
	meshes["window"].get_transform().rotate(vec3(0.0f, 4.7f, 0.0f));
	textures["window"] = texture("textures/window.jpg");
	//texture a wall
	textures["wall"] = texture("textures/brick.jpg");
	//load the lamp, change its position and texture it
	meshes["lamp"] = mesh(geometry("models/lamp.obj"));
	meshes["lamp"].get_transform().scale = vec3(0.001f, 0.001f, 0.001f);
	meshes["lamp"].get_transform().translate(vec3(3.4f, 1.98f, 0.0f));
	meshes["lamp"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["lamp"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["lamp"].get_material().set_shininess(2500.0f);
	textures["lamp"] = texture("textures/table.bmp");
	//load the tv, change its position and texture it
	meshes["TV"] = mesh(geometry("models/TV.obj"));
	meshes["TV"].get_transform().scale = vec3(0.01f, 0.01f, 0.01f);
	meshes["TV"].get_transform().translate(vec3(10.5f, 2.5f, 7.0f));
	textures["TV"] = texture("textures/sofa.jpg");
	// Load the skybox mesh
	sky_mesh = mesh(geometry_builder::create_box(vec3(1.0f, 1.0f, 1.0f)));
	sky_mesh.get_transform().scale = vec3(50.0f, 50.0f, 50.0f);
	array<string, 6> filenames = { "textures/sahara_ft.jpg", "textures/sahara_bk.jpg", "textures/sahara_up.jpg",
		"textures/sahara_dn.jpg", "textures/sahara_rt.jpg", "textures/sahara_lf.jpg" };
	cube_map = cubemap(filenames);
	skybox.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	skybox.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	skybox.build();
	//create the rectangle
	vector<vec3> positionsss{
	vec3(-1.0f, 1.9f, 1.0f),
	vec3(-1.0f, -5.0f, 1.0f),
	vec3(1.0f, -5.0f, 1.0f),
	vec3(-1.0f, 1.9f, 1.0f),
	vec3(1.0f, -5.0f, 1.0f),
	vec3(1.0f, 1.9f, 1.0f),
	vec3(1.0f, 1.9f, -1.0f),
	vec3(1.0f, -5.0f, -1.0f),
	vec3(-1.0f, -5.0f, -1.0f),
	vec3(1.0f, 1.9f, -1.0f),
	vec3(-1.0f, -5.0f, -1.0f),
	vec3(-1.0f, 1.9f, -1.0f),
	vec3(1.0f, 1.9f, 1.0f),
	vec3(1.0f, -5.0f, 1.0f),
	vec3(1.0f, -5.0f, -1.0f),
	vec3(1.0f, 1.9f, 1.0f),
	vec3(1.0f, -5.0f, -1.0f),
	vec3(1.0f, 1.9f, -1.0f),
	vec3(-1.0f, 1.9f, -1.0f),
	vec3(-1.0f, -5.0f, -1.0f),
	vec3(-1.0f, -5.0f, 1.0f),
	vec3(-1.0f, 1.9f, -1.0f),
	vec3(-1.0f, -5.0f, 1.0f),
	vec3(-1.0f, 1.9f, 1.0f),
	vec3(-1.0f, 1.9f, -1.0f),
	vec3(-1.0f, 1.9f, 1.0f),
	vec3(1.0f, 1.9f, 1.0f),
	vec3(-1.0f, 1.9f, -1.0f),
	vec3(1.0f, 1.9f, 1.0f),
	vec3(1.0f, 1.9f, -1.0f),

	};
	//Vec 3 positions for geometry
	vector<vec3> positionss{
		vec3(-1.0f, 2.0f, 1.0f),
		vec3(-1.0f, -2.0f, 1.0f),
		vec3(1.0f, -5.0f, 1.0f),
		vec3(-1.0f, 2.0f, 1.0f),
		vec3(1.0f, -5.0f, 1.0f),
		vec3(1.0f, 2.0f, 1.0f),
		vec3(1.0f, 2.0f, -1.0f),
		vec3(1.0f, -5.0f, -1.0f),
		vec3(-1.0f, -5.0f, -1.0f),
		vec3(1.0f, 2.0f, -1.0f),
		vec3(-1.0f, -5.0f, -1.0f),
		vec3(-1.0f, 2.0f, -1.0f),
		vec3(1.0f, 2.0f, 1.0f),
		vec3(1.0f, -5.0f, 1.0f),
		vec3(1.0f, -5.0f, -1.0f),
		vec3(1.0f, 2.0f, 1.0f),
		vec3(1.0f, -5.0f, -1.0f),
		vec3(1.0f, 2.0f, -1.0f),
		vec3(-1.0f, 2.0f, -1.0f),
		vec3(-1.0f, -5.0f, -1.0f),
		vec3(-1.0f, -5.0f, 1.0f),
		vec3(-1.0f, 2.0f, -1.0f),
		vec3(-1.0f, -5.0f, 1.0f),
		vec3(-1.0f, 2.0f, 1.0f),
		vec3(-1.0f, 2.0f, -1.0f),
		vec3(-1.0f, 2.0f, 1.0f),
		vec3(1.0f, 2.0f, 1.0f),
		vec3(-1.0f, 2.0f, -1.0f),
		vec3(1.0f, 2.0f, 1.0f),
		vec3(1.0f, 2.0f, -1.0f),
	};
	//Vec 3 positions for geometry
	vector<vec3> walpos{
		vec3(-1.0f, 7.0f, 10.0f),
		vec3(-1.0f, -7.0f, 10.0f),
		vec3(1.0f, -7.0f, 10.0f),
		vec3(-1.0f, 7.0f, 10.0f),
		vec3(1.0f, -7.0f, 10.0f),
		vec3(1.0f, 7.0f, 10.0f),
		vec3(-1.0f, 7.0f, -10.0f),
		vec3(-1.0f, -1.0f, -10.0f),
		vec3(-1.0f, -1.0f, 10.0f),
		vec3(-1.0f, 7.0f, -10.0f),
		vec3(-1.0f, -1.0f, 10.0f),
		vec3(-1.0f, 7.0f, 10.0f),


	};

	// Cube colours
	vector<vec4> colours;
	for (auto i = 0; i < positionsss.size(); ++i) {
		colours.push_back(vec4(1.0, 0.0, 0.0f, 1.0f));

	}
	vector<vec4> colourss;
	for (auto i = 0; i < positionss.size(); ++i) {
		colourss.push_back(vec4(1.0, 0.0, 0.0f, 1.0f));

	}
	//
	vector<vec4> wallcolours;
	for (auto i = 0; i < walpos.size(); ++i) {
		wallcolours.push_back(vec4(1.0, 0.0, 0.0f, 1.0f));
	}
	// Add to the geometry
	shape.add_buffer(positionsss, BUFFER_INDEXES::POSITION_BUFFER);
	shape.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
	meshes["rectangle"] = mesh(shape);
	//add to the geometry
	shape2.add_buffer(positionss, BUFFER_INDEXES::POSITION_BUFFER);
	shape2.add_buffer(colours, BUFFER_INDEXES::COLOUR_BUFFER);
	meshes["rectangle2"] = mesh(shape2);
	//Create, position, and texture a wall
	meshes["wall"] = mesh(geometry_builder::create_box());
	meshes["wall"].get_transform().translate(vec3(-5.0f, 0.0f, 3.0f));
	meshes["wall"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall"].get_transform().scale = vec3(0.5f, 15.0f, 10.0f);
	textures["wall"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall2"] = mesh(geometry_builder::create_box());
	meshes["wall2"].get_transform().translate(vec3(4.2f, 0.0f, -13.0f));
	meshes["wall2"].get_transform().rotate(vec3(0.0f, 17.26f, 0.0f));
	meshes["wall2"].get_transform().scale = vec3(10.0f, 15.0f, 35.0f);
	textures["wall2"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall3"] = mesh(geometry_builder::create_box());
	meshes["wall3"].get_transform().translate(vec3(4.2f, 0.0f, 13.0f));
	meshes["wall3"].get_transform().rotate(vec3(0.0f, 17.26f, 0.0f));
	meshes["wall3"].get_transform().scale = vec3(10.0f, 15.0f, 35.0f);
	textures["wall3"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall4"] = mesh(geometry_builder::create_box());
	meshes["wall4"].get_transform().translate(vec3(22.0f, 0.0f, 1.0f));
	meshes["wall4"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall4"].get_transform().scale = vec3(1.0f, 15.0f, 35.0f);
	textures["wall4"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall5"] = mesh(geometry_builder::create_box());
	meshes["wall5"].get_transform().translate(vec3(20.0f, 8.0f, 7.0f));
	meshes["wall5"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall5"].get_transform().scale = vec3(50.0f, 1.0f, 30.0f);
	textures["wall5"] = texture("textures/ceiling.jpg");
	//Create, position, and texture a wall
	meshes["wall6"] = mesh(geometry_builder::create_box());
	meshes["wall6"].get_transform().translate(vec3(-4.7f, 0.0f, -9.3f));
	meshes["wall6"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall6"].get_transform().scale = vec3(0.5f, 15.0f, 10.0f);
	textures["wall6"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall7"] = mesh(geometry_builder::create_box());
	meshes["wall7"].get_transform().translate(vec3(-4.5f, 0.0f, -3.0f));
	meshes["wall7"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall7"].get_transform().scale = vec3(0.0001f, 5.3f, 4.0f);
	textures["wall7"] = texture("textures/brick.jpg");
	//Create, position, and texture a wall
	meshes["wall8"] = mesh(geometry_builder::create_box());
	meshes["wall8"].get_transform().translate(vec3(-4.5f, 3.0f, 4.0f));
	meshes["wall8"].get_transform().rotate(vec3(0.0f, 62.82f, 0.0f));
	meshes["wall8"].get_transform().scale = vec3(0.1f, 6.0f, 3.0f);
	textures["wall8"] = texture("textures/door.jpg");
	//Transalte rectangles
	meshes["rectangle"].get_transform().translate(vec3(0.0f, -1.0f, 0.0f));
	meshes["rectangle2"].get_transform().translate(vec3(16.0f, -1.0f, 5.0f));

	// Set target camera properties
	camera2.set_position(vec3(0.0f, 1.0f, 0.0f));
	camera2.set_target(vec3(10.0f, 1.0f, 10.0f));
	camera2.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	//set free camera properties
	cam.set_position(vec3(0.0f, 2.0f, 0.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	//Bolleans for shape and lamp movement
	goingup = true;
	shapegoingup = true;
	shapegoingup2 = true;

	return true;

}
void maskk()
{
	//set render target back to screen
	renderer::set_render_target();
	//bind the masking effect
	renderer::bind(mask_eff);
	//MVP is identity matrix
	auto MVP = mat4(1.0f);
	//set MVP matrix uniform
	glUniformMatrix4fv(mask_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	//bind texture to frame buffer
	renderer::bind(frame.get_frame(), 0);
	//set tex unifrom
	glUniform1i(mask_eff.get_uniform_location("tex"), 0);
	//bind mask to alpha map
	renderer::bind(mask, 1);
	glUniform1i(mask_eff.get_uniform_location("alpha_map"), 1);
	//render screen quad
	renderer::render(screen_quad);
}
void grayscale()
{
	renderer::set_render_target();
	// Bind Tex effect
	renderer::bind(gray_eff);
	// MVP is now the identity matrix
	auto MVP = mat4(1.0f);
	// Set MVP matrix uniform
	glUniformMatrix4fv(gray_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	// Bind texture from frame buffer
	renderer::bind(frame.get_frame(), 1);
	// Set the tex uniform
	glUniform1i(gray_eff.get_uniform_location("tex"), 1);
	// Render the screen quad
	renderer::render(screen_quad);
}

bool update(float delta_time) {
	//display frame rate to comand prompt
	cout << 1.0 / delta_time << endl;

	static float light_range;
	// The ratio of pixels to rotation - remember the fov
	static double ratio_width = quarter_pi<float>() / static_cast<float>(renderer::get_screen_width());
	static double ratio_height =
		(quarter_pi<float>() *
		(static_cast<float>(renderer::get_screen_height()) / static_cast<float>(renderer::get_screen_width()))) /
		static_cast<float>(renderer::get_screen_height());
	double current_x;
	double current_y;
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
	// Use keyboard to move the camera -WASD
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

	//switch the light off
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_K)) {
		light_range = 0.0f;
		spots[0].set_range(light_range);
		points[0].set_range(light_range);
	}
	//switch the light on
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_L)) {
		light_range = 25.0f;
		spots[0].set_range(light_range);
		points[0].set_range(light_range);
	}
	//switch to overhead camera
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1)) {
		camera2.set_position(vec3(0.0f, 50.0f, 0.0f));
		cameras = false;
	}
	//start displaying the camera feed to a wall
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_2)) {
		effects["additional_eff"] = 1;
	}
	//stop the camera feed and just show a screenshot on the wall
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_3)) {
		effects["additional_eff"] = 0;
	}
	//mask on
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_4)) {
		effects["mask_eff"] = 1;
	}
	//mask off
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_5)) {
		effects["mask_eff"] = 0;
	}
	//Wireframe on
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_6)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	//Wireframe off
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_7)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//grayscale on
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_8)) {
		effects["gray_eff"] = 1;
	}
	//grayscale off
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_9)) {
		effects["gray_eff"] = 0;
	}
	//Lamp movement logic
	if ((goingup) && (meshes["lamp"].get_transform().position.y <= 5.0))
	{
		meshes["lamp"].get_transform().position.y += 0.015;
	}

	if ((!goingup) && (meshes["lamp"].get_transform().position.y >= 1.5))
	{
		meshes["lamp"].get_transform().position.y -= 0.015;
	}

	if (meshes["lamp"].get_transform().position.y >= 5.0)
	{
		goingup = false;
		meshes["lamp"].get_transform().position.y -= 0.015;

	}

	if (meshes["lamp"].get_transform().position.y <= 1.5)
	{
		goingup = true;
		meshes["lamp"].get_transform().position.y += 0.015;
	}
	if ((shapegoingup) && (meshes["rectangle2"].get_transform().position.y <= 2.0))
	{
		meshes["rectangle2"].get_transform().position.y += 0.010;
	}

	if ((!shapegoingup) && (meshes["rectangle2"].get_transform().position.y >= 0.5))
	{
		meshes["rectangle2"].get_transform().position.y -= 0.010;
	}

	if (meshes["rectangle2"].get_transform().position.y >= 2.0)
	{
		shapegoingup = false;
		meshes["rectangle2"].get_transform().position.y -= 0.010;

	}

	if (meshes["rectangle2"].get_transform().position.y <= 0.5)
	{
		shapegoingup = true;
		meshes["rectangle2"].get_transform().position.y += 0.010;
	}
	if ((shapegoingup2) && (meshes["rectangle"].get_transform().position.y <= 4.0))
	{
		meshes["rectangle"].get_transform().position.y += 0.010;
	}

	if ((!shapegoingup2) && (meshes["rectangle"].get_transform().position.y >= 1.5))
	{
		meshes["rectangle"].get_transform().position.y -= 0.010;
	}

	if (meshes["rectangle"].get_transform().position.y >= 4.0)
	{
		shapegoingup2 = false;
		meshes["rectangle"].get_transform().position.y -= 0.010;

	}

	if (meshes["rectangle"].get_transform().position.y <= 1.5)
	{
		shapegoingup2 = true;
		meshes["rectangle"].get_transform().position.y += 0.010;
	}
	//cool spinning table
	meshes["table"].get_transform().rotate(vec3(pi<float>() / 6, 2.0f, 1.0f) * delta_time);
	// Update the camera
	cam.update(delta_time);
	camera2.update(delta_time);
	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;
	return true;
}


bool render() {
	// Clear frame
	//set render target to frame buffer
	if (effects["mask_eff"] == 1)
	{
		renderer::set_render_target(frame);

		//clear frame
		renderer::clear();
	}
	if (effects["gray_eff"] == 1)
	{
		renderer::clear();
		renderer::set_render_target(frame);
		//clear frame
		renderer::clear();
	}
	if (effects["additional_eff"] == 1)
	{
		renderer::clear();
		renderer::set_render_target(frame2);
		//clear frame
		renderer::clear();
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	// Render the skybox
	renderer::bind(skybox);
	//renderer::bind(eff);
	mat4 ME = sky_mesh.get_transform().get_transform_matrix();
	auto VE = cam.get_view();
	auto PE = cam.get_projection();

	if (cameras == true)
	{
		VE = cam.get_view();
		PE = cam.get_projection();
	}
	else
	{
		VE = camera2.get_view();
		PE = camera2.get_projection();
	}
	auto MEVEPE = PE * VE * ME;
	glUniformMatrix4fv(skybox.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MEVEPE));

	renderer::bind(cube_map, 0);
	glUniform1i(skybox.get_uniform_location("cubemap"), 0);
	//render the mesh
	renderer::render(sky_mesh);
	//enable cull face
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(l_eff);
		// Create MVP matrix
		ME = m.get_transform().get_transform_matrix();
		//logic for switching cameras
		MEVEPE = PE * VE * ME;
		// Set MVP matrix uniform
		glUniformMatrix4fv(l_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MEVEPE));
		// Light code
		glUniformMatrix4fv(l_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(ME));
		glUniformMatrix3fv(l_eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		renderer::bind(m.get_material(), "mat");
		// Bind spot lights
		renderer::bind(spots, "spots");
		//bind the point light
		renderer::bind(points, "points");
		renderer::bind(textures[e.first], 0);
		glUniform1i(l_eff.get_uniform_location("tex"), 0);
		glUniform3fv(l_eff.get_uniform_location("eye_pos"), 1, value_ptr(cam.get_position()));
		//Render meshes
		renderer::render(m);
	}
	//if the grayscale effect has beed triggered
	if (effects["gray_eff"] == 1)
	{
		//run grayscale
		grayscale();
	}
	//if the masking effect has been triggered
	if (effects["mask_eff"] == 1)
	{
		//run mask
		maskk();
	}
	if (effects["additional_eff"] == 1)
	{
		{
			//Uses part of a mask function to capture what the camera sees
			//set render target back to screen
			renderer::set_render_target();
			//bind the masking effect
			renderer::bind(mask_eff);
			//MVP is identity matrix
			auto MVP = mat4(1.0f);
			//set MVP matrix uniform
			glUniformMatrix4fv(mask_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			//bind texture to frame buffer
			renderer::bind(frame2.get_frame(), 0);
			//render screen quad
			renderer::render(screen_quad);
		}
		//function to display the "screenshot" to the wall
		renderer::set_render_target();
		// Bind Tex effect
		renderer::bind(tv_eff);
		// MVP is now the identity matrix
		auto MVP = mat4(1);
		// Set MVP matrix uniform
		glUniformMatrix4fv(tv_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Bind texture from frame buffer
		renderer::bind(frame2.get_frame(), 1);
		// Set the tex uniform
		glUniform1i(tv_eff.get_uniform_location("tex"), 1);
		//display screenshot on the wall
		textures["wall4"] = texture(frame2.get_frame());

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