program_NAME := runmoody
SOURCES = $(wildcard *.cpp)
OBJECTS = ${SOURCES:.cpp=.o}

CPPFLAGS = -g

all: $(program_NAME)

$(program_NAME): $(OBJECTS)
	$(LINK.cc) $(OBJECTS) -o $(program_NAME)

