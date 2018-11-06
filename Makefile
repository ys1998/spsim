PRODUCT := spsim
INCDIR := inc
SRCDIR := src
OBJDIR := obj
DEPDIR := dep

CXX := g++
LINKER := g++
INCDIRS := -I$(INCDIR)
CXXFLAGS := -std=c++17 -Wall -Wextra #-O3

SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCFILES))
DEPFILES := $(patsubst $(SRCDIR)/%.cpp,$(DEPDIR)/%.d,$(SRCFILES))

$(shell mkdir -p $(OBJDIR) $(DEPDIR))

all: $(PRODUCT)

$(PRODUCT): $(OBJFILES)
	$(LINKER) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCDIRS) -c $< -o $@

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	$(CXX) $(INCDIRS) -MMD -MP $< | sed -e 's%^%$@ %' -e 's% % $(OBJDIR)/%'\ > $@

ifneq ($(MAKECMDGOALS), clean)
	-include $(DEPFILES)
endif

clean:
	rm -rf $(PRODUCT) $(OBJDIR) $(DEPDIR)
