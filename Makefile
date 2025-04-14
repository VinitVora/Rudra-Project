# Makefile for XOR-based encryption program (rudra.c)

CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2 -g
LDFLAGS = -pthread

# Define the name of the output executable
TARGET = rudra

# Define source files
SRC = rudra.c

# Define object files
OBJ = $(SRC:.c=.o)

# Default target (build the program)
all: $(TARGET)

# Build the program
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target (remove object and executable files)
clean:
	rm -f $(OBJ) $(TARGET)

# Install target (install the binary to /usr/local/bin or other directory)
install: $(TARGET)
	# Install the binary to /usr/local/bin
	cp $(TARGET) /usr/local/bin/
	# Optionally, you can also change the permissions for the binary
	chmod 755 /usr/local/bin/$(TARGET)

# Uninstall target (remove the binary from installation location)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the program (example usage)
run: $(TARGET)
	./$(TARGET) $(ARGS)

# Help target (show available commands)
help:
	@echo "Makefile options:"
	@echo "  all         - Build the program"
	@echo "  clean       - Remove object files and executable"
	@echo "  install     - Install the executable to /usr/local/bin"
	@echo "  uninstall   - Remove the executable from /usr/local/bin"
	@echo "  run         - Run the program with specified arguments (use ARGS)"
	@echo "  help        - Show this help message"
