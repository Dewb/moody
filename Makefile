program_NAME := runmoody
SOURCES = $(wildcard src/*.cpp)
OBJECTS = ${SOURCES:.cpp=.o}

CPPFLAGS = -g -Isrc

all: $(program_NAME)

$(program_NAME): $(OBJECTS)
	$(LINK.cc) $(OBJECTS) -o $(program_NAME)

