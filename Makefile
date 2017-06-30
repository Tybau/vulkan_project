NAME = vk_project
CC = g++

VULKAN_SDK_PATH = ~/.bin/VulkanSDK/1.0.51.0/x86_64
VULKAN_SDK_INCLUDES = $(VULKAN_SDK_PATH)/include
VULKAN_SDK_LIBS = $(VULKAN_SDK_PATH)/lib

FILES = main.cpp \
		app.cpp

OBJS = $(addprefix bin/,$(FILES:.cpp=.o))

INCLUDES = -I includes/ -I $(VULKAN_SDK_INCLUDES)

FLAGS = -std=c++11 $(INCLUDES)
LDFLAGS = -L $(VULKAN_SDK_LIBS) -lvulkan -L libs/ -lglfw3 -lGL -lm -ldl -lXinerama -lXrandr -lXi -lXcursor -lX11 -lXxf86vm -lpthread

$(NAME):bin $(OBJS)
	$(CC) $(CXXFLAGS) -o $(NAME) $(OBJS) $(INCLUDES) $(LDFLAGS)

all: $(NAME)

bin:
	mkdir -p $(dir $(OBJS))

bin/%.o: src/%.cpp
	$(CC) $(CXXFLAGS) -c $^ -o $@ $(INCLUDES)

test: all
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/explicit_layer.d ./VulkanTest

clean:
	rm -rf bin

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all test clean fclean re
