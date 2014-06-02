SHELL = /bin/bash

EXENAME = simulator

SRCDIR = src
OBJDIR = obj

LLVM_CONFIG ?= llvm-config

override CXXFLAGS += -g -Wall -O3 -std=c++1y `$(LLVM_CONFIG) --cppflags`
override LDFLAGS += `$(LLVM_CONFIG) --ldflags --libs core support target executionengine jit native` -ltinfo -ldl

SRCS := $(shell find $(SRCDIR) -name '*.cpp')
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(SRCS))

NODEPS := clean

.PHONY: clean

all: $(EXENAME)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEPS)
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst $(OBJDIR)/%.o,$(OBJDIR)/%.d,$@)'

$(EXENAME): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(EXENAME)
