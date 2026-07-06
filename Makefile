# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98

# Target executable
TARGET = webserv

# Source files
SRCS =	$(addprefix src/, main.cpp webserv.cpp) \
	$(addprefix src/reactor/, reactor.cpp epoll.cpp) \
	$(addprefix src/handlers/, event_handler.cpp listen_handler.cpp \
	conn_handler.cpp cgi_handler.cpp cgi_in_handler.cpp) \
	$(addprefix src/http_protocol/, http_parser.cpp http_request.cpp \
	http_response.cpp route_resolve.cpp route_result.cpp router.cpp \
	static_handler.cpp) \
	$(addprefix src/config/, config.cpp config_lexer.cpp config_parser.cpp)

DIRS =	src src/reactor src/handlers src/http_protocol src/config

INCLUDES = $(addprefix -I, $(DIRS))

# Dossier pour Object files
D_BUILD = .build/

# Object files
OBJS = $(SRCS:%.cpp=$(D_BUILD)%.o)

# Default rule to build and run the executable
all: $(TARGET)

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .cpp files into .o files
$(D_BUILD)%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

# Clean rule to remove generated files
clean:
	rm -r $(D_BUILD)

fclean: clean
	rm -f $(TARGET)

re: fclean all

# Rule to run the executable
run: $(TARGET)
	./$(TARGET) src/Configuration.conf
