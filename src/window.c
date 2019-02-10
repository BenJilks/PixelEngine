#include "window.h"
#include <sys/time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <X11/Xlib.h>
#include <vulkan/vulkan_xlib.h>

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MAX_DEVICE_COUNT 5
#define MAX_QUEUE_COUNT  5

// Store data about a key bind
typedef struct KeyBind
{
	int id;
	int key_code;
	int is_pressed;
} KeyBind;

// Window state data
static int 				has_sdl_init = 0;
static int 				should_close = 0;
static int 				window_width, window_height;
static struct 			timeval last_time;
static float 			delta_time;

// Input event handlers
static KeyEvent 		key_down_event;
static KeyEvent 		key_up_event;
static MouseEvent 		mouse_move_event;
static KeyBind 			key_binds[80];
static int 				key_bind_len;

// SDL window data
static SDL_Window 		*window = NULL;
static VkInstance 		vk_instance = VK_NULL_HANDLE;
static VkSurfaceKHR 	vk_surface = VK_NULL_HANDLE;
static VkPhysicalDevice vk_device;
static uint32_t 		vk_queue_family_index;

// Init all the input event handlers
static void init_input()
{
	key_down_event = NULL;
	key_up_event = NULL;
	mouse_move_event = NULL;
	key_bind_len = 0;
}

const VkApplicationInfo vk_app_info = 
{
	.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "Test Game",
    .pEngineName = "PixelEngine2",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0
};

const VkInstanceCreateInfo vk_create_info = 
{
	.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
	.pApplicationInfo = &vk_app_info,
    .enabledExtensionCount = 1,
    .ppEnabledExtensionNames = (const char* const[]) 
	{ 
		VK_KHR_SURFACE_EXTENSION_NAME
	}
};

// Create vulkan instance and link to window
static void init_vulkan()
{
	VkResult result;

	result = vkCreateInstance(&vk_create_info, 0, &vk_instance);
	if (result != VK_SUCCESS)
	{
		printf("Error: Could not create vulkan instance\n");
	}

	if (!SDL_Vulkan_CreateSurface(window, vk_instance, &vk_surface))
	{
		printf("Error: Could not create vulkan surface (%s)\n", 
			SDL_GetError());
	}
}

static void print_device_info(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	char *device_type_name = "Unkown";

	vkGetPhysicalDeviceProperties(device, &properties);
	switch(properties.deviceType)
	{
		case VK_PHYSICAL_DEVICE_TYPE_CPU: device_type_name = "CPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: device_type_name = "Intergrated GPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: device_type_name = "Discrete GPU"; break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: device_type_name = "Virtual GPU"; break;
	}

	printf("Device: %s '%s'\n", device_type_name, properties.deviceName);	
}

// Select a sutable vulkan compute device
static void init_device()
{
	uint32_t 				physical_device_count, i, j;
	uint32_t 				queue_family_count;
	VkBool32				supports_present;
	VkQueueFamilyProperties queue_family_properties[MAX_QUEUE_COUNT];
	VkPhysicalDevice 		device_handles[MAX_DEVICE_COUNT];

	// Find number of devices
	vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, 0);
	physical_device_count = MIN(physical_device_count, MAX_DEVICE_COUNT);

	// Iterate through each device
	vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, device_handles);
	for (i = 0; i < physical_device_count; i++)
	{
		print_device_info(device_handles[i]);

		// Fetch the number of properties
		vkGetPhysicalDeviceQueueFamilyProperties(device_handles[i], 
			&queue_family_count, NULL);
		queue_family_count = MIN(queue_family_count, MAX_QUEUE_COUNT);

		// Iterate through each property
		vkGetPhysicalDeviceQueueFamilyProperties(device_handles[i], 
			&queue_family_count, queue_family_properties);
		for (j = 0; j < queue_family_count; j++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(device_handles[i], 
				j, vk_surface, &supports_present);
			
			if (supports_present && (queue_family_properties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			{
				vk_queue_family_index = j;
				vk_device = device_handles[i];
			}
		}

		// If a device was found, break
		if (vk_device)
			break;
	}
}

void window_open(const char *title, int width, int height)
{
	// Only init sdl if at has not already been
	if (!has_sdl_init)
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		has_sdl_init = 1;
	}

	// Save window data
	should_close = 0;
	window_width = width;
	window_height = height;

	// Create SDL window in the center of the screen
	window = SDL_CreateWindow(title, 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

	// Init everything
	init_input();
	init_vulkan();
	init_device();

	// Start game clock
	gettimeofday(&last_time, 0);
	delta_time = 0;
}

int window_should_close()
{
	return should_close;
}

float window_delta_time()
{
	return delta_time;
}

void window_get_size(int *width, int *height)
{
	*width = window_width;
	*height = window_height;
}

void window_grab_mouse()
{
	if (SDL_SetRelativeMouseMode(SDL_TRUE) != 0)
		printf("Error: %s\n", SDL_GetError());
}

void window_force_close()
{
	should_close = 1;
}

void window_on_key_down(KeyEvent event) { key_down_event = event; }
void window_on_key_up(KeyEvent event) { key_up_event = event; }
void window_on_mouse_move(MouseEvent event) { mouse_move_event = event; }

void window_bind_key(int id, int key_code)
{
	KeyBind bind;
	bind.id = id;
	bind.key_code = key_code;
	bind.is_pressed = 0;
	key_binds[key_bind_len++] = bind;
}

int window_is_bind_pressed(int id)
{
	int i;
	for (i = 0; i < key_bind_len; i++)
		if (key_binds[i].id == id)
			return key_binds[i].is_pressed;
	return 0;
}

static void change_key_state(int key_code, int state)
{
	int i;
	for (i = 0; i < key_bind_len; i++)
		if (key_binds[i].key_code == key_code)
			key_binds[i].is_pressed = state;
}

void window_update()
{
	SDL_Event e;
	int key_code;
	
	while (SDL_PollEvent(&e))
	{
		key_code = e.key.keysym.sym;
		switch(e.type)
		{
			case SDL_QUIT:
				should_close = 1;
				break;
			
			case SDL_KEYDOWN:
				if (key_down_event != NULL)
					key_down_event(key_code);
				change_key_state(key_code, 1);
				break;
			
			case SDL_KEYUP:
				if (key_up_event != NULL)
					key_up_event(key_code);
				change_key_state(key_code, 0);
				break;
			
			case SDL_MOUSEMOTION:
				if (mouse_move_event != NULL)
					mouse_move_event(
						e.motion.xrel, 
						e.motion.yrel);
				break;
		}
	}



	struct timeval current_time;
	gettimeofday(&current_time, 0);
	delta_time = (current_time.tv_usec - last_time.tv_usec) / 1000.0f;
	delta_time = MAX(0, delta_time);
	last_time = current_time;
}

void window_close()
{
	if (has_sdl_init)
	{
		SDL_Quit();
		has_sdl_init = 0;
	}

	if (window != NULL)
	{
		SDL_DestroyWindow(window);
		vkDestroySurfaceKHR(vk_instance, vk_surface, 0);
		vkDestroyInstance(vk_instance, 0);
		window = NULL;
	}
}
