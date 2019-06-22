#Makefile

BUILD:= Build
PROGS:= Othello
SOURS:= main.cpp Othello.cpp GA.cpp
OBJES:= $(SOURS:%.cpp=$(BUILD)/%.o)

.PHONY: all
all: $(BUILD) $(PROGS)

$(BUILD):
	mkdir -p $@

$(PROGS): $(OBJES)
	g++ -o $@ $^

$(BUILD)/%.o:%.cpp
	g++ -Wall -O2 -c -o $@ -MMD -MP $<


.PHONY: clean
clean:
	rm -f $(PROGS)
	rm -f $(OBJES)
	rm -rf $(BUILD)
