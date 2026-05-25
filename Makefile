# Dataplane Makefile
# Phases 0-6+ progression toward a userspace L2/L3 router/firewall

CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c99 -D_POSIX_C_SOURCE=200809L
CFLAGS += -I./include
LDFLAGS := -lm

# Debug/Release builds
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2
endif

# Build directories
BUILD_DIR := build
BIN_DIR := bin

# Phase 0 sources (currently active)
PHASE0_SOURCES := \
    src/main.c \
    src/dp_buffer.c \
    src/dp_netdev.c \
    src/util/log.c

PHASE0_OBJECTS := $(addprefix $(BUILD_DIR)/, $(PHASE0_SOURCES:.c=.o))
PHASE0_BIN := $(BIN_DIR)/dataplane-phase0

# Target binary (currently Phase 0)
TARGET := $(PHASE0_BIN)

# Default target
.PHONY: all
all: $(TARGET)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

# Compile object files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link Phase 0 binary
$(PHASE0_BIN): $(PHASE0_OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	@echo "✓ Built: $@"

# Phony targets for building/running specific phases
.PHONY: phase0
phase0: $(PHASE0_BIN)

# Run Phase 0 (requires root for TAP interface)
.PHONY: run-phase0
run-phase0: phase0
	sudo $(PHASE0_BIN) tap0

# Build with debug symbols
.PHONY: debug
debug: DEBUG=1
debug: clean $(TARGET)
	@echo "✓ Debug build complete"

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Cleaned"

# Full clean (including any generated config/state)
.PHONY: distclean
distclean: clean
	@echo "✓ Distribution clean"

# Help target
.PHONY: help
help:
	@echo "Dataplane Build System"
	@echo ""
	@echo "Phases:"
	@echo "  Phase 0: Basic I/O loop (CURRENT)"
	@echo "  Phase 1: Ethernet L2 parsing"
	@echo "  Phase 1.5: ARP handling"
	@echo "  Phase 2: IPv4 parsing and routing"
	@echo "  Phase 3: Multiple interfaces and forwarding"
	@echo "  Phase 4: Firewall/ACLs"
	@echo "  Phase 5: NAT and connection tracking"
	@echo "  Phase 6+: Performance and observability"
	@echo ""
	@echo "Targets:"
	@echo "  make                - Build current phase binary"
	@echo "  make debug          - Build with debug symbols (-g -O0)"
	@echo "  make phase0         - Build Phase 0 (I/O loop)"
	@echo "  make run-phase0     - Run Phase 0 (sudo required)"
	@echo "  make clean          - Remove build artifacts"
	@echo "  make distclean      - Full clean"
	@echo "  make help           - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  make DEBUG=1        - Enable debug mode"
	@echo "  make -j4            - Parallel build (4 jobs)"

# Verbose output (optional)
ifdef VERBOSE
    MAKEFLAGS :=
else
    MAKEFLAGS += --silent
endif

# Prevent deletion of intermediate files
.SECONDARY: $(PHASE0_OBJECTS)

# Show target info
.PHONY: info
info:
	@echo "Project: chris_dataplane"
	@echo "Current Phase: 0 (Basic I/O)"
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "Build dir: $(BUILD_DIR)"
	@echo "Binary dir: $(BIN_DIR)"
	@echo "Target: $(TARGET)"
