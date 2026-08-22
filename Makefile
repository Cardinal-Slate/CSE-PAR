# Makefile — CSE-PAR: parallelization as a generic schedule (map an opaque op over items). Spine + DSA.
#
# Build the deps first: make -C ../CardinalSlate lib && make -C ../CSE-DSA lib

SPINE ?= ../CardinalSlate
DSA   ?= ../CSE-DSA
CC    ?= clang
CFLAGS := -std=c11 -Iinclude -I$(DSA)/include -I$(SPINE)/include -O2 -Wall -Wextra

OUT     := build
NAME    := par
HDRS    := include/cse/$(NAME).h
SRCS    := $(wildcard src/*.c)
OBJS    := $(patsubst src/%.c,$(OUT)/%.o,$(SRCS))
DEPLIBS := $(DSA)/build/libcse-dsa.a $(SPINE)/build/libslate.a

.PHONY: all check clean lib
all: check lib

$(OUT):
	@mkdir -p $(OUT)

$(OUT)/types.stamp: $(HDRS) $(SRCS) | $(OUT)
	@bad=$$(grep -rnE '\b(int|long|short|size_t|unsigned|char|bool|float|double)\b|void[[:space:]]*\*|stdint' include src 2>/dev/null || true); \
	  if [ -n "$$bad" ]; then printf "  %-10s C TYPE FOUND\n" "$(NAME):"; printf '%s\n' "$$bad" | sed 's/^/    /'; exit 1; \
	  else printf "  %-10s only slate · generic over an op\n" "$(NAME):"; fi; touch $@

$(OUT)/standalone.stamp: $(HDRS) | $(OUT)
	@for h in $(HDRS); do rel=$${h#include/}; printf '#include "%s"\nint main(void){return 0;}\n' "$$rel" > $(OUT)/one.c; \
	  $(CC) $(CFLAGS) -fsyntax-only $(OUT)/one.c || exit 1; done; touch $@

$(OUT)/%.o: src/%.c $(HDRS) | $(OUT)
	@$(CC) $(CFLAGS) -c $< -o $@

lib: $(OUT)/libcse-$(NAME).a
$(OUT)/libcse-$(NAME).a: $(OBJS) | $(OUT)
	@ar rcs $@ $(OBJS)

$(OUT)/test_$(NAME): tests/$(NAME).c $(OBJS) | $(OUT)
	@$(CC) $(CFLAGS) tests/$(NAME).c $(OBJS) $(DEPLIBS) -o $@

check: $(OUT)/types.stamp $(OUT)/standalone.stamp $(OUT)/test_$(NAME)
	@echo "== cse-$(NAME) =="; out=$$($(OUT)/test_$(NAME) 2>&1); st=$$?; \
	  if [ $$st -ne 0 ] || printf '%s' "$$out" | grep -q FAIL; then printf '%s\n' "$$out" | sed 's/^/  /'; exit 1; \
	  else printf '%s\n' "$$out" | sed 's/^/  /'; echo "== ALL PASS =="; fi

clean:
	@rm -rf $(OUT)
