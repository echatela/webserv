# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98

# Target executable
TARGET = webserv

# Source files
SRCS =	$(wildcard src/*.cpp) \
		$(wildcard src/config/*.cpp) \
		$(wildcard src/http_protocol/*.cpp) 

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
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -r $(D_BUILD)

fclean: clean
	rm -f $(TARGET)

re: fclean all

# Rule to run the executable
run: $(TARGET)
	./$(TARGET) src/Configuration.conf
