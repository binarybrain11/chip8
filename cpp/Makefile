CC = g++
TARGET = chip8
SRCDIR = src
OBJDIR = obj
INCLUDEDIR = include
FLAGS = $(INCLUDEDIR:%=-I %)
SOURCES = $(wildcard $(SRCDIR)/*.c) 
SOURCES += $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)%.c=$(OBJDIR)%.o)
# INCLUDES = $(SOURCES:$(SRCDIR)%.c=$(INCLUDEDIR)%.h)


.PHONY: all 
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS) -o $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(UNIDEPS) # $(INCLUDEDIR)/%.h
	@mkdir -p $(OBJDIR)
	$(CC) -c $< $(FLAGS) -o $@

$(INCLUDES):

$(UNIDEPS):
	@touch $@

.PHONY: clean
clean: 
	rm -rf $(OBJDIR) $(TARGET) debug

.PHONY: debug
debug: FLAGS += -g -D DEBUG
debug: clean
debug: $(TARGET)
